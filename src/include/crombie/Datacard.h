#ifndef CROMBIE_DATACARD_H
#define CROMBIE_DATACARD_H

#include <string>

#include "crombie/Plotter.h"
#include "crombie/Uncertainty.h"


namespace crombie {
  namespace Datacard {

    /**
       @class DataCardInfo
       Much of the information that is needed to write the datacard,
       such as shape uncertainties, plot location, and bin contents for plots.
       The flat uncertainties and rate params are missing from this.
    */
    class DataCardInfo {
    }


    /// Dumps the histograms and extracts information needed for datacard using the output from Plotter
    void dumpplots(const std::string& dirname,
                   const std::set<std::string>& selections,
                   const Uncertainty::UncertaintyInfo& unc,
                   const Plotter::MergeOut& plots) {

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
