#ifndef MITPLOTS_PLOT_PLOTHISTS_H
#define MITPLOTS_PLOT_PLOTHISTS_H

#include "TH1D.h"
#include "TCanvas.h"

#include "PlotBase.h"

class PlotHists : public PlotBase
{
 public:
  PlotHists();
  virtual ~PlotHists();
  
  void                   SetNormalizedHists       ( Bool_t b )        { fNormalizedHists = b;  }
  void                   SetNormalizeTo           ( Int_t to )        { fNormalizeTo = to;     }
  
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t *XBins );                   // These just return vectors of
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t MinX, Double_t MaxX );      //   histograms for other uses
  
  void                   MakeCanvas               ( Int_t NumXBins, Double_t *XBins, TString FileBase,   
                                                    TString XLabel, TString YLabel, Bool_t logY = false);
  
  void                   MakeCanvas               ( Int_t NumXBins, Double_t MinX, Double_t MaxX, TString FileBase,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

 private:
  
  Bool_t    fNormalizedHists;                     // Can normalize histograms in order to compare shapes
  Int_t     fNormalizeTo;                         // If not specified, normalized to 1
  
  ClassDef(PlotHists,1)
};

#endif
