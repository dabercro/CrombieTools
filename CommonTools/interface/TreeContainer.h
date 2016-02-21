//--------------------------------------------------------------------------------------------------
// TreeContainer
//
// This class holds all of the trees that are used to make a single plot
//
// Authors: D.Abercrombie
//--------------------------------------------------------------------------------------------------

#ifndef CROMBIETOOLS_COMMONTOOLS_TREECONTAINER_H
#define CROMBIETOOLS_COMMONTOOLS_TREECONTAINER_H

#include <vector>

#include "TString.h"
#include "TFile.h"
#include "TTree.h"

class TreeContainer
{
 public:
  TreeContainer(TString fileName = "");
  virtual ~TreeContainer();
  
  void                   AddFile             ( TString fileName );
  void                   AddDirectory        ( TString directoryName, TString searchFor = ".root" );
  void                   SetTreeName         ( TString TreeName )         { fTreeName = TreeName;           }
  void                   AddFriendName       ( TString name )             { fFriendNames.push_back(name);   }
  TTree*                 ReturnTree          ( TString Name = "", TFile *inFile = NULL );
  std::vector<TTree*>    ReturnTreeList      ( TString Name = "" );
  void                   SetPrinting         ( Bool_t printing )          { fPrinting = printing;           }
  
  void                   AddKeepBranch       ( TString name )             { fKeepBranches.push_back(name);  }
  void                   SetOutputFileName   ( TString name )             { fOutputFileName = name;         }
  void                   SetSkimmingCut      ( TString cut )              { fSkimmingCut = cut;             }
  
  std::vector<TString>   ReturnFileNames     ()                           { return fFileNames;              }
  std::vector<TFile*>    ReturnFileList      ()                           { return fFileList;               }
  
  void                   MakeFile            ( TString fileName = "", TString treeName = "");
  
 private:
  
  TTree*                 SkimTree            ( TTree *tree, TFile *inFile );
  
  Bool_t                 fPrinting;                               // Printer for debugging
  TFile*                 tempFile;                                // Pointer to File
  TTree*                 tempTree;                                // Pointer to Tree
  TTree*                 tempFriend;
  TString                fTreeName;                               // Name of Trees looking for
  std::vector<TString>   fFriendNames;
  std::vector<TFile*>    fFileList;                               // List of files
  std::vector<TTree*>    fTreeList;                               // List of trees
  TTree*                 fTree;                                   // Keep pointer to be deleted at end
  
  std::vector<TString>   fKeepBranches;                           // Branches kept in the event of skimming
  TString                fOutputFileName;                         // Potential output file name of skim
  TString                fSkimmingCut;                            // Cut to return only branches meeting given conditions
  
  std::vector<TString>   fFileNames;                              // Used to track names of files where tree list come from
  
  ClassDef(TreeContainer,1)
};

#endif
