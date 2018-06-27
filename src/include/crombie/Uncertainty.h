#ifndef CROMBIE_UNCERTAINTY_H
#define CROMBIE_UNCERTAINTY_H

#include <string>
#include <map>
#include <vector>
#include <istream>
#include <regex>
#include <algorithm>
#include <iterator>

#include "crombie/Types.h"
#include "crombie/Debug.h"
#include "crombie/Misc.h"
#include "crombie/Parse.h"

namespace crombie {
  namespace Uncertainty {

    class UncertaintyInfo {
    public:
      /// Change an to match the desired uncertainty 
      std::string expr (const std::string& key, const std::string& inexpr, const bool isup) const;

      /// Get all the needed expressions to cover the loaded uncertainties
      template<typename A, typename... Args> Types::strings exprs (A arg, Args... args) const;
      Types::strings exprs (const Types::strings& args) const;
      Types::strings exprs (const std::string& arg) const;

      /// Get a list of the uncertainty names
      Types::strings systematics () const {
        return Misc::comprehension<std::string>
          (affected_branches, [] (auto& i) {return i.first;});
      }

    private:
      /// Key is the name of the uncertainty, the first member of the pair is how the branches are named.
      Types::map<std::pair<std::string, Types::strings>> affected_branches;
      friend std::istream& operator>>(std::istream& is, UncertaintyInfo& config);
    };


    std::istream& operator>>(std::istream& is, UncertaintyInfo& config) {

      for (auto& line : Parse::parse(is)) {
        // Check for shell input and tokenize line
        auto tokens = Misc::tokenize(line);

        // Front token is the name
        auto eq_pos = tokens.front().find('=');
        auto sysname = eq_pos == std::string::npos ? tokens.front() : tokens.front().substr(0, eq_pos);
        auto branchname = eq_pos == std::string::npos ? tokens.front() : tokens.front().substr(eq_pos, tokens.front().size() - eq_pos);

        // Create the list of branches
        Types::strings branches{};
        auto iter = tokens.begin();
        for (++iter; iter != tokens.end(); ++iter)
          branches.push_back(*iter);

        config.affected_branches.insert({sysname, {branchname, std::move(branches)}});
      }
      return is;
    }

    std::string UncertaintyInfo::expr(const std::string& key, const std::string& inexpr, const bool isup) const {
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

    Types::strings UncertaintyInfo::exprs(const std::string& arg) const {
      Types::strings output{};
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

    Types::strings UncertaintyInfo::exprs(const Types::strings& args) const {
      Types::strings output {};
      for (auto& arg : args) {
        auto single = exprs(arg);
        output.insert(output.end(), std::make_move_iterator(single.begin()), std::make_move_iterator(single.end()));
      }
      return output;
    }

    template<typename A, typename... Args> Types::strings UncertaintyInfo::exprs(A arg, Args... args) const {
      Types::strings output {};

      auto single = exprs(arg);
      output.insert(output.end(), std::make_move_iterator(single.begin()), std::make_move_iterator(single.end()));

      auto more = exprs(args...);
      output.insert(output.end(), std::make_move_iterator(more.begin()), std::make_move_iterator(more.end()));

      return output;
    }


  }
}

#endif
