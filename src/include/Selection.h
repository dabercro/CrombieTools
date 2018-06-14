#ifndef CROMBIE_CUTCONFIG_H
#define CROMBIE_CUTCONFIG_H

#include <fstream>
#include <map>
#include <string>
#include <regex>
#include <iterator>

#include "Debug.h"
#include "Misc.h"

namespace Crombie {
  namespace Selection {

    class Selection {
    public:
    Selection(const std::string cut, const std::string mc, const std::string data)
      : cut{cut}, data{data}, mc{mc} {
        Debug::Debug("Selection:", cut, "---", mc, "---", data);
      }

      const std::string cut;    ///< The cut that plots must pass
      const std::string data;   ///< The data weight for plots
      const std::string mc;     ///< The mc weight for plots
    };


    class SelectionConfig {
    public:
    SelectionConfig(const std::string& mchistname)
      : mchistname{mchistname} {}

      const std::string mchistname;  ///< The mchist to normalize the weight to

      using Selections = std::map<const std::string, const Selection>;
      Selections selections;

      friend std::istream& operator>>(std::istream& is, SelectionConfig& config);
    };

    std::istream& operator>>(std::istream& is, SelectionConfig& config);

    /**
       Returns the a cut with a variable taken out.
       The function only removes expressions where the variable name is to the left of an operator.
    */
    std::string nminus1(const std::string& var, const std::string& cut);

    /// Reads a configuration file that maps regions to selections
    /// Reads a configuration file for file info
    SelectionConfig read(const std::string& mchistname, const char* config) {
      SelectionConfig output {mchistname};
      std::ifstream input {config};
      input >> output;
      return output;
    }


    // IMPLEMENTATIONS BELOW HERE //



    std::string nminus1(const std::string& var, const std::string& cut) {
      std::regex expr{std::string("\\b") + var + "\\b\\s*[=<>]*\\s*-?[\\d\\.]+"};
      std::string output = cut;
      auto begin = std::sregex_iterator(cut.cbegin(), cut.cend(), expr);
      auto end = std::sregex_iterator();
      for (auto iter = begin; iter != end; ++iter)
        output.replace(iter->position(), iter->length(), "(1)");
      return output;
    }


    std::istream& operator>>(std::istream& is, SelectionConfig& config) {
      using symbols = std::map<std::string, std::string>;
      symbols sym;

      auto parse_cut = [&sym] (const std::string& cut) {
        if (cut.find("env'") == 0) {
          auto split_loc = cut.find(':');
          return Misc::env(cut.substr(4, split_loc - 4),
                           cut.substr(split_loc + 1, cut.size() - split_loc - 2));
        }
        if (cut[0] == '\'')
          return cut.substr(1, cut.size() - 2);
        try {
          return sym.at(cut);
        }
        catch(const std::out_of_range& e) {
          std::cerr << "Exception locating " << cut << std::endl;
          throw e;
        }
      };

      symbols::key_type current_symbol;
      symbols::mapped_type joiner;

      std::regex expr{"^([^\\s]*)\\s*([^\\s\\']*)\\s+(.+)$"};
      std::smatch matches;

      for (std::string raw; std::getline(is, raw); ) {
        // Strip out comments
        std::string line {raw.substr(0, raw.find("! "))};
        if (line.size()) {
          if (line[0] == ':') {
            auto tokens = Misc::tokenize(line);
            if (tokens.size() == 3)
              tokens.push_back("'1.0'");

            if (tokens.size() != 4)
              throw std::runtime_error{"Problem with selection line " + line};
            // First token is ':'
            config.selections.emplace(std::make_pair(tokens[1],
                                                     Selection(parse_cut(tokens[1]), // Cut
                                                               parse_cut(tokens[2]), // MC weight
                                                               parse_cut(tokens[3])) // Data weight
                                                     ));
          }
          else if (std::regex_search(line, matches, expr)) {
            if (matches[1].length()) {
              current_symbol = matches[1];
              joiner = matches[2];
              if (joiner.size())
                joiner = std::string(" ") + joiner + " ";
            }
            sym[current_symbol] += (matches[2].length() ? "" : joiner) + parse_cut(matches[3]);
          }
        }
      }
      return is;
    }

  }
}

#endif
