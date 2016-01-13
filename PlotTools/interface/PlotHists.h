#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H

#include "TH1D.h"
#include "TCanvas.h"

#include "PlotBase.h"

class PlotHists : public PlotBase
{
 public:
  PlotHists();
  virtual ~PlotHists();
  
  void                   SetNormalizedHists       ( Bool_t b )                           { fNormalizedHists = b;  }
  void                   SetNormalizeTo           ( Int_t to )                           { fNormalizeTo = to;     }
  void                   SetEventsPer             ( Double_t per )                       { fEventsPer = per;      }

  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t *XBins );                   // These just return vectors of
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t MinX, Double_t MaxX );      //   histograms for other uses
  
  void                   MakeCanvas               ( TString FileBase, std::vector<TH1D*> theHists,
                                                    TString XLabel, TString YLabel, Bool_t logY = false)
                                                              { BaseCanvas(FileBase,theHists,XLabel,YLabel,logY); }
  
  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t *XBins,  
                                                    TString XLabel, TString YLabel, Bool_t logY = false);
  
  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

  void                   SetPrintTests            ( Bool_t b )                            { fPrintTests = b;      }
 private:
  
  Bool_t    fNormalizedHists;                     // Can normalize histograms in order to compare shapes
  Int_t     fNormalizeTo;                         // If not specified, normalized to 1
  Double_t  fEventsPer;
  Bool_t    fPrintTests;
  
  ClassDef(PlotHists,1)
};

#endif
