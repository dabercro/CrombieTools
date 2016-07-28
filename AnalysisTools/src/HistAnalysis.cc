#include <iostream>

#include "TH1D.h"
#include "TH1F.h"
#include "TMath.h"

#include "PlotBase.h"
#include "HistAnalysis.h"

ClassImp(HistAnalysis)

//--------------------------------------------------------------------
Double_t
HistAnalysis::DoScaleFactors(TString PlotVar, Int_t NumBins, Double_t *XBins,
                             ScaleFactorMethod  method, Bool_t NormalizeBackground,
                             TString TreeName)
{
  ResetWeight();

  SetDefaultExpr(PlotVar);

  // First create the data histograms
  SetDefaultTree(ReturnTChain(TreeName,kData));
  AddWeight(fBaseCut);
  for (UInt_t iCut = 0; iCut != fScaleFactorCuts.size(); ++iCut)
    AddWeight(fBaseCut + " && " + fDataSFCuts[iCut]);
  std::vector<TH1D*> dataHists = MakeHists(NumBins,XBins);

  // Then create the signal histograms
  SetDefaultTree(ReturnTChain(TreeName,fSignalType,fSignalName,kLegendEntry));
  ResetWeight();
  AddWeight(TString("(") + fBaseCut + ")*(" + fMCWeight + ")");
  for (UInt_t iCut = 0; iCut != fScaleFactorCuts.size(); ++iCut)
    AddWeight(TString("(") + fBaseCut + " && " + fScaleFactorCuts[iCut] + ")*(" + fMCWeight + ")");
  std::vector<TH1D*> signalHists = MakeHists(NumBins,XBins);

  // Then create the background histograms
  SetDefaultTree(ReturnTChain(TreeName,kBackground,fSignalName,kLegendEntry,false));
  std::vector<TH1D*> backgroundHists = MakeHists(NumBins,XBins);

  // Subtract out the background
  Double_t scale = -1.0 - fBackgroundChange;
  if (NormalizeBackground)
    scale *= dataHists[0]->Integral()/(backgroundHists[0]->Integral() + signalHists[0]->Integral());

  for (UInt_t iHist = 0; iHist != dataHists.size(); ++iHist)
    dataHists[iHist]->Add(backgroundHists[iHist],scale);

  std::vector<Double_t> data_yields;
  std::vector<Double_t> data_error;
  std::vector<Double_t> mc_yields;
  std::vector<Double_t> mc_error;

  Double_t factor = 1.0;

  switch (method) {

  case kCutAndCount:

    // Get the yields from each histogram
    for (UInt_t iHist = 0; iHist != dataHists.size(); ++iHist) {
      Double_t error = 0.0;
      Double_t yield = dataHists[iHist]->IntegralAndError(1,NumBins,error);
      data_yields.push_back(yield);
      data_error.push_back(error);
      error = 0.0;
      yield = signalHists[iHist]->IntegralAndError(1,NumBins,error);
      mc_yields.push_back(yield);
      mc_error.push_back(error);
    }

    factor = mc_yields[0]/data_yields[0];

    break;

  default:
    std::cerr << "I do not support that option right now." << std::endl;
    break;
  }

  if (fPrintingMethod != kNone) {
    std::cout << "\\hline" << std::endl;
    std::cout << " & No Cut";
    for (UInt_t iCut = 0; iCut != fCutNames.size(); ++iCut)
      std::cout << " & " << fCutNames[iCut];
    std::cout << " \\\\" << std::endl;
    std::cout << "\\hline" << std::endl;
    
    if (fPrintingMethod == kPresentation)
      std::cout << "\\makecell{Background \\\\ Subtracted \\\\ Data}";
    else
      std::cout << "Background Subtracted Data";
    for (UInt_t iYield = 0; iYield != data_yields.size(); ++iYield) {
      std::cout << " & " << TString::Format(fFormat,data_yields[iYield]);
      std::cout << " $\\pm$ " << TString::Format(fFormat,data_error[iYield]);
    }
    std::cout << " \\\\" << std::endl;
    
    if (fPrintingMethod == kPresentation)
      std::cout << "\\makecell{Signal-\\\\ matched MC}";
    else
      std::cout << "Signal-matched MC";
    for (UInt_t iYield = 0; iYield != mc_yields.size(); ++iYield) {
      std::cout << " & " << TString::Format(fFormat,mc_yields[iYield]);
      std::cout << " $\\pm$ " << TString::Format(fFormat,mc_error[iYield]);
    }
    std::cout << " \\\\" << std::endl;
    std::cout << "\\hline" << std::endl;
    
    if (fPrintingMethod == kPresentation)
      std::cout << "\\makecell{Normalized \\\\ Ratio}";
    else
      std::cout << "Normalized Ratio";
    for (UInt_t iYield = 0; iYield != data_yields.size(); ++iYield) {
      std::cout << " & " << TString::Format(fFormat,data_yields[iYield]/mc_yields[iYield] * factor);
      std::cout << " $\\pm$ " << TString::Format(fFormat,
                                                 TMath::Sqrt(pow(data_error[iYield]/mc_yields[iYield],2) +
                                                             pow(data_yields[iYield]/pow(mc_yields[iYield],2) * mc_error[iYield],2)) *
                                                 factor);
    }
    std::cout << " \\\\" << std::endl;
    std::cout << "\\hline" << std::endl;
  }

  return data_yields[data_yields.size()]/mc_yields[data_yields.size()] * factor;

}

