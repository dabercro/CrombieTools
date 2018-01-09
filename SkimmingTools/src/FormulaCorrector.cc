/**
  @file   FormulaCorrector.cc
  Describes the FormulaCorrector class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include "FormulaCorrector.h"

ClassImp(FormulaCorrector)

//--------------------------------------------------------------------
FormulaCorrector::FormulaCorrector(TString name, TString formula)
: Corrector(name),
  fFormulaString{formula}
{}

//--------------------------------------------------------------------
FormulaCorrector::~FormulaCorrector()
{
  if (fFormula)
    delete fFormula;
}

//--------------------------------------------------------------------
void FormulaCorrector::SetInTree(TTree* tree)
{
  Corrector::SetInTree(tree);
  if (fFormula)
    delete fFormula;

  fFormula = new TTreeFormula(fFormulaString, fFormulaString, fInTree);
}

//--------------------------------------------------------------------

Float_t FormulaCorrector::DoEval()
{
  return fFormula->EvalInstance();
}

//--------------------------------------------------------------------
FormulaCorrector* FormulaCorrector::Copy()
{
  FormulaCorrector *newFormulaCorrector = new FormulaCorrector();
  *newFormulaCorrector = *this;
  newFormulaCorrector->fIsCopy = true;
  return newFormulaCorrector;
}
