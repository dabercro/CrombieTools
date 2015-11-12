#include <iostream>
#include "TMath.h"
#include "TH2D.h"

#include "PlotFitParameters.h"

ClassImp(PlotFitParameters)

//--------------------------------------------------------------------
PlotFitParameters::PlotFitParameters() :
  fFitXBins(0),
  fMeans(0),
  fInExprX(""),
  fFunctionString(""),
  fLooseFunction(""),
  fDumpingFits(false),
  fNumFitDumps(0)
{
  fMeans.resize(0);
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
    delete fMeans[iPlot];
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
PlotFitParameters::DoFits(Int_t NumXBins, Double_t *XBins,
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
  TProfile *tempProfile;

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
    tempHist = new TH2D(tempName,tempName,NumXBins,XBins,NumYBins,MinY,MaxY);
    tempHist->Sumw2();
    inTree->Draw(inExpr+":"+fInExprX+">>"+tempName,inCut);

    tempProfile = new TProfile(tempName+"prof",tempName+"prof",NumXBins,XBins);
    inTree->Draw(fInExprX+":"+fInExprX+">>"+tempName+"prof",inCut);
    fMeans.push_back(tempProfile);

    TString dumpTitle = fLegendEntries[iPlot] + ";" + inExpr + ";Num Events";
    looseFunc->SetTitle(dumpTitle);
    fitFunc->SetTitle(dumpTitle);
    tempHist->SetTitle(fLegendEntries[iPlot] + ";" + fInExprX + ";" + inExpr + ";Num Events");

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
	  for (Int_t iParam = 0; iParam != tempComponent->GetNpar(); ++iParam)
	    tempComponent->SetParameter(tempComponent->GetParName(iParam),
					fitFunc->GetParameter(tempComponent->GetParName(iParam)));
	  tempComponent->Draw("SAME");
	  components.push_back(tempComponent);
        }
        tempCanvas->SaveAs(dumpName+".png");
	tempCanvas->SaveAs(dumpName+".pdf");
        tempCanvas->SaveAs(dumpName+".C");
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

//--------------------------------------------------------------------
void
PlotFitParameters::DoFits(Int_t NumXBins, Double_t MinX, Double_t MaxX,
			  Int_t NumYBins, Double_t MinY, Double_t MaxY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins,MinX,MaxX,XBins);
  DoFits(NumXBins,XBins,NumYBins,MinY,MaxY);
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
PlotFitParameters::MakeGraphs(TString ParameterExpr)
{
  TGraphErrors *tempGraph;
  std::vector<TGraphErrors*> theGraphs;

  TF1 parameterHolder("parameterHolder",ParameterExpr);

  for (UInt_t iLine = 0; iLine != fFits.size(); ++iLine) {
    tempGraph = new TGraphErrors(fFitXBins);

    for (Int_t iXBin = 0; iXBin != fFitXBins; ++iXBin) {

      for (Int_t iParam = 0; iParam != parameterHolder.GetNpar(); ++iParam)
	parameterHolder.SetParameter(iParam,fFits[iLine][iXBin]->GetParameter(parameterHolder.GetParName(iParam)));

      tempGraph->SetPoint(iXBin,fMeans[iLine]->GetBinContent(iXBin + 1),parameterHolder.Eval(0));
      Double_t error2 = 0;

      for (Int_t iParam = 0; iParam != parameterHolder.GetNpar(); ++iParam) {
	TString replaceExpr = ParameterExpr.ReplaceAll(TString("[") + TString(parameterHolder.GetParName(iParam)).Strip(TString::kLeading,'p') + TString("]"),'x');
	TF1 errorFunc("errorFunc",replaceExpr);
	for (Int_t jParam = 0; jParam != errorFunc.GetNpar(); ++jParam)
	  errorFunc.SetParameter(jParam,parameterHolder.GetParameter(errorFunc.GetParName(jParam)));
	Double_t error = errorFunc.Derivative(parameterHolder.GetParameter(iParam));
	error2 += error*error;
      }

      tempGraph->SetPointError(iXBin,fMeans[iLine]->GetBinError(iXBin + 1),TMath::Sqrt(error2));
    }
    theGraphs.push_back(tempGraph);
  }

  return theGraphs;
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
PlotFitParameters::MakeGraphs(Int_t ParameterNum)
{
  TString ParameterExpr = "";
  ParameterExpr.Form("[%d]",ParameterNum);
  return MakeGraphs(ParameterExpr);
}

//--------------------------------------------------------------------
void
PlotFitParameters::MakeCanvas(std::vector<TGraphErrors*> theGraphs, TString FileBase, TString XLabel, TString YLabel, 
			      Double_t YMin, Double_t YMax, Bool_t logY)
{
  for (UInt_t iGraph = 0; iGraph != theGraphs.size(); ++iGraph)
    theGraphs[iGraph]->GetYaxis()->SetRangeUser(YMin,YMax);
  BaseCanvas(theGraphs,FileBase,XLabel,YLabel,logY);
}

//--------------------------------------------------------------------
void
PlotFitParameters::MakeCanvas(TString ParameterExpr, TString FileBase, TString XLabel, TString YLabel, 
			      Double_t YMin, Double_t YMax, Bool_t logY)
{
  std::vector<TGraphErrors*> theGraphs = MakeGraphs(ParameterExpr);
  MakeCanvas(theGraphs,FileBase,XLabel,YLabel,YMin,YMax,logY);
  for (UInt_t iGraph = 0; iGraph != theGraphs.size(); ++iGraph)
    delete theGraphs[iGraph];
}

//--------------------------------------------------------------------
void
PlotFitParameters::MakeCanvas(Int_t ParameterNum, TString FileBase, TString XLabel, TString YLabel,
			      Double_t YMin, Double_t YMax, Bool_t logY)
{
  TString ParameterExpr = "";
  ParameterExpr.Form("[%d]",ParameterNum);
  MakeCanvas(ParameterExpr,FileBase,XLabel,YLabel,YMin,YMax,logY);
}
