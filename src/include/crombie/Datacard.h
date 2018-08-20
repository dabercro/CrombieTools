#ifndef CROMBIE_DATACARD_H
#define CROMBIE_DATACARD_H

#include <string>

#include "crombie/Types.h"
#include "crombie/Plotter.h"
#include "crombie/Uncertainty.h"
#include "crombie/FileSystem.h"


namespace crombie {
  namespace Datacard {

    /**
       @class DataCardInfo
       Much of the information that is needed to write the datacard,
       such as shape uncertainties, plot location, and bin contents for plots.
       The flat uncertainties and rate params are missing from this.
    */
    class DataCardInfo {
    public:
      DataCardInfo (const std::string& dir) : dir{dir} {}
      const std::string dir;                ///< The directory storing the ROOT file and datacard
      Types::map<Types::strings> shapes;    ///< A list of shape uncertainties and which samples are affected
    };


    /// Dumps the histograms and extracts information needed for datacard using the output from Plotter
    DataCardInfo dumpplots(const std::string& dirname,
                           const std::set<std::string>& selections,
                           const Uncertainty::UncertaintyInfo& unc,
                           const Plotter::MergeOut& plots) {

      DataCardInfo output {dirname};

      if (not FileSystem::exists(dirname))
        FileSystem::mkdirs(dirname);

      // Get the list of all the systematics
      auto systematics = unc.systematics();

      // Loop through the selections
      auto i_plot = plots.begin();
      for (auto& sel : selections) {
        while (i_plot->first.find(sel) != 0) {
          if (++i_plot == plots.end())
            i_plot = plots.begin();
        }
        // store the information for this plot
        
      }
    }

  }
}

#endif
