#ifndef CROMBIE_EVENTDUMP_H
#define CROMBIE_EVENTDUMP_H

#include <list>

#include "crombie/Types.h"
#include "crombie/FileConfig.h"
#include "crombie/LoadTree.h"

namespace crombie {
  namespace EventDump {

    using SingleOut = std::vector<std::vector<double>>;
    using SingleFunc = FileConfig::MapFunc<SingleOut>;

    FileConfig::MapFunc<SingleOut>
      SingleFile(const std::string& cut,
                 const Types::strings& exprs);

    SingleOut Merge (const FileConfig::ToMerge<SingleOut>& outputs);

    // IMPLEMENTATIONS BELOW HERE //

    SingleFunc SingleFile(const std::string& cut,
                          const Types::strings& exprs) {
      return SingleFunc {
        [&cut, &exprs] (const FileConfig::FileInfo& info) {
          LoadTree::Tree loaded{info.name, cut, exprs};
          SingleOut output {};
          auto& track_cut = loaded.result(cut);
          std::vector<double*> results;
          for (auto& expr : exprs)
            results.push_back(&(loaded.result(expr)));

          std::vector<double> eventout;

          while (loaded.next()) {
            if (track_cut) {
              eventout.clear();
              for (auto* val : results)
                eventout.push_back(*val);

              output.push_back(eventout);
            }
          }
          return output;
        }
      };
    }

    SingleOut Merge (const FileConfig::ToMerge<SingleOut>& outputs) {
      SingleOut output {};
      for (auto& outs : outputs) {
        // We don't care about the key
        for (auto& single : outs.second)
          output.insert(output.end(), single.begin(), single.end());
      }

      // Sort vector of vectors recursively
      sort(output.begin(), output.end(),
           [] (auto& a, auto& b) {
             for (unsigned i = 0; i < a.size(); ++i) {
               if (a[i] == b[i])
                 continue;
               return a[i] < b[i];
             }
             return false;
           });

      return output;
    }
  }
}

#endif
