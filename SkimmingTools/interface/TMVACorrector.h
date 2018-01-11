/**
   @file   TMVACorrector.h

   Header file for the TMVACorrector class.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_TMVACORRECTOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_TMVACORRECTOR_H

#include <vector>

#include "TString.h"
#include "TTreeFormula.h"
#include "TMVA/IMethod.h"
#include "TMVA/Reader.h"

#include "Corrector.h"

/**
   @ingroup skimminggroup
   @class TMVACorrector
   @brief Can be created using the CrombieTools.SkimmingTools.TMVACorrector module.

   A TMVAcorrector Takes a given formulat and saves a branch with the formula result.

*/

class TMVACorrector : public Corrector
{
 public:
  TMVACorrector(TString name, TString weights_file);
  ~TMVACorrector();

  /// Read a configuration file consisting of two columns: variable name and formula expression
  void ReadVarConfig (const char* config);

  /// Copy this TMVACorrector for parallelization
  TMVACorrector* Copy ();

 private:
  Float_t  DoEval ();                                 ///< Evaluate the MVA weights at the current entry
  TMVA::Reader* reader {new TMVA::Reader("Silent")};  ///< The reader used by this object
  std::vector<Float_t> fFormulaResults;               ///< Results of the formulae are stored in here for the reader
  std::vector<TString> fVarNames;                     ///< Keep the variable names for copy operations
  TString fWeightsFile;                               ///< Keep this variable for copy operations
  TMVA::IMethod* method {};                           ///< Pointer to method to evaluate

};

#endif
