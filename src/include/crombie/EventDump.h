#ifndef CROMBIE_EVENTDUMP_H
#define CROMBIE_EVENTDUMP_H

#include "crombie/FileConfig.h"
#include "crombie/LoadTree.h"

#include "TFile.h"

namespace crombie {
  namespace EventDump {

    using SingleOut = std::vector<std::vector<double>>;

    std::function<SingleOut(const FileConfig::FileInfo&)>
      SingleFile(const std::string& cut,
                 const std::vector<std::string>& exprs);

    SingleOut Merge (const std::vector<FileConfig::DirectoryInfo>& dirinfos,
                     const std::map<std::string, std::vector<SingleOut>>& outputs);

    // IMPLEMENTATIONS BELOW HERE //

    std::function<SingleOut(const FileConfig::FileInfo&)>
      SingleFile(const std::string& cut,
                 const std::vector<std::string>& exprs) {
      return std::function<SingleOut(const FileConfig::FileInfo&)> {
        [&cut, &exprs] (const FileConfig::FileInfo& info) {
          TFile infile {info.name.data()};
          auto loaded = LoadTree::load_tree(infile, cut, exprs);
          SingleOut output {};
          auto nevents = loaded.first->GetEntries();
          auto& track_cut = loaded.second.result(cut);
          std::vector<double*> results;
          for (auto& expr : exprs)
            results.push_back(&(loaded.second.result(expr)));

          std::vector<double> eventout;

          for (decltype(nevents) ievent = 0; ievent < nevents; ++ievent) {
            loaded.first->GetEntry(ievent);
            loaded.second.eval();
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

    SingleOut Merge (const std::vector<FileConfig::DirectoryInfo>& dirinfos,
                     const std::map<std::string, std::vector<SingleOut>>& outputs) {
      SingleOut output {};
      for (auto& dirinfo : dirinfos) {
        auto& outs = outputs.at(dirinfo.name);
        for (auto& single : outs)
          output.insert(output.end(), single.begin(), single.end());
      }
      return output;
    }
  }
}

#endif
