#include <iostream>
#include "TGraphErrors.h"

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
GetRatioToLines(std::vector<TGraphErrors*> InGraphs, std::vector<TGraphErrors*> RatioGraphs)
{
  TGraphErrors *tempGraph;
  std::vector<TGraphErrors*> outGraphs;
  for (UInt_t i0 = 0; i0 < InGraphs.size(); i0++) {
    Double_t *GraphX = InGraphs[i0]->GetX();
    Double_t *GraphY = InGraphs[i0]->GetY();
    Double_t *GraphYErrors = InGraphs[i0]->GetEY();
    Int_t NumPoints = RatioGraphs[i0]->GetN();
    Double_t *RatioY = RatioGraphs[i0]->GetY();
    Double_t *RatioYErrors = RatioGraphs[i0]->GetEY();
    tempGraph = new TGraphErrors(NumPoints);
    for (Int_t i1 = 0; i1 < NumPoints; i1++) {
      if (InGraphs[i0]->GetN() != NumPoints) {
        std::cout << "Messed up graph size... Check that out" << std::endl;
        exit(1);
      }
      tempGraph->SetPoint(i1,GraphX[i1],GraphY[i1]/RatioY[i1]);
      tempGraph->SetPointError(i1,0,sqrt(pow(GraphYErrors[i1]/RatioY[i1],2) + pow((GraphY[i1])*(RatioYErrors[i1])/pow(RatioY[i1],2),2)));
    }
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
