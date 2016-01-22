#include "Corrector.h"

ClassImp(Corrector)

//--------------------------------------------------------------------
Corrector::Corrector( TString name ) :
  fName(name),
  fInTree(NULL),
  fCorrectionFile(NULL),
  fInHist(NULL),
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
Float_t
Corrector::Evaluate()
{
  if (fInTree == NULL)
    return 1.0;
  else {
    if (fCutFormula->EvalInstance() != 0) {
      if (fNumDims == 1)
        return fInHist->GetBinContent(fInHist->FindBin(fFormulas[0]->EvalInstance()));
      else if (fNumDims == 2)
        return fInHist->GetBinContent(fInHist->FindBin(fFormulas[0]->EvalInstance(),
                                                       fFormulas[1]->EvalInstance()));
      else if (fNumDims == 3)
        return fInHist->GetBinContent(fInHist->FindBin(fFormulas[0]->EvalInstance(),
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
  fCutFormula = new TTreeFormula(fInCut,fInCut,fInTree);

  TTreeFormula* tempFormula;
  for (UInt_t iExpression = 0; iExpression != fInExpressions.size(); ++iExpression) {
    tempFormula = new TTreeFormula(fInExpressions[iExpression],fInExpressions[iExpression],fInTree);
    fFormulas.push_back(tempFormula);
  }
}
