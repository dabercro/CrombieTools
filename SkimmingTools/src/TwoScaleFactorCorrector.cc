/**
  @file   TwoScaleFactorCorrector.cc
  Describes the TwoScaleFactorCorrector class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include "TwoScaleFactorCorrector.h"

ClassImp(TwoScaleFactorCorrector)

//--------------------------------------------------------------------
TwoScaleFactorCorrector::TwoScaleFactorCorrector(TString name,
                                                 Corrector* Leg1Loose, Corrector* Leg1Tight,
                                                 Corrector* Leg2Loose, Corrector* Leg2Tight)
{
  fName = name;
  fCorrectors[0] = Leg1Loose;
  fCorrectors[1] = Leg1Tight;
  fCorrectors[2] = Leg2Loose;
  fCorrectors[3] = Leg2Tight;
}

//--------------------------------------------------------------------
TwoScaleFactorCorrector::~TwoScaleFactorCorrector()
{
  for (int iCorr = 0; iCorr < TSFCORRECTOR_NCORRECTORS; iCorr++) {
    if (fCorrectors[iCorr])
      delete fCorrectors[iCorr];
  }
}

//--------------------------------------------------------------------
void TwoScaleFactorCorrector::SetInTree(TTree* tree)
{
  Corrector::SetInTree(tree);
  for (int iCorr = 0; iCorr < TSFCORRECTOR_NCORRECTORS; iCorr++)
    fCorrectors[iCorr]->SetInTree(tree);
}

//--------------------------------------------------------------------

/**
   @returns value of correction histogram using the expressions added
            through AddInExpression(), unless the event does not pass the cut
            set by SetInCut(). In that case, a default value is returned (1.0).
*/

Float_t TwoScaleFactorCorrector::DoEval()
{
  // Evaluate each tight inner corrector.
  auto leg1_tight = fCorrectors[1]->EvaluateWithFlag();
  auto leg2_tight = fCorrectors[3]->EvaluateWithFlag();

  auto leg1_loose = fCorrectors[0]->Evaluate();
  auto leg2_loose = fCorrectors[2]->Evaluate();

  // If only one is tight, use the other's loose, and multiply
  if ((bool) leg1_tight.first != (bool) leg2_tight.first) {
    if (leg1_tight.first)
      return leg1_tight.second * leg2_loose;

    return leg2_tight.second * leg1_loose;
  }

  // If both tight are non-unity, then do the combination equation
  if (leg1_tight.first)
    return (leg1_tight.second * leg1_tight.second * leg1_loose + leg2_tight.second * leg2_tight.second * leg2_loose) /
      (leg1_tight.second + leg2_tight.second);

  return leg1_loose * leg2_loose;
}

//--------------------------------------------------------------------
TwoScaleFactorCorrector* TwoScaleFactorCorrector::Copy()
{
  TwoScaleFactorCorrector *newTwoScaleFactorCorrector = new TwoScaleFactorCorrector();
  *newTwoScaleFactorCorrector = *this;
  newTwoScaleFactorCorrector->fIsCopy = true;
  for (int iCorr = 0; iCorr < TSFCORRECTOR_NCORRECTORS; iCorr++)
    newTwoScaleFactorCorrector->fCorrectors[iCorr] = fCorrectors[iCorr]->Copy();

  return newTwoScaleFactorCorrector;
}
