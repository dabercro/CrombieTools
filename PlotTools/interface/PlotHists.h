/**
  @file   PlotHists.h
  Definition of PlotHists class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/


#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H

#include <vector>

#include "TH1D.h"
#include "TCanvas.h"

#include "UncertaintyInfo.h"
#include "PlotBase.h"

/**
   @ingroup plotgroup
   @class PlotHists
   A flexible class that plots histograms.
   Many practical uses are mediated by other classes.
*/

class PlotHists : public PlotBase
{
 public:
  PlotHists();
  virtual ~PlotHists();

  /// Add uncertainty factors to some index of histograms about to be made
  void                   AddUncertainty           ( UInt_t index, TString FileName, TString HistName,
                                                    Int_t startBin = 1, Int_t endBin = 0 );
  /// This just return vectors of histograms for other uses
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t *XBins );
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t MinX, Double_t MaxX );

  /// Alternatively, we can set values in PlotBase and then just give the binning
  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

  /// Add uncertainty factors through branch expressions instead of a histogram
  void                   SetUncertaintySquared    ( TString expr )                       { fUncExpr = expr;       }
  /// We can set normalization to match a particular other hist (or just 1)
  void                   SetNormalizedHists       ( Bool_t b )                           { fNormalizedHists = b;  }
  /// Set index of which histogram to normalize to
  void                   SetNormalizeTo           ( Int_t to )                           { fNormalizeTo = to;     }
  /// We can set events per some value of x so that our legend entry is not wrong and variable binning doesn't look stupid
  void                   SetEventsPer             ( Double_t per )                       { fEventsPer = per;      }

  /// Simplest of Canvas makers with just histograms as arugments, allowing for user manipulation of histograms
  void                   MakeCanvas               ( TString FileBase, std::vector<TH1D*> theHists,
                                                    TString XLabel, TString YLabel, Bool_t logY = false)
                                                              { BaseCanvas(FileBase,theHists,XLabel,YLabel,logY); }

 private:

  Bool_t    fNormalizedHists = false;             ///< Can normalize histograms in order to compare shapes
  Int_t     fNormalizeTo = -1;                    ///< If not specified, normalized to 1
  Double_t  fEventsPer = 0;                       ///< Histogram normalized to events per units of X axis
  std::vector<UInt_t>           fSysUncIndices;   ///< Indices of histograms to apply systematic uncertainties
  std::vector<UncertaintyInfo*> fUncerts;         ///< Uncertainties to apply to histograms
  std::vector<UncertaintyInfo*> fDeleteUnc;       ///< Uncertainties created by the class to delete at the end
  TString   fUncExpr = "";                        ///< Branch expressions to add to the systematic uncertainty

  ClassDef(PlotHists, 1)
};

#endif
