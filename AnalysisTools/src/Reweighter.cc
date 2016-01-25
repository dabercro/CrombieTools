#include "Reweighter.h"

ClassImp(Reweighter)

//--------------------------------------------------------------------
Reweighter::Reweighter(TString name) :
  Corrector(name),
  fMakingWeightsFromPlot(false)
{ }
  
//--------------------------------------------------------------------
Reweighter::~Reweighter()
{ }

//--------------------------------------------------------------------
void
Reweighter::SetCorrectionFile(TString fileName)
{
  Corrector::SetCorrectionFile(fileName);
  if (!fCorrectionFile) {
    fMakingWeightsFromPlot = true;
    fCorrectionFile = new TFile(fileName,"RECREATE");
  }
}

//--------------------------------------------------------------------
void
Reweighter::SetCorrectionHist(TString histName, Int_t NumXBins, Double_t* XBins)
{
  if (!fMakingWeightsFromPlot) {
    Corrector::SetCorrectionHist(histName);
    if (!fCorrectionHist) {
      fMakingWeightsFromPlot = true;
      fCorrectionFile->Close();
      fCorrectionFile = new TFile(fCorrectionFile->GetName(),"UPDATE");
    }
  }

  if(fMakingWeightsFromPlot) {
    // Reweights "MC" to match "Data"
    std::vector<TH1D*> DataHists = GetHistList(NumXBins,XBins,false);
    std::vector<TH1D*>   MCHists = GetHistList(NumXBins,XBins,true);

    fCorrectionHist = (TH1D*) DataHists[0]->Clone(histName);
    fCorrectionHist->Reset("M");
    TH1D* ratioHist = (TH1D*) fCorrectionHist->Clone(histName + "_divisor");
    for (UInt_t iHist = 0; iHist < DataHists.size(); iHist++)
      fCorrectionHist->Add(DataHists[iHist]);
    for (UInt_t iHist = 0; iHist < MCHists.size(); iHist++)
      ratioHist->Add(MCHists[iHist]);

    fCorrectionHist->Scale(1/fCorrectionHist->Integral("width"));
    ratioHist->Scale(1/ratioHist->Integral("width"));

    fCorrectionHist->Divide(ratioHist);
    fCorrectionFile->WriteTObject(fCorrectionHist,histName);

    delete ratioHist;
    for (UInt_t iHist = 0; iHist != MCHists.size(); ++iHist)
      delete MCHists[iHist];
    for (UInt_t iHist = 0; iHist != DataHists.size(); ++iHist)
      delete DataHists[iHist];
  }

  SetMinMax();
}

//--------------------------------------------------------------------
void
Reweighter::SetCorrectionHist(TString histName, Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins,MinX,MaxX,XBins);
  SetCorrectionHist(histName,NumXBins,XBins);
}
