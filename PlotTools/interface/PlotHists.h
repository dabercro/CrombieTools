#ifndef MITPLOTS_PLOT_PLOTHISTS_H
#define MITPLOTS_PLOT_PLOTHISTS_H

#include "TH1D.h"
#include "TCanvas.h"

#include "PlotBase.h"

class PlotHists : protected PlotBase
{
 public:
  PlotHists();
  virtual ~PlotHists();
  
  void                   SetNormalizedHists       ( Bool_t b )        { fNormalizedHists = b;  }
  
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t *XBins );                   // These just return vectors of
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t MinX, Double_t MaxX );      //   histograms for other uses
  
  void                   MakeCanvas               ( Int_t NumXBins, Double_t *XBins, TString FileBase,   
                                                    TString XLabel, TString YLabel, Bool_t logY = false);
  
  void                   MakeCanvas               ( Int_t NumXBins, Double_t MinX, Double_t MaxX, TString FileBase,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

/*   void                   MakeRatio                ( Int_t NumXBins, Double_t MinX, Double_t MaxX, TString FileBase, */
/*                                                     TString CanvasTitle, TString XLabel, TString YLabel, */
/*                                                     Int_t DataNum ); */
  
 private:
  
  Bool_t    fNormalizedHists;                     // Can normalize histograms in order to compare shapes
  
  ClassDef(PlotHists,1)
};

#endif
