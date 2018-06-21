#ifndef CROMBIE_HIST_H
#define CROMBIE_HIST_H

// I really don't like ROOT's global hash tables and confusing lifetimes
// Here, I make a simple data structure that can make a histogram

#include <cmath>
#include <exception>
#include <vector>
#include <list>
#include <limits>

#include "crombie/Debug.h"

#include "TH1D.h"

namespace crombie {
  namespace Hist {
    class Hist {
    public:
    Hist(const std::string label = "", unsigned nbins = 0, double min = 0, double max = 0)
      : label{label}, nbins{nbins}, min{min}, max{max}, contents(nbins + 2), sumw2(nbins + 2) {
      Debug::Debug(__func__, "Creating hist at", this);
    }

      void fill (double val, double weight = 1.0);
      void add  (const Hist& other);
      void scale(const double scale);

      /// Returns a Hist that is a ratio between this and another Hist
      Hist ratio(const Hist& other) const;

      /// Returns a pointer to a histogram that is owned by global list. Not thread-safe.
      TH1D* roothist () const;

      /// Get the maximum bin and the total number of bins
      std::pair<unsigned, unsigned> get_maxbin_outof () const;

      /// Get the maximum value including uncertainties (for plotting)
      double max_w_unc () const;
      /// Get the minimum value including uncertainties, but not less than 0.0 (for plotting)
      double min_w_unc () const;

    private:
      std::string label {};
      unsigned nbins {};
      double min {};
      double max {};

      std::vector<double> contents {};
      std::vector<double> sumw2 {};
    };

    namespace {
      // Put this here so we don't have to move around with the Hist objects
      std::list<TH1D> histstore;
    }

    void Hist::fill(double val, double weight) {
      unsigned bin {std::min(nbins + 2, static_cast<unsigned>(std::ceil(((val - min)/(max - min)) * nbins)))};
      contents[bin] += weight;
      sumw2[bin] += std::pow(weight, 2);
    }

    void Hist::add(const Hist& other) {
      Debug::Debug(__func__, "Start add", this, nbins);
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
      Debug::Debug(__func__, "End add", this, nbins);
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
      Debug::Debug(__func__, "Generating root hist with", title, nbins, min, max, "from", this);
      histstore.emplace_back(std::to_string(plot++).data(), title.data(), static_cast<int>(nbins), min, max);
      auto& hist = histstore.back();
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        Debug::Debug(__func__, "Bin:", ibin, "Content:", contents[ibin]);
        hist.SetBinContent(ibin, contents[ibin]);
        hist.SetBinError(ibin, std::sqrt(sumw2[ibin]));
      }
      Debug::Debug(__func__, "hist with", hist.Integral());
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

    double Hist::min_w_unc () const {
      double output = std::numeric_limits<double>::max();
      for (unsigned ibin = 1; ibin <= nbins; ++ibin)
        output = std::min(contents[ibin] - std::sqrt(sumw2[ibin]), output);
      return std::max(output, 0.0);
    }

    Hist Hist::ratio(const Hist& other) const {
      Debug::Debug(__func__, "ratio", nbins, min, max);
      Hist output{*this};
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        auto bincontent = other.contents.at(ibin);
        if (bincontent) {
          output.contents[ibin] /= other.contents.at(ibin);
          output.sumw2[ibin] /= std::pow(other.contents.at(ibin), 2);
        }
        else {
          output.contents[ibin] = 1.0;
          output.sumw2[ibin] = 0;
        }
      }
      Debug::Debug(__func__, "output", output.nbins, output.min, output.max);
      return output;
    }

  }
}


#endif
