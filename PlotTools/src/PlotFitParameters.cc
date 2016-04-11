#include <iostream>
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TMath.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TMatrixDSym.h"
#include "TProfile.h"

#include "PlotFitParameters.h"

ClassImp(PlotFitParameters)

//--------------------------------------------------------------------
PlotFitParameters::PlotFitParameters() :
  fFitXBins(0)
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

//--------------------------------------------------------------------
void
PlotFitParameters::GetMeans(Int_t NumXBins, const Double_t *XBins)
{
  fFitXBins = NumXBins;
  UInt_t NumPlots = 0;
  
  if (fInTrees.size() > 0)
    NumPlots = fInTrees.size();
  else if (fInCuts.size() > 0)
    NumPlots = fInCuts.size();
  else
    NumPlots = fInExpr.size();

  TTree *inTree = fDefaultTree;
  TString inCut = fDefaultCut;
  TString inExpr = fDefaultExpr;

  for (UInt_t iPlot = 0; iPlot != NumPlots; ++iPlot) {
    TString tempName;
    tempName.Form("Hist_%d",fPlotCounter);
    fPlotCounter++;

    if (fInTrees.size() != 0)
      inTree = fInTrees[iPlot];
    if (fInCuts.size()  != 0)
      inCut  = fInCuts[iPlot];
    if (fInExprXs.size() != 0)
      fInExprX = fInExprXs[iPlot];

    TH1 *tempProfile;

    if (fCutStyle == kBinned) {
      tempProfile = new TProfile(tempName+"prof",tempName+"prof",NumXBins,XBins);
      inTree->Draw(fInExprX+":"+fInExprX+">>"+tempName+"prof",inCut);
    }
    else {
      tempProfile = new TH1F(tempName+"prof",tempName+"prof",NumXBins,XBins);
      Int_t addBin = (fCutStyle == kLessThan) ? 1 : 0;
      for (Int_t iBin = 0; iBin != NumXBins; ++iBin) {
        tempProfile->SetBinContent(iBin + 1, XBins[iBin + addBin]);
        tempProfile->SetBinError(iBin + 1, 0);
      }
    }

    fMeans.push_back(tempProfile);
  }
}

//--------------------------------------------------------------------
void
PlotFitParameters::DoFit(TF1* fitFunc, TF1* looseFunc, TH2D* histToFit, 
                         TF1**& fitHolder, TMatrixDSym**& covHolder)
{
  Int_t NumXBins = histToFit->GetXaxis()->GetNbins();
  const Double_t *XBins = histToFit->GetXaxis()->GetXbins()->GetArray();

  TString tempName = histToFit->GetName();

  if (fMeans.size() == 0)
    GetMeans(NumXBins, XBins);

  Double_t MinY = histToFit->GetYaxis()->GetBinLowEdge(1);
  Double_t MaxY = histToFit->GetYaxis()->GetBinUpEdge(histToFit->GetYaxis()->GetNbins());

  fitHolder = new TF1*[NumXBins];
  covHolder = new TMatrixDSym*[NumXBins];

  for (Int_t iXBin = 0; iXBin != NumXBins; ++iXBin) {
    TCanvas *tempCanvas = new TCanvas();
    for (UInt_t iParam = 0; iParam != fGuessParams.size(); ++iParam)
      fitFunc->SetParameter(fGuessParams[iParam],fGuesses[iParam]);
    
    Int_t lowerBin;
    Int_t upperBin;
    switch (fCutStyle)
      {
      case kBinned:
        lowerBin = iXBin + 1;
        upperBin = iXBin + 1;
        break;
      case kLessThan:
        lowerBin = 1;
        upperBin = iXBin + 1;
        break;
      case kGreaterThan:
        lowerBin = iXBin + 1;
        upperBin = NumXBins;
        break;
      default:
        std::cout << "What case is that?" << std::endl;
        exit(1);
      }
    TH1D *projection = histToFit->ProjectionY(tempName+"_py",lowerBin,upperBin);

    if (fLooseFunction != "") {
      projection->Fit(looseFunc,fFitOptions,"",MinY,MaxY);
      for (UInt_t iParam = 0; iParam != fParamFrom.size(); ++iParam)
        fitFunc->SetParameter(fParamTo[iParam],looseFunc->GetParameter(fParamFrom[iParam]));
    }
    TFitResultPtr fitResult = projection->Fit(fitFunc,fFitOptions + "S","",MinY,MaxY);
    if (fDumpingFits) {
      TString dumpName;
      Double_t lower = XBins[lowerBin - 1];
      Double_t upper = XBins[upperBin];
      dumpName.Form("DumpFit_%04d_%.2fTo%.2f",fNumFitDumps,lower,upper);
      std::vector<TF1*> components;
      for (UInt_t iFunc = 0; iFunc < fFunctionComponents.size(); iFunc++) {
        TF1 *tempComponent = new TF1(dumpName,fFunctionComponents[iFunc],MinY,MaxY);
        for (Int_t iParam = 0; iParam != tempComponent->GetNpar(); ++iParam)
          tempComponent->SetParameter(tempComponent->GetParName(iParam),
                                      fitFunc->GetParameter(tempComponent->GetParName(iParam)));
        tempComponent->Draw("SAME");
        components.push_back(tempComponent);
      }

      if (bC)
        tempCanvas->SaveAs(dumpName+".C");
      if (bPNG)
        tempCanvas->SaveAs(dumpName+".png");
      if (bPDF)
        tempCanvas->SaveAs(dumpName+".pdf");
      for (UInt_t iComp = 0; iComp < components.size(); iComp++)
        delete components[iComp];
      components.clear();
      fNumFitDumps++;
    }
    fitHolder[iXBin] = (TF1*) fitFunc->Clone();
    covHolder[iXBin] = new TMatrixDSym(fitFunc->GetNpar());
    *covHolder[iXBin] = fitResult->GetCovarianceMatrix();
  }
}