//--------------------------------------------------------------------
Double_t
HistAnalysis::DoScaleFactors(TString PlotVar, Int_t NumBins, Double_t MinX, Double_t MaxX, 
                             ScaleFactorMethod  method, Bool_t NormalizeBackground,
                             TString TreeName)
{
  Double_t XBins[NumBins+1];
  ConvertToArray(NumBins,MinX,MaxX,XBins);
  return DoScaleFactors(PlotVar,NumBins,XBins,method,NormalizeBackground,TreeName);
}


//--------------------------------------------------------------------
void
HistAnalysis::AddScaleFactorCut(TString name, TString cut, TString datacut)
{
  fCutNames.push_back(name);
  fScaleFactorCuts.push_back(cut);
  if (datacut == "")
    fDataSFCuts.push_back(cut);
  else
    fDataSFCuts.push_back(datacut);
}

//--------------------------------------------------------------------
void
HistAnalysis::ResetScaleFactorCuts()
{
  fScaleFactorCuts.resize(0);
  fDataSFCuts.resize(0);
  fCutNames.resize(0);
}


//--------------------------------------------------------------------
void
HistAnalysis::MakeReweightHist(TString OutFile, TString OutHist, TString PlotVar, Int_t NumBins, Double_t *XBins, TString TreeName)
{
  // First create the data histogram
  TH1F *dataHist = new TH1F("dataHist","dataHist",NumBins,XBins);
  dataHist->Sumw2();
  ReturnTChain(TreeName,kData)->Draw(PlotVar + ">>dataHist",fBaseCut);

  // Then create the mc histogram
  TH1F *mcHist = new TH1F("mcHist","mcHist",NumBins,XBins);
  mcHist->Sumw2();
  ReturnTChain(TreeName,kBackground)->Draw(PlotVar + ">>mcHist",TString("(") + fBaseCut + ")*(" + fMCWeight + ")");

  dataHist->Scale(1.0/dataHist->Integral());
  mcHist->Scale(1.0/mcHist->Integral());

  dataHist->Divide(mcHist);

  TFile *theFile = new TFile(OutFile,"RECREATE");
  theFile->WriteTObject(dataHist,OutHist);
}

//--------------------------------------------------------------------
void
HistAnalysis::MakeReweightHist(TString OutFile, TString OutHist, TString PlotVar, Int_t NumBins, Double_t MinX, Double_t MaxX, TString TreeName)
{
  Double_t XBins[NumBins+1];
  ConvertToArray(NumBins,MinX,MaxX,XBins);
  MakeReweightHist(OutFile,OutHist,PlotVar,NumBins,XBins,TreeName);
}
