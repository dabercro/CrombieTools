#include <iostream>
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TMath.h"
#include "TH2D.h"

#include "Plot2D.h"

ClassImp(Plot2D)

//--------------------------------------------------------------------
Plot2D::Plot2D() :
  fInExprX(""),
  fFunctionString(""),
  fLooseFunction("")
{
  fFits.resize(0);
  fCovs.resize(0);
  fParamFrom.resize(0);
  fParamTo.resize(0);
  fParams.resize(0);
  fParamLows.resize(0);
  fParamHighs.resize(0);
  fLooseParams.resize(0);
  fLooseParamLows.resize(0);
  fLooseParamHighs.resize(0);
  fInExprXs.resize(0);
}

//--------------------------------------------------------------------
Plot2D::~Plot2D()
{
  ClearFits();
}

//--------------------------------------------------------------------
void
Plot2D::ClearFits()
{
  fFits.resize(0);
  fCovs.resize(0);
}

//--------------------------------------------------------------------
void
Plot2D::SetInitialGuess(Int_t param, Double_t guess)
{
  fGuessParams.push_back(param);
  fGuesses.push_back(guess);
}

//--------------------------------------------------------------------
void
Plot2D::SetParameterLimits(Int_t param, Double_t low, Double_t high)
{
  fParams.push_back(param);
  fParamLows.push_back(low);
  fParamHighs.push_back(high);
}

//--------------------------------------------------------------------
void
Plot2D::SetLooseLimits(Int_t param, Double_t low, Double_t high)
{
  fLooseParams.push_back(param);
  fLooseParamLows.push_back(low);
  fLooseParamHighs.push_back(high);
}

//--------------------------------------------------------------------
void
Plot2D::MapTo(TF1* fitFunc, TF1* looseFunc)
{
  for (UInt_t iParam = 0; iParam != fParamFrom.size(); ++iParam)
    fitFunc->SetParameter(fParamTo[iParam],looseFunc->GetParameter(fParamFrom[iParam]));
}

//--------------------------------------------------------------------
void
Plot2D::DoFit(TF1* fitFunc, TF1* looseFunc, TH2D* histToFit, 
              TF1** fitHolder, TMatrixDSym** covHolder)
{
  fitHolder = new TF1*[1];
  covHolder = new TMatrixDSym*[1];
  if (fLooseFunction != "") {
    histToFit->Fit(looseFunc,"MLESQ");
    MapTo(fitFunc,looseFunc);
  }
  TFitResultPtr fitResult = histToFit->Fit(fitFunc,"MLESQ");
  fitHolder[0] = (TF2*) fitFunc->Clone();
  *covHolder[0] = fitResult->GetCovarianceMatrix();
}

//--------------------------------------------------------------------
void
Plot2D::DoFits(Int_t NumXBins, Double_t *XBins,
               Int_t NumYBins, Double_t MinY, Double_t MaxY)
{
  ClearFits();

  UInt_t NumPlots = 0;

  if (fFunctionString == "") {
    std::cout << "You haven't set a function!" << std::endl;
    exit(1);
  }

  if (fInExprX == "" && fInExprXs.size() == 0) {
    std::cout << "You haven't initialized an x expression yet!" << std::endl;
    exit(1);
  }

  if (fInTrees.size() > 0)
    NumPlots = fInTrees.size();
  else if (fInCuts.size() > 0)
    NumPlots = fInCuts.size();
  else
    NumPlots = fInExpr.size();

  if(NumPlots == 0){
    std::cout << "Nothing has been initialized in resolution plot." << std::endl;
    exit(1);
  }

  TTree *inTree = fDefaultTree;
  TString inCut = fDefaultCut;
  TString inExpr = fDefaultExpr;

  TH2D *tempHist;
  TProfile *tempProfile;

  TF1 *looseFunc = MakeFunction(fLooseFunction,XBins[0],XBins[NumXBins - 1],MinY,MaxY);
  TF1 *fitFunc = MakeFunction(fFunctionString,XBins[0],XBins[NumXBins - 1],MinY,MaxY);

  for (UInt_t i0 = 0; i0 < fLooseParams.size(); i0++)
    looseFunc->SetParLimits(fLooseParams[i0],fLooseParamLows[i0],fLooseParamHighs[i0]);

  for (UInt_t i0 = 0; i0 < fParams.size(); i0++)
    fitFunc->SetParLimits(fParams[i0],fParamLows[i0],fParamHighs[i0]);

  std::cout <<  NumPlots << " lines will be made." << std::endl;

  TF1 **holdFits;
  TMatrixDSym **holdCovs;

  for (UInt_t iPlot = 0; iPlot < NumPlots; iPlot++) {
    std::cout << NumPlots - iPlot << " more to go." << std::endl;

    if (fInTrees.size() != 0)
      inTree = fInTrees[iPlot];
    if (fInCuts.size()  != 0)
      inCut  = fInCuts[iPlot];
    if (fInExpr.size() != 0)
      inExpr = fInExpr[iPlot];
    if (fInExprXs.size() != 0)
      fInExprX = fInExprXs[iPlot];

    TString tempName;
    tempName.Form("Hist_%d",fPlotCounter);
    fPlotCounter++;
    tempHist = new TH2D(tempName,tempName,NumXBins,XBins,NumYBins,MinY,MaxY);
    tempHist->Sumw2();
    inTree->Draw(inExpr+":"+fInExprX+">>"+tempName,inCut);

    TString dumpTitle = fLegendEntries[iPlot] + ";" + inExpr + ";Num Events";
    looseFunc->SetTitle(dumpTitle);
    fitFunc->SetTitle(dumpTitle);
    tempHist->SetTitle(fLegendEntries[iPlot] + ";" + fInExprX + ";" + inExpr + ";Num Events");

    DoFit(fitFunc, looseFunc, tempHist, holdFits, holdCovs);

    fFits.push_back(holdFits);
    fCovs.push_back(holdCovs);
    delete tempHist;
  }
}

//--------------------------------------------------------------------
void
Plot2D::DoFits(Int_t NumXBins, Double_t MinX, Double_t MaxX,
               Int_t NumYBins, Double_t MinY, Double_t MaxY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins,MinX,MaxX,XBins);
  DoFits(NumXBins,XBins,NumYBins,MinY,MaxY);
}

// //--------------------------------------------------------------------
// void
// Plot2D::MakeCanvas(TString FileBase, TString ParameterExpr, TString XLabel, TString YLabel, 
//                    Double_t YMin, Double_t YMax, Bool_t logY)
// {
//   std::vector<TGraphErrors*> theGraphs = MakeGraphs(ParameterExpr);
//   MakeCanvas(FileBase,theGraphs,XLabel,YLabel,YMin,YMax,logY);
//   for (UInt_t iGraph = 0; iGraph != theGraphs.size(); ++iGraph)
//     delete theGraphs[iGraph];
// }