//--------------------------------------------------------------------
std::vector<TGraphErrors*>
PlotFitParameters::MakeGraphs(TString ParameterExpr)
{
  Double_t epsilon = 0.001;

  TGraphErrors *tempGraph;
  std::vector<TGraphErrors*> theGraphs;

  TF1 parameterHolder("parameterHolder",ParameterExpr);

  for (UInt_t iLine = 0; iLine != fFits.size(); ++iLine) {
    tempGraph = new TGraphErrors(fFitXBins);

    for (Int_t iXBin = 0; iXBin != fFitXBins; ++iXBin) {

      // First set the center value of the line being drawn
      for (Int_t iParam = 0; iParam != parameterHolder.GetNpar(); ++iParam) {
        Int_t parNumInFit = fFits[iLine][iXBin]->GetParNumber(parameterHolder.GetParName(iParam));
        parameterHolder.SetParameter(iParam,fFits[iLine][iXBin]->GetParameter(parNumInFit));
        parameterHolder.SetParError(iParam,fFits[iLine][iXBin]->GetParError(parNumInFit));
      }

      tempGraph->SetPoint(iXBin,fMeans[iLine]->GetBinContent(iXBin + 1),parameterHolder.Eval(0));

      // Next set the error
      Double_t error2 = 0;

      for (Int_t iParam = 0; iParam != parameterHolder.GetNpar(); ++iParam) {
        // Just take the derivative for the first order
        ParameterExpr.ReplaceAll(TString("[") + TString(parameterHolder.GetParName(iParam)).Strip(TString::kLeading,'p') + TString("]"), 'x');
        TF1 errorFunc("errorFunc",ParameterExpr);
        for (Int_t jParam = 0; jParam != errorFunc.GetNpar(); ++jParam)
          errorFunc.SetParameter(jParam,parameterHolder.GetParameter(errorFunc.GetParName(jParam)));
        Double_t iDerivative = errorFunc.Derivative(parameterHolder.GetParameter(iParam));
        Double_t error = iDerivative * parameterHolder.GetParError(iParam);
        error2 += error*error;
        ParameterExpr.ReplaceAll('x', TString("[") + TString(parameterHolder.GetParName(iParam)).Strip(TString::kLeading,'p') + TString("]"));

        for (Int_t jParam = iParam + 1; jParam != parameterHolder.GetNpar(); ++jParam) {
          // For second order, take the other derivative
          ParameterExpr.ReplaceAll(TString("[") + TString(parameterHolder.GetParName(jParam)).Strip(TString::kLeading,'p') + TString("]"), 'x');

          TF1 errorFunc2("errorFunc2",ParameterExpr);
          for (Int_t kParam = 0; kParam != errorFunc2.GetNpar(); ++kParam)
            errorFunc2.SetParameter(kParam,parameterHolder.GetParameter(errorFunc.GetParName(kParam)));
          Double_t jDerivative = errorFunc2.Derivative(parameterHolder.GetParameter(jParam));

          // Fetch the value from the covariance matrix
          error2 += 2 * iDerivative * jDerivative *
            (*fCovs[iLine][iXBin])(fFits[iLine][iXBin]->GetParNumber(parameterHolder.GetParName(iParam)),
                                   fFits[iLine][iXBin]->GetParNumber(parameterHolder.GetParName(jParam)));

          ParameterExpr.ReplaceAll('x', TString("[") + TString(parameterHolder.GetParName(jParam)).Strip(TString::kLeading,'p') + TString("]"));
        }
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
