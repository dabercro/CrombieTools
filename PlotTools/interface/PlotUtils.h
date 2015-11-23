#include <iostream>
#include "TF1.h"
#include "TH1D.h"
#include "TH1.h"
#include "TGraphErrors.h"

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
  for (Int_t i1 = 0; i1 != theGraph->GetN(); ++i1) 
    theGraph->SetPointError(i1,0,0);
}

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
Division(TH1* PlotHist, TH1* RatioHist)
{
  PlotHist->Divide(RatioHist);
}

//--------------------------------------------------------------------
void
Division(TGraphErrors* PlotGraph, TGraphErrors* RatioGraph)
{
  Double_t* GraphX = PlotGraph->GetX();
  Double_t* GraphY = PlotGraph->GetY();
  Double_t* GraphYErrors = PlotGraph->GetEY();
  Int_t NumPoints = RatioGraph->GetN();
  Double_t* RatioY = RatioGraph->GetY();
  Double_t* RatioYErrors = RatioGraph->GetEY();
  for (Int_t i1 = 0; i1 < NumPoints; i1++) {
    if (PlotGraph->GetN() != NumPoints) {
      std::cout << "Messed up graph size... Check that out" << std::endl;
      exit(1);
    }
    PlotGraph->SetPoint(i1,GraphX[i1],GraphY[i1]/RatioY[i1]);
    PlotGraph->SetPointError(i1,0,sqrt(pow(GraphYErrors[i1]/RatioY[i1],2) + pow((GraphY[i1])*(RatioYErrors[i1])/pow(RatioY[i1],2),2)));
  }
}

//--------------------------------------------------------------------
template<class T>
std::vector<T*>
GetRatioToLines(std::vector<T*> InLines, std::vector<T*> RatioLines)
{
  T* tempLine;
  std::vector<T*> outLines;
  for (UInt_t i0 = 0; i0 != InLines.size(); ++i0) {
    tempLine = (T*) InLines[i0]->Clone();
    Division(tempLine,RatioLines[i0]);
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
  for (UInt_t i0 = 0; i0 != InLines.size(); ++i0)
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
  for (Int_t i0 = 0; i0 != NumPoints; ++i0) {
    tempRatioGraph.SetPoint(i0,GraphX[i0],RatioPoint);
    tempRatioGraph.SetPointError(i0,0,PointError);
  }
  return GetRatioToLine(InGraphs,&tempRatioGraph);
}

//--------------------------------------------------------------------
std::vector<TH1D*>
GetRatioToPoint(std::vector<TH1D*> InHists, Double_t RatioPoint, Double_t PointError = 0)
{
  TH1D tempRatioHist = *(InHists[0]);
  tempRatioHist.Reset();
  for (Int_t i0 = 0; i0 != tempRatioHist.GetXaxis()->GetNbins(); i0++) {
    tempRatioHist.SetBinContent(i0 + 1,RatioPoint);
    tempRatioHist.SetBinError(i0 + 1,PointError);
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
