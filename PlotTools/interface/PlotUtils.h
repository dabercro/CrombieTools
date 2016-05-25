#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTUTILS_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTUTILS_H

#include "TF1.h"
#include "TH1D.h"
#include "TH1.h"
#include "TGraphErrors.h"

/** @addtogroup plotgroup */
/* @{ */

//--------------------------------------------------------------------
void
SetZeroError(TF1* theFunc)
{ 
  for (Int_t iParam = 0; iParam != theFunc->GetNpar(); ++iParam) 
    theFunc->SetParError(iParam, 0);
}

//--------------------------------------------------------------------
void
SetZeroError(TH1* theHist)
{ 
  for (Int_t iBin = 0; iBin != theHist->GetXaxis()->GetNbins(); ++iBin) 
    theHist->SetBinError(iBin + 1, 0);
}

//--------------------------------------------------------------------
void
SetZeroError(TGraphErrors* theGraph)
{ 
  for (Int_t iPoint = 0; iPoint != theGraph->GetN(); ++iPoint) 
    theGraph->SetPointError(iPoint,0,0);
}

//--------------------------------------------------------------------
void
SetZeroError(TGraph*)
{ }

//--------------------------------------------------------------------
void
Division(TF1*& PlotFunc, TF1* RatioFunc)
{
  TString plotString = TString("(") + PlotFunc->GetExpFormula() + TString(")");
  TString ratString = TString("(") + RatioFunc->GetExpFormula().ReplaceAll('p','q') + TString(")");
  TF1 *tempFunc = new TF1("divided",plotString + TString("/") + ratString);
  for (Int_t iParam = 0; iParam != PlotFunc->GetNpar() + RatioFunc->GetNpar(); ++iParam) {
    if (iParam < PlotFunc->GetNpar()) {
      tempFunc->SetParameter(iParam, PlotFunc->GetParameter(iParam));
      tempFunc->SetParError(iParam, PlotFunc->GetParError(iParam));
    }
    else {
      tempFunc->SetParameter(iParam, RatioFunc->GetParameter(iParam - PlotFunc->GetNpar()));
      tempFunc->SetParError(iParam, RatioFunc->GetParError(iParam - PlotFunc->GetNpar()));
    }
  }
  delete PlotFunc;
  PlotFunc = tempFunc;
}

//--------------------------------------------------------------------
void
SetGraphErrorsForRatio(TGraph*, TGraph*, Int_t)
{ }

//--------------------------------------------------------------------
void
SetGraphErrorsForRatio(TGraphErrors* PlotGraph, TGraphErrors* RatioGraph, Int_t iPoint)
{
  PlotGraph->SetPointError(iPoint,0,
                           sqrt(pow(PlotGraph->GetEY()[iPoint]/RatioGraph->GetY()[iPoint],2) +
                                pow((PlotGraph->GetY()[iPoint])*(RatioGraph->GetEY()[iPoint])/
                                    pow(RatioGraph->GetY()[iPoint],2),2)));
}

//--------------------------------------------------------------------
void
Division(TGraph* PlotGraph, TGraph* RatioGraph)
{
  Int_t NumPoints = RatioGraph->GetN();
  Double_t RangeMin = 1000.;
  Double_t RangeMax = 0.;
  for (Int_t iPoint = 0; iPoint != NumPoints; ++iPoint) {
    Double_t point = PlotGraph->GetY()[iPoint]/RatioGraph->GetY()[iPoint];
    PlotGraph->SetPoint(iPoint,PlotGraph->GetX()[iPoint],point);
    SetGraphErrorsForRatio(PlotGraph, RatioGraph, iPoint);
    if (point < RangeMin)
      RangeMin = point;
    if (point > RangeMax)
      RangeMax = point;
  }
  PlotGraph->GetYaxis()->SetRangeUser(RangeMin,RangeMax);
}

//--------------------------------------------------------------------
void
Division(TH1* PlotHist, TH1* RatioHist)
{
  PlotHist->Divide(RatioHist);
  for (Int_t iBin = 1; iBin != RatioHist->GetXaxis()->GetNbins() + 1; ++iBin) {
    if (PlotHist->GetBinContent(iBin) == 0 && RatioHist->GetBinContent(iBin) == 0)
      PlotHist->SetBinContent(iBin,1);   // @todo: Make this configurable between 0 and 1, maybe
  }
}

//--------------------------------------------------------------------
template<class T>
std::vector<T*>
GetRatioToLines(std::vector<T*> InLines, std::vector<T*> RatioLines)
{
  T* tempLine;
  std::vector<T*> outLines;
  for (UInt_t iLine = 0; iLine != InLines.size(); ++iLine) {
    tempLine = (T*) InLines[iLine]->Clone();
    Division(tempLine,RatioLines[iLine]);
    outLines.push_back(tempLine);
  }
  return outLines;
}

//--------------------------------------------------------------------
template<class T>
std::vector<T*>
GetRatioToLine(std::vector<T*> InLines, T* RatioGraph)
{
  std::vector<T*> tempRatioLines;
  for (UInt_t iLine = 0; iLine != InLines.size(); ++iLine)
    tempRatioLines.push_back(RatioGraph);
  return GetRatioToLines(InLines,tempRatioLines);
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
GetRatioToPoint(std::vector<TGraphErrors*> InGraphs, Double_t RatioPoint, Double_t PointError = 0)
{
  Int_t NumPoints = InGraphs[0]->GetN();
  Double_t* GraphX = InGraphs[0]->GetX();
  TGraphErrors tempRatioGraph(NumPoints);
  for (Int_t iPoint = 0; iPoint != NumPoints; ++iPoint) {
    tempRatioGraph.SetPoint(iPoint,GraphX[iPoint],RatioPoint);
    tempRatioGraph.SetPointError(iPoint,0,PointError);
  }
  return GetRatioToLine(InGraphs,&tempRatioGraph);
}

//--------------------------------------------------------------------
std::vector<TH1D*>
GetRatioToPoint(std::vector<TH1D*> InHists, Double_t RatioPoint, Double_t PointError = 0)
{
  TH1D tempRatioHist = *(InHists[0]);
  tempRatioHist.Reset();
  for (Int_t iBin = 0; iBin != tempRatioHist.GetXaxis()->GetNbins(); iBin++) {
    tempRatioHist.SetBinContent(iBin + 1,RatioPoint);
    tempRatioHist.SetBinError(iBin + 1,PointError);
  }
  return GetRatioToLine(InHists,&tempRatioHist);
}

//--------------------------------------------------------------------
std::vector<TF1*>
GetRatioToPoint(std::vector<TF1*> InFuncs, Double_t RatioPoint, Double_t PointError = 0)
{
  TF1 tempRatioFunc("ratio","[0]");
  tempRatioFunc.SetParameter(0,RatioPoint);
  tempRatioFunc.SetParError(0,PointError);
  return GetRatioToLine(InFuncs,&tempRatioFunc);
}
/* @} */

#endif
