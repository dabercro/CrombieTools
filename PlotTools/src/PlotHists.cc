
#include <iostream>
#include "TLegend.h"

#include "PlotHists.h"

ClassImp(PlotHists)

//--------------------------------------------------------------------
PlotHists::PlotHists() :
  fNormalizedHists(kFALSE)
{}

//--------------------------------------------------------------------
PlotHists::~PlotHists()
{}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotHists::MakeHists(Int_t NumXBins, Double_t *XBins)
{
  UInt_t NumPlots = 0;

  if (fInTrees.size() > 0)
    NumPlots = fInTrees.size();
  else if (fInCuts.size() > 0)
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

  for (UInt_t i0 = 0; i0 < NumPlots; i0++) {

    if (fInTrees.size() != 0)
      inTree = fInTrees[i0];
    if (fInCuts.size()  != 0)
      inCut  = fInCuts[i0];
    if (fInExpr.size() != 0)
      inExpr = fInExpr[i0];

    TString tempName;
    tempName.Form("Hist_%d",fPlotCounter);
    fPlotCounter++;
    tempHist = new TH1D(tempName,tempName,NumXBins,XBins);
    if (fIncludeErrorBars)
      tempHist->Sumw2();
    inTree->Draw(inExpr+">>"+tempName,inCut);

    theHists.push_back(tempHist);
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
PlotHists::MakeCanvas(Int_t NumXBins, Double_t *XBins, TString FileBase,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  std::vector<TH1D*> hists = MakeHists(NumXBins,XBins);
  BaseCanvas(hists,FileBase,XLabel,YLabel,logY);

  for (UInt_t i0 = 0; i0 < hists.size(); i0++)
    delete hists[i0];
}

//--------------------------------------------------------------------
void
PlotHists::MakeCanvas(Int_t NumXBins, Double_t MinX, Double_t MaxX, TString FileBase,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins,MinX,MaxX,XBins);
  MakeCanvas(NumXBins,XBins,FileBase,XLabel,YLabel,logY);
}

// //--------------------------------------------------------------------
// void
// PlotHists::MakeCanvas(Int_t NumXBins, Double_t MinX, Double_t MaxX, TString FileBase,
//                       TString CanvasTitle, TString XLabel, TString YLabel,
//                       Bool_t logY, Int_t ratPlot)
// {
//   Double_t binWidth = (MaxX - MinX)/NumXBins;
//   Double_t XBins[NumXBins+1];
//   for (Int_t i0 = 0; i0 < NumXBins + 1; i0++)
//     XBins[i0] = MinX + i0 * binWidth;


//   MakeCanvas(NumXBins,XBins,FileBase,CanvasTitle,XLabel,YLabel,logY,ratPlot);
// }

// //--------------------------------------------------------------------
// void
// PlotHists::MakeRatio(Int_t NumXBins, Double_t MinX, Double_t MaxX, TString FileBase,
//                      TString CanvasTitle, TString XLabel, TString YLabel,
//                      Int_t DataNum)
// {
//   std::vector<TH1D*> hists = MakeHists(NumXBins,MinX,MaxX,DataNum);
//   TH1D *tempHist = (TH1D*) hists[DataNum]->Clone("ValueHolder");
//   for (UInt_t iHists = 0; iHists < hists.size(); iHists++) {
//     hists[iHists]->Divide(tempHist);
//   }

//   TCanvas *theCanvas = MakeCanvas(hists,CanvasTitle,
//                                   XLabel,YLabel,false);

//   theCanvas->SaveAs(FileBase+".C");
//   theCanvas->SaveAs(FileBase+".png");
//   theCanvas->SaveAs(FileBase+".pdf");

//   delete theCanvas;
//   for (UInt_t i0 = 0; i0 < hists.size(); i0++)
//     delete hists[i0];

//   delete tempHist;
// }
