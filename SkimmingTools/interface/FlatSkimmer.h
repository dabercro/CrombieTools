/**
   @file   FlatSkimmer.h
   Header file for the FlatSkimmer class.
   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H

#include <vector>
#include <set>

#include "TString.h"

#include "InDirectoryHolder.h"
#include "GoodLumiFilter.h"

/**
   @class FlatSkimmer
   Skims events out of a flat tree.
   Takes files from one directory, and writes copies to another directory, 
   taking out events based on a cut string, event filters, a GoodLumiFilter,
   and duplicate events. */

class FlatSkimmer : public InDirectoryHolder
{
 public:
  void         Skim                 ( TString fileName );
  void         AddEventFilter       ( TString filterName );

  FlatSkimmer() {};
  virtual ~FlatSkimmer() {};
  
  /// Set GoodLumiFilter to determine good events
  void         SetGoodLumiFilter    ( GoodLumiFilter *filter )   { fGoodLumiFilter = *filter;    }
  /// Set output directory to write files
  void         SetOutDirectory      ( TString dir )              { fOutDirectory = dir;          }
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
  /// @todo make all frequency reports in a centralized class
  void         SetReportFrequency   ( Int_t freq )               { fReportFreq = freq;           }
  /// Set flag to check for duplicate events
  void         SetCheckDuplicates   ( Bool_t check )             { fCheckDuplicates = check;     }
  /// Add name of TObject to copy from input file to output file unchanged
  void         AddCopyObject        ( TString name )             { fCopyObjects.push_back(name); }
  /// Get the name of the output directory
  TString      GetOutDirectory      () const                     { return fOutDirectory;         }

  /// Wrapper to be used by CrombieTools.Parallelization.RunOnDirectory()
  void         RunOnFile            ( TString name )             { Skim(name);                   }
  
 private:
  std::set<TString>    fEventFilter;             ///< Events to skim out
  GoodLumiFilter       fGoodLumiFilter;          ///< GoodLumiFilter for this FlatSkimmer
  TString              fOutDirectory = ".";      ///< Output directory
  TString              fCut = "1";               ///< Event cut
  TString              fTreeName = "events";     ///< Tree name of flat tree to read
  TString              fRunExpr = "runNum";      ///< Branch for run number
  TString              fLumiExpr = "lumiNum";    ///< Branch for lumi number
  TString              fEventExpr = "eventNum";  ///< Branch for event number
  Int_t                fReportFreq = 100000;
  Int_t                fCheckDuplicates = false; ///< Flag to check duplicates
  std::vector<TString> fCopyObjects;             ///< List of TObject names to also copy from original file
  
  ClassDef(FlatSkimmer,1)
};

#endif
