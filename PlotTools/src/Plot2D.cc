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
  fLooseFunction(""),
  fDumpingFits(false),
  fNumFitDumps(0),
  fNumPoints(100)
{
  fFits.resize(0);
  fCovs.resize(0);
  fGuessParams.resize(0);
  fGuesses.resize(0);
  fLooseGuessParams.resize(0);
  fLooseGuesses.resize(0);
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
              TF1**& fitHolder, TMatrixDSym**& covHolder)
{
  fitHolder = new TF1*[1];
  covHolder = new TMatrixDSym*[1];
  TCanvas *tempCanvas = new TCanvas();
  if (fLooseFunction != "") {
    histToFit->Fit(looseFunc,"MLESQ");
    MapTo(fitFunc,looseFunc);
  }
  TFitResultPtr fitResult = histToFit->Fit(fitFunc,"MLES");
  fitHolder[0] = (TF2*) fitFunc->Clone();
  covHolder[0] = new TMatrixDSym(fitFunc->GetNpar());
  *covHolder[0] = fitResult->GetCovarianceMatrix();
  if (fDumpingFits) {
    TString dumpName;
    dumpName.Form("DumpFit_%04d_2D",fNumFitDumps);
    ++fNumFitDumps;
    tempCanvas->SaveAs(dumpName+".png");
    tempCanvas->SaveAs(dumpName+".pdf");
    tempCanvas->SaveAs(dumpName+".C");
  }
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

  if (fInTrees.size() != 0)
    NumPlots = fInTrees.size();
  else if (fInCuts.size() != 0)
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

  TF1 *looseFunc = 0;
  if (fLooseFunction != "")
    looseFunc = MakeFunction(fLooseFunction,XBins[0],XBins[NumXBins - 1],MinY,MaxY);
  for (UInt_t iGuess = 0; iGuess != fLooseGuessParams.size(); ++iGuess)
    looseFunc->SetParameter(fLooseGuessParams[iGuess],fLooseGuesses[iGuess]);
  for (UInt_t iParam = 0; iParam != fLooseParams.size(); ++iParam)
    looseFunc->SetParLimits(fLooseParams[iParam],fLooseParamLows[iParam],fLooseParamHighs[iParam]);

  TF1 *fitFunc = MakeFunction(fFunctionString,XBins[0],XBins[NumXBins - 1],MinY,MaxY);
  for (UInt_t iGuess = 0; iGuess != fGuessParams.size(); ++iGuess)
    fitFunc->SetParameter(fGuessParams[iGuess],fGuesses[iGuess]);
  for (UInt_t iParam = 0; iParam != fParams.size(); ++iParam)
    fitFunc->SetParLimits(fParams[iParam],fParamLows[iParam],fParamHighs[iParam]);
  
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
    if (fLooseFunction != "")
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

//--------------------------------------------------------------------
std::vector<TF1*>
Plot2D::MakeFuncs(TString ParameterExpr, Double_t MinX, Double_t MaxX)
{
  TF1 *tempFunc;
  std::vector<TF1*> theFuncs;

  for (UInt_t iLine = 0; iLine != fFits.size(); ++iLine) {
    tempFunc = new TF1("parameterHolder",ParameterExpr,MinX,MaxX);

    for (Int_t iParam = 0; iParam != tempFunc->GetNpar(); ++iParam) {
      Int_t parNumInFit = fFits[iLine][0]->GetParNumber(tempFunc->GetParName(iParam));
      tempFunc->SetParameter(iParam,fFits[iLine][0]->GetParameter(parNumInFit));
      tempFunc->SetParError(iParam,fFits[iLine][0]->GetParError(parNumInFit));
    }
    theFuncs.push_back(tempFunc);
  }

  return theFuncs;
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
Plot2D::MakeGraphs(TString ParameterExpr)
{
  Double_t MinX = 0.0;
  Double_t MaxX = 0.0;
  fFits[0][0]->GetRange(MinX,MaxX);
  std::vector<TF1*> theFuncs = MakeFuncs(ParameterExpr,MinX,MaxX);
  std::vector<TGraphErrors*> theGraphs;
  TGraphErrors* tempGraph;
  Double_t width = (MaxX - MinX)/fNumPoints;
  for (UInt_t iFunc = 0; iFunc != theFuncs.size(); ++iFunc) {
    std::vector<TF1*> holdFuncs;
    for (Int_t iParam = 0; iParam != theFuncs[iFunc]->GetNpar(); ++iParam) {
      TString parToReplace = TString('[') + theFuncs[iFunc]->GetParName(iParam) + TString(']');
      TF1* holdFunc = new TF1("holding",theFuncs[iFunc]->GetExpFormula().ReplaceAll('x',"[x0]").ReplaceAll(parToReplace,'x'));
      for (Int_t jParam = 0; jParam != holdFunc->GetNpar(); ++jParam) {
        TString paramName = holdFunc->GetParName(jParam);
        if (paramName == "x0")
          continue;
        holdFunc->SetParameter(jParam,theFuncs[iFunc]->GetParameter(paramName));
      }
      holdFuncs.push_back(holdFunc);
    }
    tempGraph = new TGraphErrors(fNumPoints);
    Double_t xVal = MinX;
    for (Int_t iPoint = 0; iPoint != fNumPoints + 1; ++iPoint) {
      tempGraph->SetPoint(iPoint,xVal,theFuncs[iFunc]->Eval(xVal));
      Double_t error2 = 0;
      for (Int_t iParam = 0; iParam != theFuncs[iFunc]->GetNpar(); ++iParam) {
        holdFuncs[iParam]->SetParameter("x0",xVal);
        for (Int_t jParam = 0; jParam != iParam + 1; ++jParam) {
          Double_t factor = 2.0;
          if (iParam == jParam)
            factor = 1.0;
          Double_t toAdd = holdFuncs[iParam]->Derivative(theFuncs[iFunc]->GetParameter(iParam)) * ((*(fCovs[iFunc][0]))(iParam,jParam));
          error2 += toAdd*toAdd;
        }
      }
      tempGraph->SetPointError(iPoint,0,TMath::Sqrt(error2));
      xVal += width;
    }
    theGraphs.push_back(tempGraph);
    for (UInt_t iParam = 0; iParam != holdFuncs.size(); ++iParam)
      delete holdFuncs[iParam];
  }

  for (UInt_t iFunc = 0; iFunc != theFuncs.size(); ++iFunc)
    delete theFuncs[iFunc];
  return theGraphs;
}

//--------------------------------------------------------------------
void
Plot2D::MakeCanvas(TString FileBase, std::vector<TGraphErrors*> theGraphs, TString XLabel, TString YLabel, 
                   Double_t YMin, Double_t YMax, Bool_t logY)
{
  for (UInt_t iGraph = 0; iGraph != theGraphs.size(); ++iGraph)
    theGraphs[iGraph]->GetYaxis()->SetRangeUser(YMin,YMax);
  BaseCanvas(FileBase,theGraphs,XLabel,YLabel,logY);
}

//--------------------------------------------------------------------
void
Plot2D::MakeCanvas(TString FileBase, TString ParameterExpr, TString XLabel, TString YLabel, 
                   Double_t YMin, Double_t YMax, Bool_t logY)
{
  std::vector<TGraphErrors*> theGraphs = MakeGraphs(ParameterExpr);
  MakeCanvas(FileBase,theGraphs,XLabel,YLabel,YMin,YMax,logY);
  for (UInt_t iGraph = 0; iGraph != theGraphs.size(); ++iGraph)
    delete theGraphs[iGraph];
}

//--------------------------------------------------------------------
void
Plot2D::MakeCanvas(TString FileBase, Int_t ParameterNum, TString XLabel, TString YLabel,
                   Double_t YMin, Double_t YMax, Bool_t logY)
{
  TString ParameterExpr = "";
  ParameterExpr.Form("[%d]",ParameterNum);
  MakeCanvas(FileBase,ParameterExpr,XLabel,YLabel,YMin,YMax,logY);
}
