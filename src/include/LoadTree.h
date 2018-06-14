#ifndef CROMBIE_LOADTREE_H
#define CROMBIE_LOADTREE_H

#include <string>
#include <vector>
#include <regex>

#include "Misc.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreeFormula.h"

namespace Crombie {
  namespace LoadTree {

    std::mutex rootlock;

    using strings = std::vector<std::string>;

    class Formulas {
    public:
      Formulas (TTree* tree) : tree{tree} {}
      // I hate ROOT, but this causes things to crash
      /* virtual ~Formulas () { */
      /*   for (auto& form : forms) */
      /*     delete form.second; */
      /*   forms.clear(); */
      /* } */

      void add (const std::string& expr) {
        if (forms.find(expr) == forms.end()) {
          TTreeFormula* form = new TTreeFormula(expr.data(), expr.data(), tree);
          forms[expr] = std::make_pair(0.0, form);
        }
      }
      void add (const strings& exprs) {
        for (auto expr : exprs)
          add(expr);
      }
      template<typename A, typename... Args> void add(const A& expr, Args... args) {
        add(expr);
        add(args...);
      }

      /// Get a reference to the result for a given formula
      double& result (const std::string& expr) { return forms[expr].first; }

      /// Evaluates all of the formulas and stores the results
      void eval () {
        for (auto& form : forms)
          form.second.first = form.second.second->EvalInstance();
      }

    private:
      TTree* tree {};
      std::map<std::string, std::pair<double, TTreeFormula*>> forms {};
    };

    namespace {

      /// Get the branches needed to evaluate an expression
      void add_branches(std::set<std::string>& needed, TTree& tree, const std::string& expr) {
        for (auto branch : *(tree.GetListOfBranches())) {
          auto name = branch->GetName();
          if (needed.find(name) == needed.end() && expr.find(name) != std::string::npos) {
            // Only save branches that are really in the expression
            std::regex regexpr {std::string("\\b") + name + "\\b"};
            std::smatch matches;
            if (std::regex_search(expr, matches, regexpr))
              needed.insert(name);
          }
        }
      }

      void add_branches(std::set<std::string>& needed, TTree& tree, const strings& exprs) {
        for (auto expr : exprs)
          add_branches(needed, tree, expr);
      }

      template<typename A, typename... Args> void add_branches(std::set<std::string>& needed, TTree& tree, const A& expr, Args... args) {
        add_branches(needed, tree, expr);
        add_branches(needed, tree, args...);
      }
    }

    /// Get the branches needed from the TTree to satisfy args
    template<typename... Args> std::set<std::string> needed_branches(TTree& tree, Args... args) {
      std::set<std::string> needed;
      add_branches(needed, tree, args...);
      return needed;
    }
    
    /// Get a container of formulas
    template<typename... Args> Formulas get_formula(TTree* tree, Args... exprs) {
      rootlock.lock();
      Formulas output {tree};
      output.add(exprs...);
      rootlock.unlock();
      return output;
    }


    /**
       Returns a pointer to a tree which is owned by the file and a map of TTreeFormula initialized on the tree
       @param infile is a reference to the input file that is holding the tree.
                     The name of the tree is extracted from the environment variable ``tree``.
       @param args is a varialbe number of arguments that can be of type ``std::string`` or ``std::vector<std::string>``.
    */
    template<typename... Args> std::pair<TTree*, Formulas> load_tree(TFile& infile, Args... args) {
      auto treename = Misc::env("tree", "events");
      auto* tree = static_cast<TTree*>(infile.Get(treename.data()));
      tree->SetBranchStatus("*", 0);
      auto needed = needed_branches(*tree, args...);
      for (auto need : needed)
        tree->SetBranchStatus(need.data(), 1);

      return std::make_pair(tree, get_formula(tree, args...));
    }

  }
}

#endif
