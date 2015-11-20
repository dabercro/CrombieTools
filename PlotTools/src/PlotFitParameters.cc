#include <iostream>
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TMath.h"
#include "TH2D.h"

#include "PlotFitParameters.h"

ClassImp(PlotFitParameters)

//--------------------------------------------------------------------
PlotFitParameters::PlotFitParameters() :
  fFitXBins(0),
  fMeans(0),
  fDumpingFits(false),
  fNumFitDumps(0)
{
  fMeans.resize(0);
  fFunctionComponents.resize(0);
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
  fMeans.resize(0);
  fFits.resize(0);
  fCovs.resize(0);
}

// //// Means should be done separately

//   TString tempName = histToFit->GetName();

//   TProfile *tempProfile = new TProfile(tempName+"prof",tempName+"prof",NumXBins,XBins);
//   inTree->Draw(fInExprX+":"+fInExprX+">>"+tempName+"prof",inCut);
//   fMeans.push_back(tempProfile);


//--------------------------------------------------------------------
void
PlotFitParameters::DoFit(TF1* fitFunc, TF1* looseFunc, TH2D* histToFit, 
                         TF1** fitHolder, TMatrixDSym** covHolder)
{
  Int_t NumXBins = histToFit->GetXaxis()->GetNbins();
  const Double_t *XBins = histToFit->GetXaxis()->GetXbins()->GetArray();

  Int_t NumYBins = histToFit->GetYaxis()->GetNbins();
  const Double_t *YBins = histToFit->GetYaxis()->GetXbins()->GetArray();
  Double_t MinY = YBins[0];
  Double_t MaxY = YBins[NumYBins - 1];

  TString tempName = histToFit->GetName();

  fitHolder = new TF1*[NumXBins];

  for (Int_t iXBin = 0; iXBin != NumXBins; ++iXBin) {
    TCanvas *tempCanvas = new TCanvas();
    for (UInt_t iParam = 0; iParam != fGuessParams.size(); ++iParam)
      fitFunc->SetParameter(fGuessParams[iParam],fGuesses[iParam]);
    
    if (fLooseFunction != "") {
      histToFit->ProjectionY(tempName+"_py",iXBin+1,iXBin+1)->Fit(looseFunc,"MLEQ","",MinY,MaxY);
      for (UInt_t iParam = 0; iParam != fParamFrom.size(); ++iParam)
        fitFunc->SetParameter(fParamTo[iParam],looseFunc->GetParameter(fParamFrom[iParam]));
    }
    TFitResultPtr fitResult = histToFit->ProjectionY(tempName+"_py",iXBin+1,iXBin+1)->Fit(fitFunc,"MLEQ","",MinY,MaxY);
    if (fDumpingFits) {
      TString dumpName;
      Int_t lower = XBins[iXBin];
      Int_t upper = XBins[iXBin + 1];
      dumpName.Form("DumpFit_%04d_%dTo%d",fNumFitDumps,lower,upper);
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
    fitHolder[iXBin] = (TF1*) fitFunc->Clone();
    *covHolder[iXBin] = fitResult->GetCovarianceMatrix();
  }
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

      for (Int_t iParam = 0; iParam != parameterHolder.GetNpar(); ++iParam) {
	Int_t parNumInFit = fFits[iLine][iXBin]->GetParNumber(parameterHolder.GetParName(iParam));
	parameterHolder.SetParameter(iParam,fFits[iLine][iXBin]->GetParameter(parNumInFit));
	parameterHolder.SetParError(iParam,fFits[iLine][iXBin]->GetParError(parNumInFit));
      }

      tempGraph->SetPoint(iXBin,fMeans[iLine]->GetBinContent(iXBin + 1),parameterHolder.Eval(0));
      Double_t error2 = 0;

      for (Int_t iParam = 0; iParam != parameterHolder.GetNpar(); ++iParam) {
	TString replaceExpr = ParameterExpr.ReplaceAll(TString("[") + TString(parameterHolder.GetParName(iParam)).Strip(TString::kLeading,'p') + TString("]"),'x');
	TF1 errorFunc("errorFunc",replaceExpr);
	for (Int_t jParam = 0; jParam != errorFunc.GetNpar(); ++jParam)
	  errorFunc.SetParameter(jParam,parameterHolder.GetParameter(errorFunc.GetParName(jParam)));
	Double_t error = errorFunc.Derivative(parameterHolder.GetParameter(iParam)) * parameterHolder.GetParError(iParam);
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
PlotFitParameters::MakeCanvas(TString FileBase, std::vector<TGraphErrors*> theGraphs, TString XLabel, TString YLabel, 
			      Double_t YMin, Double_t YMax, Bool_t logY)
{
  for (UInt_t iGraph = 0; iGraph != theGraphs.size(); ++iGraph)
    theGraphs[iGraph]->GetYaxis()->SetRangeUser(YMin,YMax);
  BaseCanvas(FileBase,theGraphs,XLabel,YLabel,logY);
}

//--------------------------------------------------------------------
void
PlotFitParameters::MakeCanvas(TString FileBase, TString ParameterExpr, TString XLabel, TString YLabel, 
			      Double_t YMin, Double_t YMax, Bool_t logY)
{
  std::vector<TGraphErrors*> theGraphs = MakeGraphs(ParameterExpr);
  MakeCanvas(FileBase,theGraphs,XLabel,YLabel,YMin,YMax,logY);
  for (UInt_t iGraph = 0; iGraph != theGraphs.size(); ++iGraph)
    delete theGraphs[iGraph];
}

//--------------------------------------------------------------------
void
PlotFitParameters::MakeCanvas(TString FileBase, Int_t ParameterNum, TString XLabel, TString YLabel,
			      Double_t YMin, Double_t YMax, Bool_t logY)
{
  TString ParameterExpr = "";
  ParameterExpr.Form("[%d]",ParameterNum);
  MakeCanvas(FileBase,ParameterExpr,XLabel,YLabel,YMin,YMax,logY);
}
