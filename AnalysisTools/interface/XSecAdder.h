/**
   @file   XSecAdder.h

   Header file for the XSecAdder class.

   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_XSECADDER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_XSECADDER_H

#include "TString.h"
#include "FileConfigReader.h"

/**
   @ingroup skimminggroup
   @class XSecAdder
   @brief Can be created using the CrombieTools.SkimmingTools.XSecAdder module.

   A XSecAdder reads from a config file and adds a branch for a XSec weight.
   Still needs to be multiplied by Luminosity to get proper number of events. */

class XSecAdder : public FileConfigReader
{
 public:
  /// Constructor setting the name of a branch it would like to write to.
  XSecAdder( TString name = "XSecWeight" );
  virtual ~XSecAdder();

  /// Loops through all the stored files and adds a XSec branch to the right tree
  void    AddXSecs       ();

  /// Set the name of the tree to read to get number of events
  void    SetInTreeName  ( TString name )   { fInTreeName = name;    }
  /// Set the name of the tree to write to
  void    SetOutTreeName ( TString name )   { fOutTreeName = name;   }
  /// Set the name of the branch to write to
  void    SetBranchName  ( TString name )   { fBranchName = name;    }

 private:
  TString fInTreeName = "events";   ///< Name of Tree to read to get number of events
  TString fOutTreeName = "events";  ///< Name of Tree to write to
  TString fBranchName;              ///< Name of Branch to write to

  ClassDef(XSecAdder,1)
};

#endif
