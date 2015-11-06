#include <iostream>
#include "TF1.h"
#include "TH2D.h"
#include "TLegend.h"

#include "PlotFitParameters.h"

using namespace mithep;

ClassImp(mithep::PlotFitParameters)

//--------------------------------------------------------------------
PlotFitParameters::PlotFitParameters() :
  fInExprX(""),
  fFunctionString(""),
  fLooseFunction(""),
  fDumpingFits(false),
  fNumFitDumps(0)
{
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
{}

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
std::vector<TGraphErrors*>
PlotFitParameters::MakeFitGraphs(Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                 Int_t NumYBins, Double_t MinY, Double_t MaxY,
                                 Int_t NumParam)
{
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

  for (UInt_t i0 = 0; i0 < NumPlots; i0++) {
    std::cout << NumPlots - i0 << " more to go." << std::endl;

    if (fInTrees.size() != 0)
      inTree = fInTrees[i0];
    if (fInCuts.size()  != 0)
      inCut  = fInCuts[i0];
    if (fInExpr.size() != 0)
      inExpr = fInExpr[i0];
    if (fInExprXs.size() != 0)
      fInExprX = fInExprXs[i0];

    TString tempName;
    tempName.Form("Hist_%d",fPlotCounter);
    fPlotCounter++;
    tempHist = new TH2D(tempName,tempName,NumXBins,MinX,MaxX,NumYBins,MinY,MaxY);
    inTree->Draw(inExpr+":"+fInExprX+">>"+tempName,inCut);
    tempGraph = new TGraphErrors(NumXBins);

    for (Int_t i1 = 0; i1 < NumXBins; i1++) {
      TCanvas *tempCanvas = new TCanvas();
      if (fLooseFunction != "") {
        tempHist->ProjectionY(tempName+"_py",i1+1,i1+1)->Fit(looseFunc,"","",MinY,MaxY);
        for (UInt_t iParam = 0; iParam < fParamFrom.size(); iParam++)
          fitFunc->SetParameter(fParamTo[iParam],looseFunc->GetParameter(fParamFrom[iParam]));
      }
      tempHist->ProjectionY(tempName+"_py",i1+1,i1+1)->Fit(fitFunc,"","",MinY,MaxY);
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
      // This is where we do the tricky parameter fetching stuff!!
      tempGraph->SetPoint(i1,tempHist->GetXaxis()->GetBinCenter(i1+1),fitFunc->GetParameter(NumParam));
      if (fIncludeErrorBars)
        tempGraph->SetPointError(i1,0,fitFunc->GetParError(NumParam));
    }
    theGraphs.push_back(tempGraph);
    delete tempHist;
  }
  return theGraphs;
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
PlotFitParameters::MakeFitGraphs(Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                 Int_t NumYBins, Double_t MinY, Double_t MaxY,
                                 TString ParamExpr, TString ErrorExpr)
{
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

  TF1 *paramFunc = new TF1("paramFunc",ParamExpr,-1,1);
  TF1 *errorFunc = new TF1("errorFunc",ErrorExpr,-1,1);

  for (UInt_t i0 = 0; i0 < NumPlots; i0++) {
    std::cout << NumPlots - i0 << " more to go." << std::endl;

    if (fInTrees.size() != 0)
      inTree = fInTrees[i0];
    if (fInCuts.size()  != 0)
      inCut  = fInCuts[i0];
    if (fInExpr.size() != 0)
      inExpr = fInExpr[i0];
    if (fInExprXs.size() != 0)
      fInExprX = fInExprXs[i0];

    TString tempName;
    tempName.Form("Hist_%d",fPlotCounter);
    fPlotCounter++;
    tempHist = new TH2D(tempName,tempName,NumXBins,MinX,MaxX,NumYBins,MinY,MaxY);
    inTree->Draw(inExpr+":"+fInExprX+">>"+tempName,inCut);
    tempGraph = new TGraphErrors(NumXBins);

    for (Int_t i1 = 0; i1 < NumXBins; i1++) {
      TCanvas *tempCanvas = new TCanvas();
      if (fLooseFunction != "") {
        tempHist->ProjectionY(tempName+"_py",i1+1,i1+1)->Fit(looseFunc,"","",MinY,MaxY);
        for (UInt_t iParam = 0; iParam < fParamFrom.size(); iParam++)
          fitFunc->SetParameter(fParamTo[iParam],looseFunc->GetParameter(fParamFrom[iParam]));
      }
      tempHist->ProjectionY(tempName+"_py",i1+1,i1+1)->Fit(fitFunc,"","",MinY,MaxY);
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
      // This is where we do the tricky parameter fetching stuff!!
      paramFunc->SetParameters(fitFunc->GetParameters());
      tempGraph->SetPoint(i1,tempHist->GetXaxis()->GetBinCenter(i1+1),paramFunc->Eval(0));
      if (fIncludeErrorBars) {
        errorFunc->SetParameters(fitFunc->GetParameters());
        tempGraph->SetPointError(i1,0,errorFunc->Eval(0));
      }
    }
    theGraphs.push_back(tempGraph);
    delete tempHist;
  }
  delete paramFunc;
  delete errorFunc;
  return theGraphs;
}

//--------------------------------------------------------------------
TCanvas*
PlotFitParameters::MakeCanvas(std::vector<TGraphErrors*> theGraphs,
                           TString CanvasTitle, TString XLabel, TString YLabel,
                           Double_t YMin, Double_t YMax, Bool_t logY)
{
  UInt_t NumPlots = theGraphs.size();
  TCanvas *theCanvas = new TCanvas(fCanvasName,fCanvasName);
  theCanvas->SetTitle(CanvasTitle+";"+XLabel+";"+YLabel);
  TLegend *theLegend = new TLegend(l1,l2,l3,l4);
  theLegend->SetBorderSize(fLegendBorderSize);
  for (UInt_t i0 = 0; i0 < NumPlots; i0++) {
    theGraphs[i0]->SetTitle(CanvasTitle+";"+XLabel+";"+YLabel);
    theGraphs[i0]->SetLineWidth(fLineWidths[i0]);
    theGraphs[i0]->SetLineStyle(fLineStyles[i0]);
    theGraphs[i0]->SetLineColor(fLineColors[i0]);
    theLegend->AddEntry(theGraphs[i0],fLegendEntries[i0],"l");
  }
  theGraphs[0]->GetYaxis()->SetRangeUser(YMin,YMax);
  theGraphs[0]->Draw();
  for (UInt_t i0 = 1; i0 < NumPlots; i0++)
    theGraphs[i0]->Draw("same");

  theLegend->Draw();
  if (logY)
    theCanvas->SetLogy();

  return theCanvas;
}

//--------------------------------------------------------------------
void
PlotFitParameters::MakeCanvas(TString FileBase, std::vector<TGraphErrors*> theGraphs,
                           TString CanvasTitle, TString XLabel, TString YLabel,
                           Double_t YMin, Double_t YMax, Bool_t logY)
{
  TCanvas *theCanvas = MakeCanvas(theGraphs, CanvasTitle, XLabel, YLabel, YMin, YMax, logY);

  theCanvas->SaveAs(FileBase+".C");
  theCanvas->SaveAs(FileBase+".png");
  theCanvas->SaveAs(FileBase+".pdf");

  delete theCanvas;
}
