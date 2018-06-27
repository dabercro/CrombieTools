#ifndef CROMBIE_LUMI_H
#define CROMBIE_LUMI_H

#include <ostream>
#include <map>
#include <utility>
#include <list>
#include <cmath>

#include "crombie/Debug.h"
#include "crombie/FileConfig.h"
#include "crombie/LoadTree.h"
#include "crombie/Misc.h"

namespace crombie {
  namespace Lumi {

    /// Stores the lumi and run as a map to ranges
    class LumiSelection {
    public:
      /// Add a lumi range to this object
      void add(unsigned long run, std::pair<unsigned, unsigned> lumi);
      /// Add a single run, lumi to this object
      void add(unsigned long run, unsigned lumi);
      /// Merge other LumiSelection objects into this one
      void add(const LumiSelection& other);
    private:
      /// A store that is kept in order and merges lumis in
      std::map<unsigned long, std::list<std::pair<unsigned, unsigned>>> store;

      friend std::ostream& operator<<(std::ostream& os, const LumiSelection& selection);
    };

    std::ostream& operator<<(std::ostream& os, const LumiSelection& selection);

    /// Return the LumiSelection for a single file
    LumiSelection SingleRunner(const FileConfig::FileInfo& info);
    /// A functional for the FileConfig::runfiles to be happy
    FileConfig::MapFunc<LumiSelection> SingleFile {SingleRunner};

    /// Return the merged LumiSelection
    LumiSelection Merge(const FileConfig::ToMerge<LumiSelection>& outputs);


    // IMPLEMENTATIONS BELOW HERE //


    namespace {
      auto runname = Misc::env("runnum", "runNumber");
      auto luminame = Misc::env("luminum", "lumiNumber");
    }

    LumiSelection SingleRunner(const FileConfig::FileInfo& info) {
      LoadTree::Tree loaded{info.name, runname, luminame};

      auto& run = loaded.result(runname);
      auto& lumi = loaded.result(luminame);

      LumiSelection output;

      while (loaded.next())
        output.add(run, lumi);

      return output;
    }

    LumiSelection Merge(const FileConfig::ToMerge<LumiSelection>& outputs) {
      LumiSelection output {};
      // For each possible directory
      for (auto& info : outputs) {
        // Add the LumiSelections in the vector
        for (auto& sel : info.second)
          output.add(sel);
      }
      return output;
    }

    void LumiSelection::add(unsigned long run, std::pair<unsigned, unsigned> lumi) {
      Debug::Debug(__PRETTY_FUNCTION__, "Adding", run, lumi.first, lumi.second);

      auto& to_insert = store[run];

      auto before{to_insert.begin()};  // Iterator the beginning of the new pair should go
      auto after{before};              // Iterator where the end of the new pair should go

      for (; after != to_insert.end(); ++after) {
        if ((lumi.second + 1) < after->first)  // If would not merge higher, stop here
          break;
        if (before == after) {                  // If still finding, drag that along
          if (lumi.first > (after->second + 1))
            ++before;
        }
      }

      // Just need to insert
      if (before == after)
        to_insert.insert(before, lumi);
      else {
        // Otherwise, need best values
        auto insertion = std::make_pair(std::min(lumi.first, before->first),
                                        std::max(lumi.second, (--after)->second));
        // Placed in front of "before"
        to_insert.insert(before, insertion);
        // Remove the other values
        // Go forward one for after again, since we went backwards above for range
        to_insert.erase(before, ++after);
      }
    }

    void LumiSelection::add(unsigned long run, unsigned lumi) {
      add(run, std::make_pair(lumi, lumi));
    }

    void LumiSelection::add(const LumiSelection& other) {
      for (auto& run : other.store) {
        for (auto& pair : run.second)
          add(run.first, pair);
      }
    }

    std::ostream& operator<<(std::ostream& os, const LumiSelection& selection) {
      os << "{";
      bool startedrun = false;
      for (auto& run : selection.store) {
        bool startedlumi = false;
        if (startedrun)
          os << "], ";
        startedrun = true;
        os << '"' << run.first << '"' << ": [";
        for (auto& lumi : run.second) {
          if (startedlumi)
            os << ", ";
          startedlumi = true;
          os << "[" << lumi.first << ", " << lumi.second << "]";
        }
      }
      os << "]}" << std::endl;
      return os;
    }

  }
}

#endif
