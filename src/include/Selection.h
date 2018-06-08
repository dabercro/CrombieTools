#ifndef CROMBIE_CUTCONFIG_H
#define CROMBIE_CUTCONFIG_H

#include <map>
#include <string>
#include <regex>
#include <iterator>

#include "Misc.h"

namespace Crombie {
  namespace Selection {

    class Selection {
    public:
    Selection(const std::string cut, const std::string data, const std::string mc)
      : cut{cut}, data{data}, mc{mc} {}

      const std::string cut;    ///< The cut that plots must pass
      const std::string data;   ///< The data weight for plots
      const std::string mc;     ///< The mc weight for plots
    };

    /**
       Returns the a cut with a variable taken out.
       The function only removes expressions where the variable name is to the left of an operator.
    */
    std::string nminus1(const std::string& var, const std::string& cut) {
      std::regex expr{std::string("\\b") + var + "\\b\\s*[=<>]*\\s*-?[\\d\\.]+"};
      std::string output = cut;
      auto begin = std::sregex_iterator(cut.cbegin(), cut.cend(), expr);
      auto end = std::sregex_iterator();
      for (auto iter = begin; iter != end; ++iter)
        output.replace(iter->position(), iter->length(), "(1)");
      return output;
    }

    /// Reads a configuration file that maps regions to selections
    std::map<const std::string, const Selection> read(const char* config) {
      std::ifstream input {config};

      std::map<const std::string, const Selection> output;

      using symbols = std::map<std::string, std::string>;
      symbols sym;
      symbols::key_type current_symbol;
      symbols::mapped_type joiner;

      std::regex expr{"^([^\\s]*)\\s*([^\\s\\']*)\\s+(.+)$"};
      std::smatch matches;

      auto parse_cut = [&sym] (const std::string& cut) {
        if (cut.find("env'") == 0) {
          auto split_loc = cut.find(':');
          return Misc::env(cut.substr(4, split_loc - 4),
                           cut.substr(split_loc + 1, cut.size() - split_loc - 2));
        }
        if (cut[0] == '\'')
          return cut.substr(1, cut.size() - 2);
        return sym[cut];
      };

      for (std::string raw; std::getline(input, raw); ) {
        // Strip out comments
        std::string line {raw.substr(0, raw.find("! "))};
        if (line.size()) {
          if (line[0] == ':') {
            auto tokens = Misc::tokenize(line);
            if (tokens.size() == 4)
              tokens.push_back("'1'");

            if (tokens.size() != 5)
              throw std::runtime_error{"Problem with selection line " + line};
            // First token is ':'
            output.emplace(std::make_pair(tokens[1],
                                          Selection(parse_cut(tokens[2]),
                                                    parse_cut(tokens[3]),
                                                    parse_cut(tokens[4]))
                                          ));
          }
          else if (std::regex_search(line, matches, expr)) {
            if (matches[1].length()) {
              current_symbol = matches[1];
              joiner = matches[2];
            }
            sym[current_symbol] += (matches[2].length() ? "" : joiner) + parse_cut(matches[3]);
          }
        }
      }
      return output;
    }

  }
}

#endif
