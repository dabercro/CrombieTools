#ifndef CROMBIE_HIST_H
#define CROMBIE_HIST_H

// I really don't like ROOT's global hash tables and confusing lifetimes
// Here, I make a simple data structure that can make a histogram

#include <cmath>
#include <exception>
#include <vector>
#include <list>

#include "crombie/Debug.h"

#include "TH1D.h"

namespace crombie {
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
      TH1D* roothist () const;

      /// Get the maximum bin and the total number of bins
      std::pair<unsigned, unsigned> get_maxbin_outof () const;

      /// Get the maximum value including uncertainties (for plotting)
      double max_w_unc () const;

    private:
      std::string label {};
      unsigned nbins {};
      double min {};
      double max {};

      std::vector<double> contents {};
      std::vector<double> sumw2 {};

      // We use a list to avoid reallocation
      mutable std::list<TH1D> histstore {};
    };

    void Hist::fill(double val, double weight) {
      unsigned bin {std::min(nbins + 2, static_cast<unsigned>(std::ceil(((val - min)/(max - min)) * nbins)))};
      contents[bin] += weight;
      sumw2[bin] += std::pow(weight, 2);
    }

    void Hist::add(const Hist& other) {
      if (nbins == 0)
        *this = other;
      else {
        if (other.nbins != nbins) {
          std::cerr << "Num bins other: " << other.nbins << " me: " << nbins << std::endl;
          throw std::runtime_error{"Hists don't have same number of bins"};
        }
        for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
          contents[ibin] += other.contents[ibin];
          sumw2[ibin] += other.sumw2[ibin];
        }
      }
    }

    void Hist::scale(const double scale) {
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        contents[ibin] *= scale;
        sumw2[ibin] *= scale;
      }
    }

    TH1D* Hist::roothist() const {
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

    std::pair<unsigned, unsigned> Hist::get_maxbin_outof () const {
      double max = 0;
      unsigned maxbin = 0;
      for (unsigned ibin = 1; ibin <= nbins; ++ibin) {
        if (contents[ibin] > max) {
          max = contents[ibin];
          maxbin = ibin;
        }
      }
      return std::make_pair(maxbin, std::max(nbins, 1u));
    }

    double Hist::max_w_unc () const {
      double output = 0;
      for (unsigned ibin = 1; ibin <= nbins; ++ibin)
        output = std::max(contents[ibin] + std::sqrt(sumw2[ibin]), output);
      return output;
    }


  }
}


#endif
