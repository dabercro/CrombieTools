#include <iostream>
#include "TProfile.h"

#include "PlotUtils.h"
#include "PlotHists.h"

ClassImp(PlotHists)

//--------------------------------------------------------------------
PlotHists::PlotHists()
{}

//--------------------------------------------------------------------
PlotHists::~PlotHists()
{
  for (UInt_t iDelete = 0; iDelete != fDeleteUnc.size(); ++iDelete)
    delete fDeleteUnc[iDelete];
}

//--------------------------------------------------------------------
void
PlotHists::AddUncertainty(UInt_t index, TString FileName, TString HistName, 
                          Int_t startBin, Int_t endBin )
{
  fSysUncIndices.push_back(index);
  UncertaintyInfo* Uncert = new UncertaintyInfo("", FileName, HistName, startBin, endBin);
  fUncerts.push_back(Uncert);
  fDeleteUnc.push_back(Uncert);
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotHists::MakeHists(Int_t NumXBins, Double_t *XBins)
{
  UInt_t NumPlots = 0;

  if (fNormalizeTo != -1)
    fNormalizedHists = true;

  if (fInTrees.size() != 0)
    NumPlots = fInTrees.size();
  else if (fInCuts.size() != 0)
    NumPlots = fInCuts.size();
  else
    NumPlots = fInExpr.size();

  if(NumPlots == 0){
    std::cout << "Nothing has been initialized in hists plot." << std::endl;
    exit(1);
  }

  TTree *inTree = fDefaultTree;
  TString inCut = fDefaultCut;
  TString inExpr = fDefaultExpr;

  TH1D *tempHist;

  std::vector<TH1D*> theHists;

  for (UInt_t iPlot = 0; iPlot != NumPlots; ++iPlot) {

    if (fInTrees.size() != 0)
      inTree = fInTrees[iPlot];
    if (fInCuts.size()  != 0)
      inCut  = fInCuts[iPlot];
    if (fInExpr.size() != 0)
      inExpr = fInExpr[iPlot];

    TString tempName;
    tempName.Form("Hist_%d", fPlotCounter);
    fPlotCounter++;
    tempHist = new TH1D(tempName, tempName, NumXBins, XBins);
    tempHist->Sumw2();

    inTree->Draw(inExpr+">>"+tempName, inCut);

    if (fUncExpr != "") {

      // If there's an uncertainty expression, add systematics to the plot
      tempName += "_unc";
      TProfile *uncProfile = new TProfile(tempName, tempName, NumXBins, XBins);
      inTree->Draw(fUncExpr + ">>" + tempName, inCut);
      for (Int_t iBin = 1; iBin != NumXBins + 1; ++iBin) {
        Double_t content = tempHist->GetBinContent(iBin);
        tempHist->SetBinError(iBin,
                              TMath::Sqrt(pow(tempHist->GetBinError(iBin), 2) + 
                                          content * content *
                                          uncProfile->GetBinContent(iBin)));
      }
      delete uncProfile;
    }

    theHists.push_back(tempHist);
  }

  if (fEventsPer > 0) {
    TString tempName;
    tempName.Form("Hist_%d", fPlotCounter);
    fPlotCounter++;
    tempHist = new TH1D(tempName, tempName, NumXBins, XBins);
    for (Int_t iBin = 1; iBin != NumXBins + 1; ++iBin)
      tempHist->SetBinContent(iBin, tempHist->GetBinWidth(iBin)/fEventsPer);

    SetZeroError(tempHist);
    for (UInt_t iHist = 0; iHist != theHists.size(); ++iHist)
      Division(theHists[iHist], tempHist);

    delete tempHist;
  }

  if (fNormalizedHists) {
    Double_t normInt = 1;
    if (fNormalizeTo != -1)
      normInt = theHists[fNormalizeTo]->Integral("width");

    for (UInt_t iHist = 0; iHist != NumPlots; ++iHist)
      theHists[iHist]->Scale(normInt/theHists[iHist]->Integral("width"));
  }

  for (UInt_t iUncert = 0; iUncert != fSysUncIndices.size(); ++iUncert)
    ApplyUncertainty(theHists[fSysUncIndices[iUncert]], fUncerts[iUncert]);

  return theHists;
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotHists::MakeHists(Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  return MakeHists(NumXBins, XBins);
}

//--------------------------------------------------------------------
void
PlotHists::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  std::vector<TH1D*> hists = MakeHists(NumXBins, XBins);
  BaseCanvas(FileBase, hists, XLabel, YLabel, logY);

  for (UInt_t i0 = 0; i0 != hists.size(); ++i0)
    delete hists[i0];
}

//--------------------------------------------------------------------
void
PlotHists::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  MakeCanvas(FileBase, NumXBins, XBins, XLabel, YLabel, logY);
}
