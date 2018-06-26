#ifndef CROMBIE_PARSE_H
#define CROMBIE_PARSE_H

/**
   This header file includes common functions that are used to interpret configuration files
 */

#include <istream>
#include <regex>
#include <string>

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

      /// Filter lines according to Parser expectations
      std::string parse (const std::string& line) {
        auto output = 
          do_shell
          (remove_comments
           (line));
        Debug::Debug(__PRETTY_FUNCTION__, "IN:", line, "\nOUT:", output);
        return output;
      }
    }

    /// Parse an input stream and return a vector of parsed lines, with empty lines removed
    std::vector<std::string> parse (std::istream& is) {
      std::vector<std::string> output{};
      for (std::string raw; std::getline(is, raw); ) {
        auto&& line = parse(raw);
        if (line.size())
          output.push_back(line);
      }
      return output;
    }

  }
}

#endif
