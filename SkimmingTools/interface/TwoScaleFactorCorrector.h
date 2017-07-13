/**
   @file   TwoScaleFactorCorrector.h

   Header file for the TwoScaleFactorCorrector class.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_TWOSCALEFACTORCORRECTOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_TWOSCALEFACTORCORRECTOR_H

#define TSFCORRECTOR_NCORRECTORS 4

#include <vector>
#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "Corrector.h"

/**
   @ingroup skimminggroup
   @class TwoScaleFactorCorrector
   @brief Can be created using the CrombieTools.SkimmingTools.TwoScaleFactorCorrector module.

   A TwoScaleFactorCorrector reads from a tree and histogram and returns a correction factor.
   The CorrectorApplicator facilitates this application by providing the trees
   and writes the result to a branch.

   Unlike the Corrector, there are two different scale factors for two different legs considered.
   Cuts and conditions are given explicitly for when to apply which scale factor to which leg
   by supplying this Corrector with four regular Corrector objects.

   A condition that results in a combinatoric scale factor is also given.
   If this condition is true, for example, if we require two loose leptons,
   with at least one also tight (though both can be tight),
   we use the following scale factor when both leptons are tight:
   \f[
     SF = \frac{SF_{1,tight}^2 SF_{2,loose} + SF_{1,loose} SF_{2,tight}^2}
               {SF_{1,tight} + SF_{2,tight}}
   \f]

*/

class TwoScaleFactorCorrector : public Corrector
{
 public:
  /// Constructor setting the name of a branch it would like to write to.
  TwoScaleFactorCorrector()  {}
  TwoScaleFactorCorrector( TString name,
                           Corrector* Leg1Loose, Corrector* Leg1Tight,
                           Corrector* Leg2Loose, Corrector* Leg2Tight );
  virtual ~TwoScaleFactorCorrector();

  /// Copy this TwoScaleFactorCorrector for parallelization
  TwoScaleFactorCorrector* Copy                         ();

  /// Evaluate the TTree pointer fInTree at its current entry.
  Float_t                  Evaluate                     ();

  /// Set the pointer to the TTree the Corrector objects inside are reading.
  void                     SetInTree                    ( TTree* tree );

 private:
  Corrector* fCorrectors[TSFCORRECTOR_NCORRECTORS];   ///< Pointers to the Correctors

  ClassDef(TwoScaleFactorCorrector,1)
};

#endif
