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

    /**
       The information to hold the information for a plotter to implement a single systematic plot
     */
    class SysInfo {
    public:
    SysInfo(const std::string& key, const std::string& suff, const unsigned bin = 1)
      : key{key}, suff{suff}, bin{bin} {}
      const std::string key {};
      const std::string suff {};
      const unsigned bin {};
    };


    class UncertaintyInfo {
    public:
      /**
         Change the expression for an uncertainty given by the key.
         @param key The name of the uncertainty switched to. This pulls out the branch name
         @param inexpr The expression to change in order to match the uncertainty
         @param suff The suffix to change to, typically "Up" or "Down", but can be any envelope expression too
       */
      std::string expr (const std::string& key, const std::string& inexpr, const std::string& suff) const;

      /// Get all the needed expressions to cover the loaded uncertainties, along with the original expressions
      template<typename A, typename... Args> Types::strings exprs (A arg, Args... args) const;
      Types::strings exprs (const Types::strings& args) const;
      Types::strings exprs (const std::string& arg) const;

      /// Get a list of the uncertainty names
      Types::strings systematics () const;

      /// Returns a list maintained by this object of parameters needed to implement all sysematics
      const std::list<SysInfo>& full_systematic_infos () const { return full_infos; }

    private:
      /// Key is the name of the uncertainty, the first member of the pair is how the branches are named.
      Types::map<std::pair<std::string, Types::strings>> updown_branches;

      class Envelope {
      public:
      Envelope(const std::string& branch) : branch{branch} {}
        /// Name of the branch to change in expressions
        std::string branch;
        ///< First element is the bin that is normalized, then new branch name
        std::vector<std::pair<unsigned, std::string>> otherbranches {};
      };

      /// Key is the name of the uncertainty, the first member of the pair is which bin to use for total events.
      Types::map<Envelope> env_branches;
      /// A central list of all the systematics needed to make plots. Maintained here to avoid needing to duplicate it.
      std::list<SysInfo> full_infos;

      friend std::istream& operator>>(std::istream& is, UncertaintyInfo& config);
    };


    std::istream& operator>>(std::istream& is, UncertaintyInfo& config) {

      for (auto& line : Parse::parse(is)) {
        // Check for shell input and tokenize line
        auto tokens = Misc::tokenize(line);

        // Front token is the name

        // Get if envelope
        bool is_env = tokens.front().back() == ':';
        if (is_env) {
          tokens.front().pop_back();
          auto branchname = tokens.back();
          // Go through rest of tokens
          auto newenv = config.env_branches.insert({tokens.front(), {branchname}});
          auto iter = tokens.begin();
          for (auto splitpos = (++iter)->find(':');
               iter != tokens.end() and splitpos != std::string::npos;
               splitpos = (++iter)->find(':')) {

            unsigned bin = std::stoi(iter->substr(0, splitpos));
            auto suff = iter->substr(splitpos+1);
            newenv.first->second.otherbranches.emplace_back(bin, branchname + "_" + suff);

            config.full_infos.emplace_back(newenv.first->first, suff, bin);
          }
          
        }
        else { // If not envelope
          auto eq_pos = tokens.front().find('=');
          auto sysname = eq_pos == std::string::npos ? tokens.front() : tokens.front().substr(0, eq_pos);
          auto branchname = eq_pos == std::string::npos ? tokens.front() : tokens.front().substr(eq_pos, tokens.front().size() - eq_pos);

          // Create the list of branches
          Types::strings branches{};
          auto iter = tokens.begin();
          for (++iter; iter != tokens.end(); ++iter)
            branches.push_back(*iter);

          config.updown_branches.insert({sysname, {branchname, std::move(branches)}});
          for (auto& updown : {"Up", "Down"})
            config.full_infos.emplace_back(sysname, updown);
        }
      }
      return is;
    }


    std::string UncertaintyInfo::expr(const std::string& key, const std::string& inexpr, const std::string& suff) const {
      std::string output = inexpr;
      std::string name {};
      Types::strings branches {};

      bool isupdown = (suff == "Up" or suff == "Down");

      if (isupdown) {
        auto& info = updown_branches.at(key);
        name = info.first;
        branches = info.second;
      }
      else
        branches.push_back(env_branches.at(key).branch);

      for (auto& branch : branches) {
        std::regex expr {std::string("\\b") + branch + "\\b"};
        std::string replace = branch + '_' + name + suff;
        output = std::regex_replace(output, expr, replace);
      }

      return output;
    }


    Types::strings UncertaintyInfo::exprs(const std::string& arg) const {
      Types::strings output{arg};
      for (auto& unc : updown_branches) {
        for (bool isup : {true, false}) {
          // Just rvalue ref, because we'll either stick it in output, or drop
          auto&& check = expr(unc.first, arg, isup ? "Up" : "Down");
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


    Types::strings UncertaintyInfo::systematics () const {
      auto comp = [] (auto& branches) {
        return Misc::comprehension<std::string>(branches, [] (auto& i) {return i.first;});
      };

      auto output = comp(updown_branches);
      auto other = comp(env_branches);

      output.insert(output.end(),
                    std::make_move_iterator(other.begin()),
                    std::make_move_iterator(other.end()));

      return output;
    }

  }
}

#endif
