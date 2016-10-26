/**
   @file CorrectorApplicator.h
   Header that defines the CorrectorApplicator class.
   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_CORRECTORAPPLICATOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_CORRECTORAPPLICATOR_H

#include <vector>
#include "TString.h"
#include "ProgressReporter.h"
#include "InDirectoryHolder.h"
#include "Corrector.h"

/**
   @ingroup skimminggroup
   @class CorrectorApplicator
   @brief Can be created using the CrombieTools.SkimmingTools.Corrector module.

   Applies multiple Corrector objects to a tree in a file.
*/

class CorrectorApplicator : public InDirectoryHolder, public ProgressReporter
{
 public:
  CorrectorApplicator( TString name = "", Bool_t saveAll = true );
  virtual ~CorrectorApplicator();

  /// Copy this CorrectorApplicator for parallelization
  CorrectorApplicator* Copy                 ();

  /// Apply the corrections to a given file.
  void                 ApplyCorrections     ( TString fileName );

  /// Set the name of the input tree for each of the Corrector objects to read.
  inline void          SetInputTreeName     ( TString tree )              { fInputTreeName = tree;                  }
  /**
     Set the name of the output tree for the correctors.
     If this is the same as the input tree, the branch must not already exist.
  */
  inline void          SetOutputTreeName    ( TString tree )              { fOutputTreeName = tree;                 }
  /// Add a Corrector object to the CorrectorApplicator
  inline void          AddCorrector         ( Corrector* corrector )      { fCorrectors.push_back(corrector);       }
  /// Add a branch name to include by multiplication into the main output branch of the CorrectorApplicator.
  inline void          AddFactorToMerge     ( TString factor )            { fMergeFactors.push_back(factor);        }

  /// Wrapper for CrombieTools.Parallelization.RunOnDirectory() to use.
  inline void          RunOnFile            ( TString fileName )          { ApplyCorrections(fileName);             }

  /// Sets whether the common branch is an uncertainty or a scale factor.
  inline void          SetIsUncertainty     ( Bool_t unc )                { fIsUncertainty = unc;                   }

 private:
  TString                   fName;                             ///< Name of the master output branch.
  Bool_t                    fSaveAll;                          ///< Bool to save other Corrector results to tree too.
  TString                   fInputTreeName = "events";         ///< Input tree name.
  TString                   fOutputTreeName = "corrections";   ///< Output tree name.
  std::vector<Corrector*>   fCorrectors;                       ///< Vector of corrector pointers.
  std::vector<TString>      fMergeFactors;                     ///< Vector of branches to merge.
  Bool_t                    fIsUncertainty = false;            ///< Determines how to merge the branches.

  ClassDef(CorrectorApplicator,1)
};

#endif
