#ifndef CROMBIE_CUTCONFIG_H
#define CROMBIE_CUTCONFIG_H

#include <fstream>
#include <map>
#include <string>
#include <regex>
#include <iterator>

#include "crombie/Debug.h"
#include "crombie/Misc.h"
#include "crombie/Parse.h"

namespace crombie {
  namespace Selection {

    class Selection {
    public:
    Selection(const std::string& cut, const std::string& mc, const std::string& data, const bool blinded)
      : cut{cut}, data{data}, mc{mc}, blinded{blinded} {
        Debug::Debug(__PRETTY_FUNCTION__, "Selection:", cut, "---", mc, "---", data);
      }

      const std::string cut;     ///< The cut that plots must pass
      const std::string data;    ///< The data weight for plots
      const std::string mc;      ///< The mc weight for plots
      const bool blinded;        ///< Whether data should be blinded or not
    };


    using Selections = Types::map<const Selection>;


    class SelectionConfig {
    public:
    SelectionConfig(const std::string& mchistname)
      : mchistname{mchistname} {}

      const std::string mchistname;  ///< The mchist to normalize the weight to

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
    SelectionConfig read(const std::string& mchistname, const std::string& config) {
      SelectionConfig output {mchistname};
      std::ifstream input {config};
      input >> output;
      return output;
    }


    // IMPLEMENTATIONS BELOW HERE //


    std::string nminus1(const std::string& var, const std::string& cut) {
      std::regex expr{std::string("\\b") + var + "\\b\\s*[=<>]*\\s*-?[\\d\\.]+"};
      return std::regex_replace(cut, expr, "(1)");
    }

    std::istream& operator>>(std::istream& is, SelectionConfig& config) {
      using symbols = Types::map<std::string>;
      symbols sym;

      auto parse_cut = [&sym] (const std::string& cut) {
        if (cut[0] == '\'')
          return cut.substr(1, cut.size() - 2);
        try {
          return sym.at(cut);
        }
        catch(const std::out_of_range& e) {
          std::cerr << "Exception locating " << cut << std::endl;
          throw;
        }
      };

      auto current_symbol = symbols::key_type{};
      auto joiner = symbols::mapped_type{};

      std::regex expr{"^([^\\s]*)\\s*([^\\s\\']*)\\s+(.+)$"};
      std::smatch matches;

      for (auto& line : Parse::parse(is)) {
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
                                                             parse_cut(tokens[3]), // Data weight
                                                             line[1] == 'b')       // Should this be blind
                                                   ));
        }
        else if (std::regex_search(line, matches, expr)) {
          if (matches[1].length()) {
            current_symbol = matches[1];
            joiner = matches[2];
            if (joiner.size())
              joiner = decltype(joiner)(" ") + joiner + " ";
          }
          sym[current_symbol] += (matches[2].length() ? "" : joiner) + parse_cut(matches[3]);
        }
      }
      return is;
    }

  }
}

#endif
