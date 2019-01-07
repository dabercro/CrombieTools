#ifndef CROMBIE_LOADTREE_H
#define CROMBIE_LOADTREE_H

#include <string>
#include <vector>
#include <regex>
#include <exception>

#include "crombie/Types.h"
#include "crombie/Misc.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreeFormula.h"

namespace crombie {
  namespace LoadTree {

    std::mutex rootlock;

    class Formulas {
    public:
      Formulas (TTree* tree) : tree{tree} {}
      // I hate ROOT: this causes things to crash
      /* virtual ~Formulas () { */
      /*   for (auto& form : forms) */
      /*     delete form.second; */
      /*   forms.clear(); */
      /* } */

      void add (const std::string& expr) {
        Debug::Debug(__PRETTY_FUNCTION__, "Adding formula", expr);
        if (forms.find(expr) == forms.end()) {
          TTreeFormula* form = new TTreeFormula(expr.data(), expr.data(), tree);
          forms[expr] = std::make_pair(0.0, form);
        }
      }
      void add (const Types::strings& exprs) {
        for (auto expr : exprs)
          add(expr);
      }
      template<typename A, typename... Args> void add(const A& expr, Args... args) {
        add(expr);
        add(args...);
      }

      /// Get a reference to the result for a given formula
      double& result (const std::string& expr) {
        auto i_form = forms.find(expr);
        if (i_form != forms.end())
          return i_form->second.first;
        throw std::logic_error{expr + ": Asking for expression that wasn't loaded"};
      }

      /// Evaluates all of the formulas and stores the results
      void eval () {
        for (auto& form : forms)
          form.second.first = form.second.second->EvalInstance();
      }

    private:
      TTree* tree;
      Types::map<std::pair<double, TTreeFormula*>> forms {}; /// Key to map is the full formula
    };

    /**
       Class that holds a loaded tree from a file.
       The tree name is given in the environment variable `tree`, and defaults to `"events"`.
     */
    class Tree {
    public:
      /**
         Constructor of a loaded tree. Note that copy and move constructors won't work because of the TFile member.
         @param infile is the name of the input file to read
         @param args is a variable number of arguments that can be of type ``std::string`` or ``crombie::Types::strings>``.
      */
      template<typename... Args> Tree(const std::string& infile, Args... args);
      ~Tree () { file->Close(); delete file; }

      /// Loads the next event into memory and evaluates all of the formulas
      bool next();
        
      /// Get a reference to the result for a given formula
      double& result (const std::string& expr) { return forms.result(expr); }

      /**
         Get a bare pointer to a TObject inside of this file.
         @param C is the type of pointer you would like to get
         @param name is the name of the object, searched for with TFile::Get
      */
      template<typename C> C* get(const std::string& name);

    private:
      TFile* file;         ///< The TFile that is being read
      TTree* tree;         ///< Holds the pointer to the tree
      long long nentries;  ///< Total number of events in the tree
      long long ientry{0}; ///< Which entry are we on
      Formulas forms;      ///< Holds formulas initialized with
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

      void add_branches(std::set<std::string>& needed, TTree& tree, const Types::strings& exprs) {
        for (auto expr : exprs)
          add_branches(needed, tree, expr);
      }

      template<typename A, typename... Args> void add_branches(std::set<std::string>& needed, TTree& tree, const A& expr, Args... args) {
        add_branches(needed, tree, expr);
        add_branches(needed, tree, args...);
      }

      /// Get the branches needed from the TTree to satisfy args
      template<typename... Args> std::set<std::string> needed_branches(TTree& tree, Args... args) {
        std::set<std::string> needed;
        add_branches(needed, tree, args...);
        return needed;
      }

    }

    template<typename... Args> Tree::Tree(const std::string& infile, Args... args)
      : file{TFile::Open(infile.data())} {
      if (not file)
        std::runtime_error(std::string("Error opening file ") + infile);

      tree = get<TTree>(Misc::env("tree", "events"));
      nentries = tree->GetEntries();
      forms = Formulas(tree);

      tree->SetBranchStatus("*", 0);
      auto needed = needed_branches(*tree, args...);
      for (auto need : needed)
        tree->SetBranchStatus(need.data(), 1);

      rootlock.lock();
      forms.add(args...);
      rootlock.unlock();
    }

    bool Tree::next() {
      if (ientry == nentries)
        return false;
      tree->GetEntry(ientry++);
      forms.eval();
      return true;
    }

    template<typename C> C* Tree::get(const std::string& name) {
      auto* obj = dynamic_cast<C*>(file->Get(name.data()));
      if (not obj)
        throw std::runtime_error(std::string("Could not find object '") + name + "' in " + file->GetName());
      return obj;
    }

  }
}

#endif
