/**
   @file   XSecAdder.h

   Header file for the XSecAdder class.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_XSECADDER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_XSECADDER_H

#include <vector>

#include "TString.h"
#include "ParallelRunner.h"

/**
   @ingroup analysisgroup
   @class XSecAdder
   @brief Can be created using the CrombieTools.AnalysisTools.XSecAdder module.

   A XSecAdder reads from a config file and adds a branch for a XSec weight.
   Still needs to be multiplied by Luminosity to get proper number of events.
*/

class XSecAdder : public ParallelRunner
{
 public:
  /// Constructor setting the name of a branch it would like to write to.
  XSecAdder( TString name = "XSecWeight" );
  virtual ~XSecAdder();

  /// Loops through all the stored files and adds a XSec branch to the right tree
  void    AddXSecs       ();

  /// Set the name of the tree to read to get number of events
  void    SetInTreeName  ( TString name )   { fInTreeName = name;             }
  /// Set the name of the tree to write to
  void    SetOutTreeName ( TString name )   { fOutTreeName = name;            }
  /// Set the name of the branch to write to
  void    SetBranchName  ( TString name )   { fBranchName = name;             }
  /// Add a branch to merge into the new cross section branch
  void    AddMergeBranch ( TString name )   { fMergeBranches.push_back(name); }

 private:
  TString fInTreeName = "events";       ///< Name of Tree to read to get number of events
  TString fOutTreeName = "events";      ///< Name of Tree to write to
  TString fBranchName;                  ///< Name of Branch to write to

  std::vector<TString> fMergeBranches;  ///< List of branches to merge into the new branch

  void RunFile(FileInfo& info);         ///< Run over a single file

  ClassDef(XSecAdder,1)
};

#endif
