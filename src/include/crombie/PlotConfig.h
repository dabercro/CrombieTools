#ifndef CROMBIE_PLOTCONFIG_H
#define CROMBIE_PLOTCONFIG_H

#include <vector>
#include <string>
#include <fstream>
#include <regex>

#include "crombie/Hist.h"

namespace crombie {
  namespace PlotConfig {

    /// All of the information needed to make a plot
    class Plot {
    public:
    Plot(const std::string name, const unsigned nbins, const double low, const double max,
         const std::string label, const std::string data_var = "", const std::string mc_var = "")
      : name{name}, nbins{nbins}, low{low}, max{max}, label{label},
        data_var{data_var.size() ? data_var : name}, mc_var{mc_var.size() ? mc_var : name} {
          Debug::Debug(__PRETTY_FUNCTION__, "New plot", name, nbins, low, max, label, data_var, mc_var);
        };

      const std::string name;
    private:
      const unsigned nbins;
      const double low;
      const double max;
      const std::string label;
    public:
      const std::string data_var;
      const std::string mc_var;

      Hist::Hist get_hist() const;   ///< Get a histogram that's properly formatted for this plot
    };


    std::vector<Plot> read(const char* config);


    // IMPLEMENTATIONS BELOW HERE //


    std::vector<Plot> read(const char* config) {
      std::vector<Plot> output;

      std::regex expr {"'([^']+)',\\s*(\\d+),\\s*(-?[\\d\\.]+),\\s*(-?[\\d\\.]+),\\s*'([^']+)'(,\\s*'([^']+)',\\s*'([^']+)')?"};
      std::smatch matches;

      std::ifstream input {config};
      for (std::string raw; std::getline(input, raw); ) {
        // Strip out comments
        std::string line {raw.substr(0, raw.find("! "))};
        if (line.size()) {
          Debug::Debug(__PRETTY_FUNCTION__, "Plot line", line);
          if (std::regex_match(line, matches, expr)) {
            output.push_back({matches[1], static_cast<unsigned>(std::stoi(matches[2])),
                              std::stod(matches[3]), std::stod(matches[4]),
                              matches[5], matches[7], matches[8]});
          }
        }
      }
      return output;
    }

    Hist::Hist Plot::get_hist() const {
      return Hist::Hist(label, nbins, low, max);
    }

  }
}

#endif
