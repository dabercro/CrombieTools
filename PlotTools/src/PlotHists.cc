#include <iostream>
#include "TLegend.h"

#include "PlotHists.h"

ClassImp(PlotHists)

//--------------------------------------------------------------------
PlotHists::PlotHists() :
  fNormalizedHists(false),
  fNormalizeTo(-1)
{}

//--------------------------------------------------------------------
PlotHists::~PlotHists()
{}

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
    tempName.Form("Hist_%d",fPlotCounter);
    fPlotCounter++;
    tempHist = new TH1D(tempName,tempName,NumXBins,XBins);
    if (fIncludeErrorBars || int(iPlot) == fDataIndex)
      tempHist->Sumw2();

    inTree->Draw(inExpr+">>"+tempName,inCut);

    theHists.push_back(tempHist);
  }

  if (fNormalizedHists) {
    Double_t normInt = 1;
    if (fNormalizeTo != -1)
      normInt = theHists[fNormalizeTo]->Integral("width");

    for (UInt_t iHist = 0; iHist != NumPlots; ++iHist)
      theHists[iHist]->Scale(normInt/theHists[iHist]->Integral("width"));
  }

  return theHists;
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotHists::MakeHists(Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins,MinX,MaxX,XBins);
  return MakeHists(NumXBins,XBins);
}

//--------------------------------------------------------------------
void
PlotHists::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  std::vector<TH1D*> hists = MakeHists(NumXBins,XBins);
  BaseCanvas(FileBase,hists,XLabel,YLabel,logY);

  for (UInt_t i0 = 0; i0 < hists.size(); i0++)
    delete hists[i0];
}

//--------------------------------------------------------------------
void
PlotHists::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins,MinX,MaxX,XBins);
  MakeCanvas(FileBase,NumXBins,XBins,XLabel,YLabel,logY);
}
