#include <iostream>
#include "TH2D.h"
#include "TLegend.h"

#include "PlotFitParameters.h"

ClassImp(PlotFitParameters)

//--------------------------------------------------------------------
PlotFitParameters::PlotFitParameters() :
  fFitXBins(0),
  fInExprX(""),
  fFunctionString(""),
  fLooseFunction(""),
  fDumpingFits(false),
  fNumFitDumps(0)
{
  fFits.resize(0);
  fFunctionComponents.resize(0);
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
PlotFitParameters::~PlotFitParameters()
{
  ClearFits();
}

//--------------------------------------------------------------------
void
PlotFitParameters::ClearFits()
{
  for (UInt_t iPlot = 0; iPlot != fFits.size(); ++iPlot) {
    for (Int_t iXBin = 0; iXBin != fFitXBins; ++iXBin) {
      delete fFits[iPlot][iXBin];
    }
  }
  fFits.resize(0);
}

//--------------------------------------------------------------------
void
PlotFitParameters::SetParameterLimits(Int_t param, Double_t low, Double_t high)
{
  fParams.push_back(param);
  fParamLows.push_back(low);
  fParamHighs.push_back(high);
}

//--------------------------------------------------------------------
void
PlotFitParameters::SetLooseLimits(Int_t param, Double_t low, Double_t high)
{
  fLooseParams.push_back(param);
  fLooseParamLows.push_back(low);
  fLooseParamHighs.push_back(high);
}

//--------------------------------------------------------------------
void
PlotFitParameters::MakeFitGraphs(Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                 Int_t NumYBins, Double_t MinY, Double_t MaxY)
{
  ClearFits();
  fFitXBins = NumXBins;

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

  TGraphErrors *tempGraph;
  std::vector<TGraphErrors*> theGraphs;

  TF1 *looseFunc = new TF1("func",fLooseFunction,MinY,MaxY);
  TF1 *fitFunc = new TF1("func",fFunctionString,MinY,MaxY);

  for (UInt_t i0 = 0; i0 < fLooseParams.size(); i0++)
    looseFunc->SetParLimits(fLooseParams[i0],fLooseParamLows[i0],fLooseParamHighs[i0]);

  for (UInt_t i0 = 0; i0 < fParams.size(); i0++)
    fitFunc->SetParLimits(fParams[i0],fParamLows[i0],fParamHighs[i0]);

  std::cout <<  NumPlots << " lines will be made." << std::endl;

  TF1 **holdFits;

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
    tempHist = new TH2D(tempName,tempName,NumXBins,MinX,MaxX,NumYBins,MinY,MaxY);
    inTree->Draw(inExpr+":"+fInExprX+">>"+tempName,inCut);
    tempGraph = new TGraphErrors(NumXBins);

    holdFits = new TF1*[NumXBins];

    for (Int_t iXBin = 0; iXBin < NumXBins; iXBin++) {
      TCanvas *tempCanvas = new TCanvas();
      if (fLooseFunction != "") {
        tempHist->ProjectionY(tempName+"_py",iXBin+1,iXBin+1)->Fit(looseFunc,"","",MinY,MaxY);
        for (UInt_t iParam = 0; iParam < fParamFrom.size(); iParam++)
          fitFunc->SetParameter(fParamTo[iParam],looseFunc->GetParameter(fParamFrom[iParam]));
      }
      tempHist->ProjectionY(tempName+"_py",iXBin+1,iXBin+1)->Fit(fitFunc,"","",MinY,MaxY);
      if (fDumpingFits) {
        TString dumpName;
        dumpName.Form("DumpFit_%d",fNumFitDumps);
        std::vector<TF1*> components;
        for (UInt_t iFunc = 0; iFunc < fFunctionComponents.size(); iFunc++) {
          TF1 *tempComponent = new TF1(dumpName,fFunctionComponents.size(),MinY,MaxY);
          tempComponent->SetParameters(fitFunc->GetParameters());
          tempComponent->Draw("SAME");
          components.push_back(tempComponent);
        }
        tempCanvas->SaveAs(dumpName+".png");
        for (UInt_t iComp = 0; iComp < components.size(); iComp++)
          delete components[iComp];
        components.clear();
        fNumFitDumps++;
      }
      holdFits[iXBin] = (TF1*) fitFunc->Clone();
    }
    fFits.push_back(holdFits);
    delete tempHist;
  }
}

// //--------------------------------------------------------------------
// TCanvas*
// PlotFitParameters::MakeCanvas(std::vector<TGraphErrors*> theGraphs,
// 			      TString CanvasTitle, TString XLabel, TString YLabel,
// 			      Double_t YMin, Double_t YMax, Bool_t logY)
// {
//   UInt_t NumPlots = theGraphs.size();
//   TCanvas *theCanvas = new TCanvas(fCanvasName,fCanvasName);
//   theCanvas->SetTitle(CanvasTitle+";"+XLabel+";"+YLabel);
//   TLegend *theLegend = new TLegend(l1,l2,l3,l4);
//   theLegend->SetBorderSize(fLegendBorderSize);
//   for (UInt_t iPlot = 0; iPlot < NumPlots; iPlot++) {
//     theGraphs[iPlot]->SetTitle(CanvasTitle+";"+XLabel+";"+YLabel);
//     theGraphs[iPlot]->SetLineWidth(fLineWidths[iPlot]);
//     theGraphs[iPlot]->SetLineStyle(fLineStyles[iPlot]);
//     theGraphs[iPlot]->SetLineColor(fLineColors[iPlot]);
//     theLegend->AddEntry(theGraphs[iPlot],fLegendEntries[iPlot],"l");
//   }
//   theGraphs[0]->GetYaxis()->SetRangeUser(YMin,YMax);
//   theGraphs[0]->Draw();
//   for (UInt_t iPlot = 1; iPlot < NumPlots; iPlot++)
//     theGraphs[iPlot]->Draw("same");

//   theLegend->Draw();
//   if (logY)
//     theCanvas->SetLogy();

//   return theCanvas;
// }

//--------------------------------------------------------------------
void
MakeCanvas(TString FileBase, TString ParameterExpr, TString XLabel, TString YLabel, 
	   Double_t YMin, Double_t YMax, Bool_t logY)
{
  std::cout << FileBase << ParameterExpr << XLabel << YLabel << YMin << YMax << logY << std::endl;
}

//--------------------------------------------------------------------
void
MakeCanvas(TString FileBase, Int_t ParameterNum, TString XLabel, TString YLabel,
	   Double_t YMin, Double_t YMax, Bool_t logY)
{
  TString ParameterExpr = "";
  ParameterExpr.Form("[%d]",ParameterNum);
  MakeCanvas(FileBase, ParameterExpr, XLabel, YLabel, YMin, YMax, logY);
}
