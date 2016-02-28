/**
   @file CorrectorApplicator.h
   Header that defines the CorrectorApplicator class.
   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_CORRECTORAPPLICATOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_CORRECTORAPPLICATOR_H

#include <vector>
#include "TString.h"
#include "InDirectoryHolder.h"
#include "Corrector.h"

/**
   @class CorrectorApplicator
   Applies multiple correctors to a tree in a file. */

class CorrectorApplicator : public InDirectoryHolder
{
 public:
  /// Apply the corrections to a given file.
  void                 ApplyCorrections     ( TString fileName );

  CorrectorApplicator( TString name, Bool_t saveAll );
  CorrectorApplicator()                                                   { CorrectorApplicator("", true);          }
  virtual ~CorrectorApplicator() {} ;

  /// Set the name of the input tree for each of the Corrector objects to read.
  void                 SetInputTreeName     ( TString tree )              { fInputTreeName = tree;                  }
  /// Set the name of the output tree for the correctors. 
  /// If this is the same as the input tree, the branch must not already exist.
  void                 SetOutputTreeName    ( TString tree )              { fOutputTreeName = tree;                 }
  /// Add a Corrector object to the CorrectorApplicator
  void                 AddCorrector         ( Corrector* corrector )      { fCorrectors.push_back(*corrector);      }
  /// Add a branch name to include by multiplication into the main output branch of the CorrectorApplicator.
  void                 AddFactorToMerge     ( TString factor )            { fMergeFactors.push_back(factor);        }
  /// @todo make all frequency reports in a centralized class
  void                 SetReportFrequency   ( Int_t freq )                { fReportFrequency = freq;                }

  /// Wrapper for CrombieTools.Parallelization.RunOnDirectory() to use.
  void                 RunOnFile            ( TString fileName )          { ApplyCorrections(fileName);             }

 private:
  TString                   fName;                             ///< Name of the master output branch.
  Bool_t                    fSaveAll;                          ///< Bool to save other Corrector results to tree too.
  TString                   fInputTreeName = "events";         ///< Input tree name.
  TString                   fOutputTreeName = "corrections";   ///< Output tree name.
  std::vector<Corrector>    fCorrectors;                       ///< Vector of corrector objects.
  std::vector<TString>      fMergeFactors;                     ///< Vector of branches to merge.
  Int_t                     fReportFrequency = 100000;

  ClassDef(CorrectorApplicator,1)
};

#endif
