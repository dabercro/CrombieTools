/**
  @file   RatioComparison.h
  Definition of RatioComparison class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/


#ifndef CROMBIETOOLS_PLOTTOOLS_RATIOCOMPARISON_H
#define CROMBIETOOLS_PLOTTOOLS_RATIOCOMPARISON_H

#include <vector>

#include "TString.h"

#include "FileConfigReader.h"

/**
   @ingroup plotgroup
   @class RatioComparison
   This class is used to quickly make ratios between control regions and compare the
   data and Monte Carlo shapes
*/

class RatioComparison : public FileConfigReader
{
 public:
  RatioComparison();
  virtual ~RatioComparison();

  /// Make canvas using an array
  void         MakeCompare               ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                                           TString XLabel, TString YLabel );

  /// Make canvas using regular binning
  void         MakeCompare              ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                          TString XLabel, TString YLabel );

  /// Reset the stored backgrounds to make a different plot
  void         Reset                    ();

  /// A line is created for each ratio cut, using the correct data type, set by this function
  void         AddRatioCuts             ( TString num_cut, TString denom_cut, FileType type, TString entry, Int_t color );

 private:

  std::vector<TString> fNumeratorCuts;       ///< The numerator cut for each line
  std::vector<TString> fDenominatorCuts;     ///< The denominator cut for each line
  std::vector<FileType> fTypes;              ///< The type of data for each line

  ClassDef(RatioComparison, 1)
};

#endif
