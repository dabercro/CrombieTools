/**
  @file   Corrector.cc
  Describes the Corrector class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <regex>

#include "TAxis.h"
#include "Corrector.h"

ClassImp(Corrector)

//--------------------------------------------------------------------
Corrector::Corrector(TString name) :
  fName(name)
{ }

//--------------------------------------------------------------------
Corrector::~Corrector()
{
  if (fCorrectionFile != NULL && fCorrectionFile->IsOpen() && !fIsCopy)
    fCorrectionFile->Close();

  if (fCutFormula != NULL)
    delete fCutFormula;

  for (UInt_t iFormula = 0; iFormula != fFormulas.size(); ++iFormula)
    delete fFormulas[iFormula];
}

//--------------------------------------------------------------------
void Corrector::SetCorrectionFile(TString fileName)
{
  fCorrectionFile = new TFile(fileName);
  if (fCorrectionFile == NULL) {
    Message(eError, "Could not open %s", fileName.Data());
    exit(1);
  }
}

//--------------------------------------------------------------------
void Corrector::SetCorrectionHist(TString histName)
{
  fCorrectionHist = (TH1*) fCorrectionFile->Get(histName);
  if (fCorrectionHist == NULL) {
    Message(eError, "Could not load %s: Looking inside %s", 
            histName.Data(), fCorrectionFile->GetName());
    exit(1);
  }
  SetMinMax();
}

//--------------------------------------------------------------------
void Corrector::SetCorrectionHist(TString hist1, TString hist2)
{
  fCorrectionHist  = (TH1*) fCorrectionFile->Get(hist1);
  if (fCorrectionHist == NULL) {
    Message(eError, "Could not load %s: Looking inside %s", 
            hist1.Data(), fCorrectionFile->GetName());
    exit(1);
  }

  TH1* divisorHist = (TH1*) fCorrectionFile->Get(hist2);
  if (divisorHist == NULL) {
    Message(eError, "Could not load %s: Looking inside %s", 
            hist2.Data(), fCorrectionFile->GetName());
    exit(1);
  }

  fCorrectionHist->Divide(divisorHist);
  SetMinMax();
}

//--------------------------------------------------------------------
Double_t Corrector::GetFormulaResult(Int_t index)
{
  Double_t eval = fFormulas[index]->EvalInstance();
  if (eval < fMins[index])
    eval = fMins[index];
  else if (eval > fMaxs[index])
    eval = fMaxs[index];
  return eval;
}

//--------------------------------------------------------------------

/**
   @returns value of correction histogram using the expressions added
            through AddInExpression(), unless the event does not pass the cut
            set by SetInCut(). In that case, a default value is returned
            (depending on if it's a scale factor or an uncertainty).
            First part of the pair is a bool determining whether or not that cut was passed
*/

std::pair<bool, Float_t> Corrector::EvaluateWithFlag()
{
  bool flag = false;
  Float_t Output = (fHistReader == eZeroCenteredUnc) ? 0.0 : 1.0;

  if (fMatchedFileName && fInTree != NULL && fCutFormula->EvalInstance() != 0) {
    flag = true;
    if (fNumDims == 1) {
      Double_t evalX = GetFormulaResult(0);
      Output = fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(evalX));
    }
    else if (fNumDims == 2) {
      Double_t evalX = GetFormulaResult(0);
      Double_t evalY = GetFormulaResult(1);
      Output = fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(evalX,evalY));
    }
    else if (fNumDims == 3) {
      Double_t evalX = GetFormulaResult(0);
      Double_t evalY = GetFormulaResult(1);
      Double_t evalZ = GetFormulaResult(2);
      Output = fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(evalX,evalY,evalZ));
    }
  }

  if (fHistReader == eUnityCenteredUnc)
    Output -= 1.0;

  return std::make_pair(flag, Output);

}

//--------------------------------------------------------------------

/**
   @returns value of correction histogram using the expressions added
            through AddInExpression(), unless the event does not pass the cut
            set by SetInCut(). In that case, a default value is returned
            (depending on if it's a scale factor or an uncertainty).
*/

Float_t Corrector::Evaluate()
{

  return EvaluateWithFlag().second;

}

//--------------------------------------------------------------------
void Corrector::InitializeTree()
{
  if (fCutFormula)
    delete fCutFormula;

  fCutFormula = new TTreeFormula(fInCut,fInCut,fInTree);

  if (fFormulas.size() != 0) {
    for (UInt_t iFormula = 0; iFormula != fFormulas.size(); ++iFormula)
      delete fFormulas[iFormula];
    fFormulas.resize(0);
  }

  if (fInExpressions.size() != 0) {
    TTreeFormula* tempFormula;
    for (UInt_t iExpression = 0; iExpression != fInExpressions.size(); ++iExpression) {
      tempFormula = new TTreeFormula(fInExpressions[iExpression],fInExpressions[iExpression],fInTree);
      fFormulas.push_back(tempFormula);
    }
  }
}

//--------------------------------------------------------------------
Bool_t Corrector::CompareFileName(TString fileName) {
  fMatchedFileName = (fMatchFileName == "") or (std::regex_match(fileName.Data(), std::regex(fMatchFileName.Data())));
  return fMatchedFileName;
}

//--------------------------------------------------------------------
void Corrector::SetMinMax()
{
  for (Int_t iDim = 0; iDim != fNumDims; ++iDim) {
    TAxis* theAxis;
    if (iDim == 0)
      theAxis = fCorrectionHist->GetXaxis();
    else if (iDim == 1)
      theAxis = fCorrectionHist->GetYaxis();
    else if (iDim == 2)
      theAxis = fCorrectionHist->GetZaxis();
    else {
      Message(eError, "I don't support this many axes at the moment.");
      exit(3);
    }
    fMins.push_back(theAxis->GetBinCenter(theAxis->GetFirst()));
    fMaxs.push_back(theAxis->GetBinCenter(theAxis->GetLast()));
  }
}

//--------------------------------------------------------------------
Corrector* Corrector::Copy()
{
  Corrector *newCorrector = new Corrector();
  *newCorrector = *this;
  newCorrector->fIsCopy = true;
  return newCorrector;
}
