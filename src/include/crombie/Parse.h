#ifndef CROMBIE_PARSE_H
#define CROMBIE_PARSE_H

/**
   This header file includes common functions that are used to interpret configuration files
 */

#include <istream>
#include <regex>
#include <string>
#include <vector>

#include "crombie/Types.h"
#include "crombie/Debug.h"
#include "crombie/Misc.h"

namespace crombie {
  namespace Parse {

    namespace {
      /// Extract shell commands from lines and replace them in the line
      std::string do_shell (const std::string& line) {
        std::regex expr {"`([^`]+)`"};
        std::smatch match;
        std::string output = line;
        while (std::regex_search(output, match, expr))
          output = output.replace(match.position(), match.position() + match.length(), Misc::shell(match[1]));

        return output;
      }

      /// Remove comments from lines
      std::string remove_comments (const std::string& line) {
        return line.substr(0, line.find("! "));
      }

      /// Substitute environment variables when needed
      std::string envsubstitution(const std::string& line) {
        auto startpos = line.find("env{");
        if (startpos != std::string::npos) {
          startpos += 4;         // Don't pick out "env{"
          auto splitpos = line.find(':', startpos);
          auto endpos = line.find('}', splitpos);
          if (splitpos != std::string::npos and endpos != std::string::npos) {
            auto envvar = line.substr(startpos, splitpos - startpos);
            splitpos += 1;       // Skip over ':'
            auto envdefault = line.substr(splitpos, endpos - splitpos);
            Debug::Debug(__PRETTY_FUNCTION__, envvar, envdefault);
            startpos -= 4;       // Now replace "env{"
            endpos += 1;         // And '}'
            auto output = line;
            return output.replace(startpos, endpos - startpos, Misc::env(envvar, envdefault));
          }
        }
        return line;
      }

      /// Filter lines according to Parser expectations
      std::string parse (const std::string& line) {
        auto output = do_shell(envsubstitution(line));
        Debug::Debug(__PRETTY_FUNCTION__, "IN:", line, "\nOUT:", output);
        return output;
      }

      /// Do the expansion of tokens
      std::string expand(const std::string &line) {
        std::string output = line;
        // Match the outer <> and the first {} pair you come to
        std::regex expand_op {"<(.*?)\\{([^\\}]*)\\}(.*?)>(?!>)"};
        std::smatch matches;
        while(std::regex_search(output, matches, expand_op)) {
          Debug::Debug(__PRETTY_FUNCTION__, output);
          std::string begin = matches[1];
          auto tokens = Misc::tokenize(matches[2]);
          std::string end = matches[3];
          std::string replace {};
          for (auto& token : tokens) {
            if (replace.size())
              replace += ' ';
            replace += begin + token + end;
          }
          Debug::Debug(__PRETTY_FUNCTION__, begin, end, replace);

          output.replace(matches.position(), matches.length(), replace);
        }
        return output;
      }

      /// Do the multi-line expansion
      std::vector<std::string> multiline(const std::string& line) {
        std::vector<std::string> output;
        output.push_back(line);
        bool need_checked = true;
        // 1st group is the main line to keep
        // 2nd group is the character to replace
        // 3rd group is the different replacements for each line
        std::regex multi_op {"(.*)\\s+;(\\S?)\\s+(.*)"};
        std::smatch matches;
        while(need_checked) {
          need_checked = false;
          for (auto check = output.begin(); check != output.end();) {
            if (std::regex_match(*check, matches, multi_op)) {
              need_checked = true;
              auto remove = check;
              std::string toreplace = matches[2];
              if (not toreplace.size())
                toreplace += '$';
              auto replacements = Misc::tokenize(matches[3]);

              for (auto& sub : replacements) {
                std::string newline = matches[1];
                for(auto pos = newline.find(toreplace); pos != std::string::npos; pos = newline.find(toreplace))
                  newline.replace(pos, 1, sub);
                output.insert(remove, std::move(newline));
              }

              output.erase(remove, ++check);
            }
            else  // Just move along
              ++check;
          }
        }
        return Misc::comprehension<std::string>(output, parse);
      }
    }

    /// Parse an input stream and return a list of parsed lines, with empty lines removed
    Types::strings parse (std::istream& is) {
      Types::strings output{};
      for (std::string raw; std::getline(is, raw); ) {
        Debug::Debug(__PRETTY_FUNCTION__, "Raw line:", raw);
        auto&& line = remove_comments(raw);
        if (line.size())
          for (auto&& expanded : multiline(expand(line))) {
            Debug::Debug(__PRETTY_FUNCTION__, "Expanded:", expanded);
            output.push_back(expanded);
          }
      }
      return output;
    }

  }
}

#endif
