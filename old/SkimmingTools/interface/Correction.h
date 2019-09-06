#ifndef CROMBIE_CORRECTION_H
#define CROMBIE_CORRECTION_H 1

#include <map>
#include <string>

#include "TFile.h"
#include "TH2.h"

// ROOT won't let us open files, load histograms, and close files peacefully,
// but at least we can avoid opening files multiple times
namespace {

  class Files : public std::map<std::string, TFile*> {
  public:
    ~Files () {
      // Fuck ROOT
      /* for (auto& file : *this) */
      /*   file.second->Close(); */
    }
  };

  Files files {};

}

/**
   @ingroup skimminggroup
   @class Correction
   @brief A rewrite of the Corrector that allows easy standalone correction application

   @param H The type of the histogram to read
*/

template<typename H>
class Correction {
 public:
  /**
     Reads in one or two histograms from a file.

     Both histograms must have the same binning.
     If two histograms are given the corrector applies the first histogram divided by the second.
  */
  Correction(std::string filename, const char* histname, const char* denom = 0);
  ~Correction() { /* delete hist; */ }

  /// Get the correction value from the histogram
  double GetCorrection(const double xval, const double yval);
  double GetCorrection(const double xval);

 private:
  H* hist;
  double get(const int bin);
  using minmax = std::pair<double, double>;
  minmax xminmax {};
  minmax yminmax {};
  
  double Filter(const minmax& mm, double val);

};

template<typename H>
Correction<H>::Correction(std::string filename, const char* histname, const char* denom)
{
  if (files.find(filename) == files.end())
    files[filename] = TFile::Open(filename.data());

  auto& in = files[filename];

  hist = static_cast<H*>(in->Get(histname)->Clone());
  if (denom)
    hist->Divide(static_cast<H*>(in->Get(denom)));

  auto* axis = hist->GetXaxis();
  xminmax.first = axis->GetBinCenter(axis->GetFirst());
  xminmax.second = axis->GetBinCenter(axis->GetLast());

  if (dynamic_cast<const TH2*>(hist)) {
    axis = hist->GetYaxis();
    yminmax.first = axis->GetBinCenter(axis->GetFirst());
    yminmax.second = axis->GetBinCenter(axis->GetLast());
  }
}

template<typename H>
double Correction<H>::GetCorrection(const double xval, const double yval) {
  return get(hist->FindBin(Filter(xminmax, xval), Filter(yminmax, yval)));
};

template<typename H>
double Correction<H>::GetCorrection(const double xval) {
  return get(hist->FindBin(Filter(xminmax, xval)));
};

template<typename H>
double Correction<H>::get(const int bin) {
  return hist->GetBinContent(bin);
};

template<typename H>
double Correction<H>::Filter(const Correction::minmax& mm, double val) {
  return std::max(mm.first, std::min(mm.second, val));
}

#endif
