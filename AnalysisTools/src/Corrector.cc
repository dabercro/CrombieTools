#include "Corrector.h"

ClassImp(Corrector)

//--------------------------------------------------------------------
Corrector::Corrector(TString name) :
  fName(name),
  fInTree(NULL),
  fCorrectionFile(NULL),
  fCorrectionHist(NULL),
  fNumDims(0),
  fInCut('1'),
  fCutFormula(NULL)
{
  fInExpressions.resize(0);
  fFormulas.resize(0);
}
  
//--------------------------------------------------------------------
Corrector::~Corrector()
{
  if (fCorrectionFile != NULL) {
    if (fCorrectionFile->IsOpen())
      fCorrectionFile->Close();
  }

  if (fCutFormula != NULL)
    delete fCutFormula;
  
  for (UInt_t iFormula = 0; iFormula != fFormulas.size(); ++iFormula)
    delete fFormulas[iFormula];
}

//--------------------------------------------------------------------
void
Corrector::SetCorrectionHist(TString hist1, TString hist2)
{
  fCorrectionHist  = (TH1*) fCorrectionFile->Get(hist1);
  TH1* divisorHist = (TH1*) fCorrectionFile->Get(hist2);
  fCorrectionHist->Divide(divisorHist);
}
  
//--------------------------------------------------------------------
Float_t
Corrector::Evaluate()
{
  if (fInTree == NULL)
    return 1.0;
  else {
    if (fCutFormula->EvalInstance() != 0) {
      if (fNumDims == 1)
        return fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(fFormulas[0]->EvalInstance()));
      else if (fNumDims == 2)
        return fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(fFormulas[0]->EvalInstance(),
                                                       fFormulas[1]->EvalInstance()));
      else if (fNumDims == 3)
        return fCorrectionHist->GetBinContent(fCorrectionHist->FindBin(fFormulas[0]->EvalInstance(),
                                                       fFormulas[1]->EvalInstance(),
                                                       fFormulas[2]->EvalInstance()));
      else
        return 1.0;
    }
    else
      return 1.0;
  }
}

//--------------------------------------------------------------------
void
Corrector::InitializeTree()
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
