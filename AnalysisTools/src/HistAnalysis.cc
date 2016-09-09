#include <iostream>

#include "TH1D.h"
#include "TH1F.h"
#include "TMath.h"

#include "PlotBase.h"
#include "HistAnalysis.h"

ClassImp(HistAnalysis)

//--------------------------------------------------------------------
TH1D*
HistAnalysis::DoScaleFactors(TString PlotVar, Int_t NumBins, Double_t *XBins,
                             ScaleFactorMethod  method)
{
  TH1D* output = new TH1D("ScaleFactors", "ScaleFactors", NumBins, XBins);
  output->Sumw2();

  ResetWeight();

  SetDefaultExpr(PlotVar);

  // First create the data histograms
  std::vector<TH1D*> dataHists;
  SetDefaultWeight(fBaseCut);
  dataHists.push_back(GetHist(NumBins, XBins, kData));
  for (UInt_t iCut = 0; iCut != fScaleFactorCuts.size(); ++iCut) {
    SetDefaultWeight(fBaseCut + " && " + fDataSFCuts[iCut]);
    dataHists.push_back(GetHist(NumBins, XBins, kData));
  }

  // Then create the signal and background histograms
  std::vector<TH1D*> signalHists;
  std::vector<TH1D*> backgroundHists;
  SetDefaultWeight(fBaseCut);
  signalHists.push_back(GetHist(NumBins, XBins, fSignalType, fSignalName, fSearchBy));
  backgroundHists.push_back(GetHist(NumBins, XBins, fSignalType, fSignalName, fSearchBy, false));
  for (UInt_t iCut = 0; iCut != fScaleFactorCuts.size(); ++iCut) {
    SetDefaultWeight(TString("(") + fBaseCut + " && " + fScaleFactorCuts[iCut] + ")");
    signalHists.push_back(GetHist(NumBins, XBins, fSignalType, fSignalName, fSearchBy));
    backgroundHists.push_back(GetHist(NumBins, XBins, fSignalType, fSignalName, fSearchBy, false));
  }

  for (Int_t iBin = 1; iBin != NumBins + 1; ++iBin) {

    // Subtract out the background
    Double_t scale = -1.0 - fBackgroundChange;
    if (fNormalized)
      scale *= dataHists[0]->GetBinContent(iBin)/
        (backgroundHists[0]->GetBinContent(iBin) +
         signalHists[0]->GetBinContent(iBin));

    for (UInt_t iHist = 0; iHist != dataHists.size(); ++iHist)
      dataHists[iHist]->Add(backgroundHists[iHist], scale);

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

    output->SetBinContent(iBin, data_yields[data_yields.size()]/mc_yields[data_yields.size()] * factor);
    output->SetBinError(iBin, TMath::Sqrt(pow(data_error[data_yields.size()]/mc_yields[data_yields.size()],2) +
                                          pow(data_yields[data_yields.size()]/pow(mc_yields[data_yields.size()],2) *
                                              mc_error[data_yields.size()],2)) * factor);
  }

  return output;

}

//--------------------------------------------------------------------
TH1D*
HistAnalysis::DoScaleFactors(TString PlotVar, Int_t NumBins, Double_t MinX, Double_t MaxX, 
                             ScaleFactorMethod  method)
{
  Double_t XBins[NumBins+1];
  ConvertToArray(NumBins, MinX, MaxX, XBins);
  return DoScaleFactors(PlotVar, NumBins, XBins, method);
}


//--------------------------------------------------------------------

/**
   @param name is the name that the cut will be given in the table that is printed out
   @param cut is the cut string to apply for the scale factor cut
   @param datacut is an optional argument in case you want to have a different cut on data
*/

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
HistAnalysis::MakeReweightHist(TString OutFile, TString OutHist, TString PlotVar, Int_t NumBins, Double_t *XBins)
{

  SetDefaultExpr(PlotVar);
  SetDefaultWeight(fBaseCut);

  // First create the data histogram
  TH1D *dataHist = GetHist(NumBins, XBins, kData);

  // Then create the mc histogram
  TH1D *mcHist = GetHist(NumBins, XBins, fSignalType, fSignalName, fSearchBy, true);

  // Do background subtraction, if necessary
  if (fSignalName != "") {

    TH1D *backgroundHist = GetHist(NumBins, XBins, fSignalType, fSignalName, fSearchBy, false);
    dataHist->Add(backgroundHist, -1.0);
    delete backgroundHist;

  }

  // Normalize for most applications, unless we are getting transfer factors or something
  if (fNormalized) {

    dataHist->Scale(1.0/dataHist->Integral());
    mcHist->Scale(1.0/mcHist->Integral());

  }

  dataHist->Divide(mcHist);

  TH1F *uncHist = new TH1F("unc", "unc", NumBins, XBins);
  for (Int_t iBin = 1; iBin != NumBins + 1; ++iBin)
    uncHist->SetBinContent(iBin, dataHist->GetBinError(iBin)/dataHist->GetBinContent(iBin));

  TFile *theFile = new TFile(OutFile, "RECREATE");
  theFile->WriteTObject(dataHist, OutHist);
  theFile->WriteTObject(uncHist, OutHist + "_unc");

  delete dataHist;
  delete mcHist;
  delete uncHist;

}

//--------------------------------------------------------------------
void
HistAnalysis::MakeReweightHist(TString OutFile, TString OutHist, TString PlotVar, Int_t NumBins, Double_t MinX, Double_t MaxX)
{

  Double_t XBins[NumBins+1];
  ConvertToArray(NumBins, MinX, MaxX, XBins);
  MakeReweightHist(OutFile, OutHist, PlotVar, NumBins, XBins);

}
