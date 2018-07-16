#ifndef CROMBIE_DATACARD_H
#define CROMBIE_DATACARD_H

#include <string>

#include "crombie/Plotter.h"


namespace crombie {
  namespace Datacard {

    /// Dumps the histogram and datacard using the output from Plotter
    void dumpplots(const std::string& dirname, const std::set<std::string>& selections, const Plotter::MergeOut& plots) {
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
