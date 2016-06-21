/**
  @file   PlotHist.h
  Definition of PlotHist class.
  @author Daniel Abercrombie <dabercro@mit.edu> */


#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H

#include "TH1D.h"
#include "TCanvas.h"

#include "PlotBase.h"

/**
   @ingroup plotgroup
   @class PlotHists
   A flexible class that plots histograms.
   Many practical uses are mediated by other classes. */

class PlotHists : public PlotBase
{
 public:
  PlotHists();
  virtual ~PlotHists();
  
  /// We can set normalization to match a particular other hist (or just 1)
  void                   SetNormalizedHists       ( Bool_t b )                           { fNormalizedHists = b;  }
  /// Set index of which histogram to normalize to
  void                   SetNormalizeTo           ( Int_t to )                           { fNormalizeTo = to;     }
  /// We can set events per some value of x so that our legend entry is not wrong and variable binning doesn't look stupid
  void                   SetEventsPer             ( Double_t per )                       { fEventsPer = per;      }

  /// This just return vectors of histograms for other uses
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t *XBins );
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t MinX, Double_t MaxX );
  
  /// Simplest of Canvas makers with just histograms as arugments, allowing for user manipulation of histograms
  void                   MakeCanvas               ( TString FileBase, std::vector<TH1D*> theHists,
                                                    TString XLabel, TString YLabel, Bool_t logY = false)
                                                              { BaseCanvas(FileBase,theHists,XLabel,YLabel,logY); }
  
  /// Alternatively, we can set values in PlotBase and then just give the binning
  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t *XBins,  
                                                    TString XLabel, TString YLabel, Bool_t logY = false);
  
  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

  /// There are some tests I had dump out, set by this bool
  void                   SetPrintTests            ( Bool_t b )                            { fPrintTests = b;      }

 private:
  
  Bool_t    fNormalizedHists;                     ///< Can normalize histograms in order to compare shapes
  Int_t     fNormalizeTo;                         ///< If not specified, normalized to 1
  Double_t  fEventsPer;                           ///< Histogram normalized to events per units of X axis
  Bool_t    fPrintTests;                          ///< Can dump some compatibility tests of histograms
  
  ClassDef(PlotHists,1)
};

#endif
