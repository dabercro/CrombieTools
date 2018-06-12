#ifndef CROMBIE_PLOTTER_H
#define CROMBIE_PLOTTER_H

#include <string>
#include <vector>
#include <map>

#include "Selection.h"
#include "PlotConfig.h"
#include "FileConfig.h"

#include "TFile.h"

namespace Crombie {
  namespace Plotter {

    /**
       This is the output running over a single file.
       The first number is the number of events for cross section normalization.
       The key corresponds to a selection, and the different TH1Ds are different process cuts.
    */
    using SingleOut = std::pair<double, std::map<std::string, std::vector<TH1D>>>;

    /// Constructs a function that runs over a single file and produces all the necessary histograms
    std::function<SingleOut(const FileConfig::FileInfo&)>
      SingleFile (const std::vector<PlotConfig::Plot>& plots,
                  const Selection::SelectionConfig& selections);

    /// This class has everything needed to draw a plot
    class Plot {
      void  draw  (std::string filebase);
      void  dumpdatacard (std::string filename);
    private:
      TH1D datahist;
      std::map<std::string, TH1D> mchists;
      std::map<std::string, TH1D> signals;
    };

    /**
       Merges the output of the SingleFile functional
       The key is the output file name for the histogram
    */
    std::map<std::string, Plot> Merge (const std::vector<FileConfig::DirectoryInfo>& dirinfos,
                                       const std::map<std::string, std::vector<SingleOut>>& outputs);


    // IMPLEMENTATIONS BELOW HERE //


    std::function<SingleOut(const FileConfig::FileInfo&)>
      SingleFile (const std::vector<PlotConfig::Plot>& plots,
                  const Selection::SelectionConfig& selections) {
      return std::function<SingleOut(const FileConfig::FileInfo&)> {
        [&plots, &selections] (const FileConfig::FileInfo& info) {
          TFile input {info.name.data()};

          SingleOut output {
            static_cast<TH1*>(input.Get(selections.mchistname.data()))->GetBinContent(0),
            {}
          };

          // Build the formulas and plots to use

          return output;
        }
      };
    }

    std::map<std::string, Plot> Merge (const std::vector<FileConfig::DirectoryInfo>& dirinfos,
                                       const std::map<std::string, std::vector<SingleOut>>& outputs) {
      std::map<std::string, Plot> output {};
      return output;
    }

  }
}

#endif
