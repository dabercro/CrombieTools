/**
   @file   FormulaCorrector.h

   Header file for the FormulaCorrector class.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_FORMULACORRECTOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_FORMULACORRECTOR_H

#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "Corrector.h"

/**
   @ingroup skimminggroup
   @class FormulaCorrector
   @brief Can be created using the CrombieTools.SkimmingTools.FormulaCorrector module.

   A Formulacorrector Takes a given formulat and saves a branch with the formula result.

*/

class FormulaCorrector : public Corrector
{
  // Private, only used by Copy
  FormulaCorrector()  {}
 public:
  /// Constructor setting the name of a branch it would like to write to.
  FormulaCorrector( TString name, TString formula );
  virtual ~FormulaCorrector();

  /// Copy this FormulaCorrector for parallelization
  FormulaCorrector* Copy                         ();

  /// Set the pointer to the TTree the Corrector objects inside are reading.
  void                     SetInTree                    ( TTree* tree );

 private:
  /// Evaluate the TTree pointer fInTree at its current entry.
  Float_t                  DoEval                       ();

  TString                  fFormulaString;
  TTreeFormula*            fFormula = NULL;

  ClassDef(FormulaCorrector,1)
};

#endif
