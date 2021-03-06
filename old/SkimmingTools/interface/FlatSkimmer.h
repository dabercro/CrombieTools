/**
   @file   FlatSkimmer.h
   Header file for the FlatSkimmer class.
   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H

#include <vector>
#include <set>

#include "TString.h"

#include "ProgressReporter.h"
#include "InOutDirectoryHolder.h"
#include "GoodLumiFilter.h"

/**
   @ingroup skimminggroup
   @class FlatSkimmer
   @brief Can be created using the CrombieTools.SkimmingTools.FlatSkimmer module.

   Skims events out of a flat tree.
   Takes files from one directory, and writes copies to another directory,
   taking out events based on a cut string, event filters, a GoodLumiFilter,
   and duplicate events.
*/

class FlatSkimmer : public InOutDirectoryHolder, public ProgressReporter
{
 public:
  FlatSkimmer();
  virtual ~FlatSkimmer();

  void         Skim                 ( TString fileName );
  void         AddEventFilter       ( TString filterName );

  /// Copy this FlatSkimmer for parallelization
  FlatSkimmer* Copy                 ();

  /// Set GoodLumiFilter to determine good events
  void         SetGoodLumiFilter    ( GoodLumiFilter *filter )   { fGoodLumiFilter = *filter;    }
  /// Set cut that events must pass
  void         SetCut               ( TString cut )              { fCut = cut;                   }
  /// Set input tree name
  void         SetTreeName          ( TString name )             { fTreeName = name;             }
  /// Set branch name for run number
  void         SetRunExpr           ( TString expr )             { fRunExpr = expr;              }
  /// Set branch name for lumi number
  void         SetLumiExpr          ( TString expr )             { fLumiExpr = expr;             }
  /// Set branch name for event number
  void         SetEventExpr         ( TString expr )             { fEventExpr = expr;            }
  /// Set flag to check for duplicate events
  void         SetCheckDuplicates   ( Bool_t check )             { fCheckDuplicates = check;     }
  /// Add name of TObject to copy from input file to output file unchanged
  void         AddCopyObject        ( TString name )             { fCopyObjects.push_back(name); }

  /// Wrapper to be used by CrombieTools.Parallelization.RunOnDirectory()
  void         RunOnFile            ( TString name )             { Skim(name);                   }

  /// Gives a file that contains names of branches to not activate
  void         SetDisableFile       ( TString name )             { fDisableFile = name;          }

  /// Gives a file that contains names of branches to activate
  void         SetKeepFile          ( TString name )             { fKeepFile = name;             }

 private:
  std::set<TString>    fEventFilter;             ///< Events to skim out
  GoodLumiFilter       fGoodLumiFilter;          ///< GoodLumiFilter for this FlatSkimmer
  TString              fCut = "1";               ///< Event cut
  TString              fTreeName = "events";     ///< Tree name of flat tree to read
  TString              fRunExpr = "runNumber";      ///< Branch for run number
  TString              fLumiExpr = "lumiNumber";    ///< Branch for lumi number
  TString              fEventExpr = "eventNumber";  ///< Branch for event number
  TString              fDisableFile = "";        ///< File to look for branches to disable
  TString              fKeepFile = "";           ///< File to look for branches to keep
  Int_t                fCheckDuplicates = false; ///< Flag to check duplicates
  std::vector<TString> fCopyObjects;             ///< List of TObject names to also copy from original file

  ClassDef(FlatSkimmer,1)
};

#endif
