/**
  @file   Corrector.cc
  Describes the Corrector class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <iostream>
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
    std::cout << "Could not open " << fileName << std::endl;
    exit(1);
  }
}

//--------------------------------------------------------------------
void Corrector::SetCorrectionHist(TString histName)
{
  fCorrectionHist = (TH1*) fCorrectionFile->Get(histName);
  if (fCorrectionHist == NULL) {
    std::cout << "Could not load " << histName << std::endl;
    std::cout << "Looking inside " << fCorrectionFile->GetName() << std::endl;
    exit(1);
  }
  SetMinMax();
}

//--------------------------------------------------------------------
void Corrector::SetCorrectionHist(TString hist1, TString hist2)
{
  fCorrectionHist  = (TH1*) fCorrectionFile->Get(hist1);
  if (fCorrectionHist == NULL) {
    std::cout << "Could not load " << hist1 << std::endl;
    std::cout << "Looking inside " << fCorrectionFile->GetName() << std::endl;
    exit(1);
  }

  TH1* divisorHist = (TH1*) fCorrectionFile->Get(hist2);
  if (divisorHist == NULL) {
    std::cout << "Could not load " << hist2 << std::endl;
    std::cout << "Looking inside " << fCorrectionFile->GetName() << std::endl;
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
            set by SetInCut(). In that case, a value of 1 is returned.
*/

Float_t Corrector::Evaluate()
{
  if (fInTree == NULL)
    return 1.0;
  else {
    if (fCutFormula->EvalInstance() != 0) {
      if (fNumDims == 1) {
        Double_t evalX = GetFormulaResult(0);
        return fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(evalX));
      }
      else if (fNumDims == 2) {
        Double_t evalX = GetFormulaResult(0);
        Double_t evalY = GetFormulaResult(1);
        return fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(evalX,evalY));
      }
      else if (fNumDims == 3) {
        Double_t evalX = GetFormulaResult(0);
        Double_t evalY = GetFormulaResult(1);
        Double_t evalZ = GetFormulaResult(2);
        return fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(evalX,evalY,evalZ));
      }
      else
        return 1.0;
    }
    else
      return 1.0;
  }
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

  TTreeFormula* tempFormula;
  for (UInt_t iExpression = 0; iExpression != fInExpressions.size(); ++iExpression) {
    tempFormula = new TTreeFormula(fInExpressions[iExpression],fInExpressions[iExpression],fInTree);
    fFormulas.push_back(tempFormula);
  }
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
      std::cout << "I don't support this many axes at the moment." << std::endl;
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
