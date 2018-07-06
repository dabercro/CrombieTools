#ifndef CROMBIE_CUTFLOW_H
#define CROMBIE_CUTFLOW_H

#include "crombie/Types.h"
#include "crombie/FileConfig.h"
#include "crombie/LoadTree.h"

namespace crombie {
  namespace Cutflow {

    /// The contents of the cutflow is a vector of decreasing number of events
    using SingleOut = std::vector<unsigned long>;
    using SingleFunc = FileConfig::MapFunc<SingleOut>;


    namespace {
      // We need a reference to the result in a vector, so here's a wrapper class
      struct Reader {
        double& res;
      };
    }


    /// Takes a vectors of cuts and returns a parallel cutflow vector
    SingleFunc SingleFile(const Types::strings& cuts) {
      return SingleFunc {
        [&cuts] (const FileConfig::FileInfo& info) {
          LoadTree::Tree loaded{info.name, cuts};
          std::vector<struct Reader> readers;
          readers.reserve(cuts.size());
          SingleOut output(cuts.size());
          for (auto& cut : cuts)
            readers.push_back({loaded.result(cut)});

          while (loaded.next()) {
            for (unsigned i = 0; i < readers.size(); ++i) {
              if (not readers[i].res)
                break;
              ++output[i];
            }
          }

          return output;
        }
      };
    }


    /// Returns a cutflow vector
    SingleOut Merge (const FileConfig::ToMerge<SingleOut>& outputs) {
      SingleOut output {};
      for (auto& info : outputs) {
        // Add the LumiSelections in the vector
        for (auto& vec : info.second) {
          if (output.size()) {
            for (unsigned i = 0; i < output.size(); ++i)
              output[i] += vec[i];
          }
          else
            output = vec;
        }
      }
      return output;
    }

  }
}

#endif
