#ifndef CROMBIE_HIST_H
#define CROMBIE_HIST_H

// I really don't like ROOT's global hash tables and confusing lifetimes
// Here, I make a simple data structure that can make a histogram

#include <cmath>
#include <exception>
#include <vector>
#include <list>

#include "Debug.h"

#include "TH1D.h"

namespace Crombie {
  namespace Hist {
    class Hist {
    public:
    Hist() {}
    Hist(const std::string label, unsigned nbins, double min, double max)
      : label{label}, nbins{nbins}, min{min}, max{max}, contents(nbins + 2), sumw2(nbins + 2) {}

      void fill (double val, double weight = 1.0);
      void add  (const Hist& other);
      void scale(const double scale);

      /// Returns a pointer to a histogram that is owned by this object
      TH1D* roothist ();

    private:
      std::string label {};
      unsigned nbins {};
      double min {};
      double max {};

      std::vector<double> contents {};
      std::vector<double> sumw2 {};

      // We use a list to avoid reallocation
      std::list<TH1D> histstore {};
    };

    void Hist::fill(double val, double weight) {
      unsigned bin {std::min(nbins + 2, static_cast<unsigned>(std::ceil(((val - min)/(max - min)) * nbins)))};
      contents[bin] += weight;
      sumw2[bin] += std::pow(weight, 2);
    }

    void Hist::add(const Hist& other) {
      if (other.nbins != nbins)
        throw std::runtime_error{"Hists don't have same number of bins"};
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        contents[ibin] += other.contents[ibin];
        sumw2[ibin] += other.sumw2[ibin];
      }        
    }

    void Hist::scale(const double scale) {
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        contents[ibin] *= scale;
        sumw2[ibin] *= scale;
      }
    }

    TH1D* Hist::roothist() {
      static unsigned plot = 0;
      auto title = std::string(";") + label + ";Events";
      histstore.push_back({std::to_string(plot++).data(), title.data(), static_cast<int>(nbins), min, max});
      auto& hist = histstore.back();
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        hist.SetBinContent(ibin, contents[ibin]);
        hist.SetBinError(ibin, std::sqrt(sumw2[ibin]));
      }
      Debug::Debug("hist with", hist.Integral());
      return &hist;
    }

  }
}


#endif
