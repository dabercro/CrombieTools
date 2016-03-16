#ifndef CROMBIETOOLS_SKIMMINGTOOLS_REWEIGHTER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_REWEIGHTER_H

#include "Corrector.h"
#include "PlotStack.h"

class Reweighter : public Corrector, public PlotStack
{
 public:
  Reweighter( TString name = "reweight" );
  virtual ~Reweighter();

  void             SetCorrectionFile ( TString fileName );
  void             SetCorrectionHist ( TString histName, Int_t NumXBins, Double_t* XBins );
  void             SetCorrectionHist ( TString histName, Int_t NumXBins, Double_t MinX, Double_t MaxX );

 private:
  Bool_t           fMakingWeightsFromPlot;
  
  ClassDef(Reweighter,1)
};

#endif
