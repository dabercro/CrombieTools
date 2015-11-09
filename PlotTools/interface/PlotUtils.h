#include <iostream>
#include "TGraphErrors.h"

//--------------------------------------------------------------------
//template<class T>
std::vector<TGraphErrors*>
GetRatioToLines(std::vector<TGraphErrors*> InGraphs, std::vector<TGraphErrors*> RatioGraphs)
{
  TGraphErrors *tempGraph;
  std::vector<TGraphErrors*> outGraphs;
  for (UInt_t i0 = 0; i0 < InGraphs.size(); i0++) {
    tempGraph = (TGraphErrors*) InGraphs->Clone();
    Division(tempGraph,RatioGraphs[i0]);
    outGraphs.push_back(tempGraph);
  }
  return outGraphs;
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
GetRatioToLine(std::vector<TGraphErrors*> InGraphs, TGraphErrors *RatioGraph)
{
  std::vector<TGraphErrors*> tempRatioGraphs;
  for (UInt_t i0 = 0; i0 < InGraphs.size(); i0++)
    tempRatioGraphs.push_back(RatioGraph);
  return GetRatioToLines(InGraphs,tempRatioGraphs);
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
GetRatioToPoint(std::vector<TGraphErrors*> InGraphs, Double_t RatioPoint, Double_t PointError = 0)
{
  Int_t NumPoints = InGraphs[0]->GetN();
  Double_t *GraphX = InGraphs[0]->GetX();
  TGraphErrors *tempRatioGraph = new TGraphErrors(NumPoints);
  for (Int_t i0 = 0; i0 < NumPoints; i0++) {
    tempRatioGraph->SetPoint(i0,GraphX[i0],RatioPoint);
    tempRatioGraph->SetPointError(i0,0,PointError);
  }
  return GetRatioToLine(InGraphs,tempRatioGraph);
}

//--------------------------------------------------------------------
void
PlotBase::Division(TH1 *PlotHist, TH1 *RatioHist)
{
  for (Int_t iBin = 0; iBin != RatioHist->GetXaxis()->GetNbins(); ++iBin)
    RatioHist->SetBinError(iBin + 1, 0);

  PlotHist->Divide(RatioHist);
}

//--------------------------------------------------------------------
void
PlotBase::Division(TGraphErrors *PlotGraph, TGraphErrors *RatioGraph)
{
  Double_t *GraphX = PlotGraph->GetX();
  Double_t *GraphY = PlotGraph->GetY();
  Double_t *GraphYErrors = PlotGraph->GetEY();
  Int_t NumPoints = RatioGraph->GetN();
  Double_t *RatioY = RatioGraph->GetY();
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

