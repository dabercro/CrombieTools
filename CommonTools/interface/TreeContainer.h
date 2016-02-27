/**
   @file TreeContainer.h
   Defines the TreeContainer class.
   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_COMMONTOOLS_TREECONTAINER_H
#define CROMBIETOOLS_COMMONTOOLS_TREECONTAINER_H

#include <vector>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

/**
   @class TreeContainer
   Capable of holding many trees and skimming them.
   Probably just a poor reimplementation of TChain, but I have code that depends on this now. */

class TreeContainer
{
 public:
  TreeContainer(TString fileName = "");
  virtual ~TreeContainer();
  
  /// Adds another file to the list of files contained in TreeContainer.
  void                   AddFile             ( TString fileName );
  /// Adds all of the files located in a particular directory.
  void                   AddDirectory        ( TString directoryName, TString searchFor = ".root" );
  /// Sets the name of the tree searched for in the input files.
  void                   SetTreeName         ( TString TreeName )         { fTreeName = TreeName;           }
  /// Add a friend to look for in each file.
  void                   AddFriendName       ( TString name )             { fFriendNames.push_back(name);   }
  /// Return all of the trees contained as a single TTree pointer.
  TTree*                 ReturnTree          ( TString Name = "", TFile *inFile = NULL );
  /// Return all of the trees contained as a vector of TTree pointers.
  std::vector<TTree*>    ReturnTreeList      ( TString Name = "" );
  /// Set the printing level for debugging.
  void                   SetPrinting         ( Bool_t printing )          { fPrinting = printing;           }
  
  /// Use this to keep only certain branches when trees are returned.
  void                   AddKeepBranch       ( TString name )             { fKeepBranches.push_back(name);  }
  /// Use this to set an output file to place a single tree.
  void                   SetOutputFileName   ( TString name )             { fOutputFileName = name;         }
  /// Set a cut to apply to returned trees.
  void                   SetSkimmingCut      ( TString cut )              { fSkimmingCut = cut;             }
  
  /// Return vector of file names read by the TreeContainer.
  std::vector<TString>   ReturnFileNames     ()                           { return fFileNames;              }
  /// Return vector of TFile pointers read by the TreeContainer.
  std::vector<TFile*>    ReturnFileList      ()                           { return fFileList;               }
  
  /// Write a single TTree into a TFile.
  void                   MakeFile            ( TString fileName = "", TString treeName = "");
  
 private:
  
  TTree*                 SkimTree            ( TTree *tree, TFile *inFile );
  
  Bool_t                 fPrinting = false;                       ///< Printer for debugging
  TFile*                 tempFile = NULL;                         ///< Pointer to File
  TTree*                 tempTree = NULL;                         ///< Pointer to Tree
  TTree*                 tempFriend = NULL;
  TString                fTreeName = "events";                    ///< Name of Trees looking for
  std::vector<TString>   fFriendNames;
  std::vector<TFile*>    fFileList;                               ///< List of files
  std::vector<TTree*>    fTreeList;                               ///< List of trees
  TTree*                 fTree = NULL;                            ///< Keep pointer to be deleted at end
  
  std::vector<TString>   fKeepBranches;                           ///< Branches kept in the event of skimming
  TString                fOutputFileName = "output.root";         ///< Potential output file name of skim
  TString                fSkimmingCut = "";                       ///< Cut to return only branches meeting given conditions
  
  std::vector<TString>   fFileNames;                              ///< Used to track names of files where tree list come from
  
  ClassDef(TreeContainer,1)
};

#endif
