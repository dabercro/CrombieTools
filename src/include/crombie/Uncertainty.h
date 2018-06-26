#ifndef CROMBIE_UNCERTAINTY_H
#define CROMBIE_UNCERTAINTY_H

#include <string>
#include <map>
#include <vector>
#include <istream>
#include <regex>
#include <algorithm>
#include <iterator>

#include "crombie/Debug.h"
#include "crombie/Misc.h"

namespace crombie {
  namespace Uncertainty {

    class UncertaintyInfo {
    public:
      /// Change an to match the desired uncertainty 
      std::string expr (const std::string& key, const std::string& inexpr, const bool isup);
      /// Get all the needed expressions to cover the loaded uncertainties
      template<typename A, typename... Args> std::vector<std::string> exprs (A arg, Args... args);
      std::vector<std::string> exprs (const std::vector<std::string>& args);
      std::vector<std::string> exprs (const std::string& arg);
    private:
      /// Key is the name of the uncertainty, the first member of the pair is how the branches are named.
      std::map<std::string, std::pair<std::string, std::vector<std::string>>> affected_branches;
      friend std::istream& operator>>(std::istream& is, UncertaintyInfo& config);
    };


    std::istream& operator>>(std::istream& is, UncertaintyInfo& config) {

      auto parse_for_shell = [] (const std::string& line) {
        std::regex expr {"`([^`]+)`"};
        std::smatch match;
        std::string output = line;
        while (std::regex_search(output, match, expr))
          output = output.replace(match.position(), match.position() + match.length(), Misc::shell(match[1]));

        return output;
      };

      for (std::string raw; std::getline(is, raw); ) {
        Debug::Debug(__PRETTY_FUNCTION__, raw);
        // Strip out comments
        std::string line {raw.substr(0, raw.find("! "))};
        if (line.size()) {
          // Check for shell input and tokenize line
          auto tokens = Misc::tokenize(parse_for_shell(line));

          // Front token is the name
          auto eq_pos = tokens.front().find('=');
          auto sysname = eq_pos == std::string::npos ? tokens.front() : tokens.front().substr(0, eq_pos);
          auto branchname = eq_pos == std::string::npos ? tokens.front() : tokens.front().substr(eq_pos, tokens.front().size() - eq_pos);

          // Create the list of branches
          std::vector<std::string> branches{};
          auto iter = tokens.begin();
          for (++iter; iter != tokens.end(); ++iter)
            branches.push_back(*iter);

          config.affected_branches.insert({sysname, {branchname, std::move(branches)}});

        }
      }
      return is;
    }

    std::string UncertaintyInfo::expr(const std::string& key, const std::string& inexpr, const bool isup) {
      auto& info = affected_branches.at(key);
      auto& name = info.first;
      auto& branches = info.second;
      std::string output = inexpr;

      for (auto& branch : branches) {
        std::regex expr {std::string("\\b") + branch + "\\b"};
        std::string replace = branch + '_' + name + (isup ? "Up" : "Down");
        output = std::regex_replace(output, expr, replace);
      }

      return output;
    }

    std::vector<std::string> UncertaintyInfo::exprs(const std::string& arg) {
      std::vector<std::string> output{};
      for (auto& unc : affected_branches) {
        for (bool isup : {true, false}) {
          // Just rvalue ref, because we'll either stick it in output, or drop
          auto&& check = expr(unc.first, arg, isup);
          if (check == arg)
            break;
          output.push_back(check);
        }
      }
      return output;
    }

    std::vector<std::string> UncertaintyInfo::exprs(const std::vector<std::string>& args) {
      std::vector<std::string> output {};
      for (auto& arg : args) {
        auto single = exprs(arg);
        output.insert(output.end(), std::make_move_iterator(single.begin()), std::make_move_iterator(single.end()));
      }
      return output;
    }

    template<typename A, typename... Args> std::vector<std::string> UncertaintyInfo::exprs(A arg, Args... args) {
      std::vector<std::string> output {};

      auto single = exprs(arg);
      output.insert(output.end(), std::make_move_iterator(single.begin()), std::make_move_iterator(single.end()));

      auto more = exprs(args...);
      output.insert(output.end(), std::make_move_iterator(more.begin()), std::make_move_iterator(more.end()));

      return output;
    }


  }
}

#endif
