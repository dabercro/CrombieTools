/**
  @file   PlotROC.cc
  Defines function members of the PlotROC class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <iostream>

#include "PlotROC.h"

ClassImp(PlotROC)

//--------------------------------------------------------------------
PlotROC::PlotROC()
{ }

//--------------------------------------------------------------------
PlotROC::~PlotROC()
{ }

//--------------------------------------------------------------------
TGraph*
PlotROC::MakeROC(TString CutVar, Int_t NumBins)
{
  // Get the minimum and maximum values of the histograms
  TH1F *htemp;
  fSignalTree->Draw(CutVar, fSignalCut);
  htemp = (TH1F*) gPad->GetPrimitive("htemp");
  Double_t XMin = htemp->GetXaxis()->GetBinLowEdge(1);
  Int_t   NBins = htemp->GetNbinsX();
  Double_t XMax = htemp->GetXaxis()->GetBinLowEdge(NBins) + htemp->GetXaxis()->GetBinWidth(NBins);
  fBackgroundTree->Draw(CutVar,fBackgroundCut);
  htemp = (TH1F*) gPad->GetPrimitive("htemp");
  Double_t checkM = htemp->GetXaxis()->GetBinLowEdge(1);
  if (XMin > checkM)
    XMin = checkM;
  NBins = htemp->GetNbinsX();
  checkM = htemp->GetXaxis()->GetBinLowEdge(NBins) + htemp->GetXaxis()->GetBinWidth(NBins);
  if (XMax < checkM)
    XMax = checkM;

  fPlotHists.SetDefaultExpr(CutVar);
  fPlotHists.ResetTree();
  fPlotHists.ResetWeight();
  fPlotHists.AddTreeWeight(fSignalTree, fSignalCut);
  fPlotHists.AddTreeWeight(fBackgroundTree, fBackgroundCut);

  std::vector<TH1D*> theHists = fPlotHists.MakeHists(NumBins, XMin, XMax);
  const Int_t numPoints = NumBins + 1;
  Double_t XVals[numPoints];
  Double_t RevXVals[numPoints];
  Double_t YVals[numPoints];
  Double_t RevYVals[numPoints];

  Double_t sigArea  = theHists[0]->Integral();
  Double_t backArea = theHists[1]->Integral();
  if (fPlotType == kROC) {
    for (Int_t iPoint = 0; iPoint < numPoints; iPoint++) {
      XVals[iPoint]    = theHists[0]->Integral(iPoint, numPoints)/sigArea;
      RevXVals[iPoint] = theHists[0]->Integral(0, numPoints-iPoint)/sigArea;
      YVals[iPoint]    = theHists[1]->Integral(iPoint, numPoints)/backArea;
      RevYVals[iPoint] = theHists[1]->Integral(0, numPoints-iPoint)/backArea;
    }
  }
  else if (fPlotType == kSignificance) {
    for (Int_t iPoint = 0; iPoint < numPoints; iPoint++) {
      XVals[iPoint] = theHists[0]->GetXaxis()->GetBinLowEdge(iPoint);
      RevXVals[iPoint] = theHists[0]->GetXaxis()->GetBinLowEdge(iPoint);

      sigArea  = theHists[0]->Integral(iPoint, numPoints);
      backArea = theHists[1]->Integral(iPoint, numPoints);
      YVals[iPoint] = (backArea + sigArea == 0) ? 0 : sigArea / sqrt(sigArea + backArea);

      sigArea  = theHists[0]->Integral(0, numPoints - iPoint);
      backArea = theHists[1]->Integral(0, numPoints - iPoint);
      YVals[iPoint] = (backArea + sigArea == 0) ? 0 : sigArea / sqrt(sigArea + backArea);
    }
  }

  TGraph *rocCurve    = new TGraph(numPoints, XVals, YVals);
  TGraph *revRocCurve = new TGraph(numPoints, RevXVals, RevYVals);
  delete theHists[0];
  delete theHists[1];
  theHists.resize(0);

  if (revRocCurve->Integral() > rocCurve->Integral()) {
    delete rocCurve;
    return revRocCurve;
  }
  else {
    delete revRocCurve;
    return rocCurve;
  }
}

//--------------------------------------------------------------------
std::vector<TGraph*>
PlotROC::MakeROCs(Int_t NumBins)
{
  std::vector<TGraph*> theGraphs;
  for (UInt_t i0 = 0; i0 < fROCVars.size(); i0++)
    theGraphs.push_back(MakeROC(fROCVars[i0], NumBins));

  return theGraphs;
}

//--------------------------------------------------------------------
void
PlotROC::MakeCanvas(TString FileBase, Int_t NumBins, TString XLabel,
                    TString YLabel, Bool_t logY, Bool_t logX)
{
  if (fAxisMin == fAxisMax)
    SetAxisMinMax(0.0,1.0);

  std::vector<TGraph*> rocs = MakeROCs(NumBins);
  BaseCanvas(FileBase, rocs, XLabel, YLabel, logY, logX);

  for (UInt_t i0 = 0; i0 != rocs.size(); ++i0)
    delete rocs[i0];
}
