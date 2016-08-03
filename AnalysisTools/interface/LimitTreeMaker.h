/**
  @file   LimitTreeMaker.h

  Header file for the LimitTreeMaker class.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_LIMITTREEMAKER_H
#define CROMBIETOOLS_ANALYSISTOOLS_LIMITTREEMAKER_H

#include <map>
#include <set>
#include <vector>
#include "TString.h"
#include "InOutDirectoryHolder.h"
#include "FileConfigReader.h"

/**
   @class LimitTreeMaker
   Makes small flat trees for limit tools.
   Reads from a [config file](@ref formatmc) or multiple and creates one file
   with a tree for each control region and each file read in. Only works if you have
   write access to /tmp/$USER since it uses the space to hold trees while copying.
*/

class LimitTreeMaker : public FileConfigReader
{
 public:
  /// Default constructor, making the output file name "`limittree.root`".
  LimitTreeMaker()                                                                        { LimitTreeMaker("limittree.root");           }
  LimitTreeMaker( TString outputName );
  virtual ~LimitTreeMaker();

  /// Reads a config and swaps out files for one control region
  void              ReadExceptionConfig      ( TString config, TString region, TString fileDir = "" );
  /// Makes the file containing all of the limit trees. @todo Tries to read fInDirectory without a file, fixme.
  void              MakeTrees                ();

  /// Set name of limit tree file
  inline    void    SetOutFileName           ( TString file )                             { fOutputFileName = file;                     }
  /// Set the name of the tree read from each input file
  inline    void    SetTreeName              ( TString tree )                             { fTreeName = tree;                           }
  /// Add friends to input tree. @todo I don't think this works.
  inline    void    AddFriendName            ( TString tree )                             { fFriendNames.push_back(tree);               }
  inline    void    AddKeepBranch            ( TString branch, Bool_t isInt = false )     { fKeepBranches.push_back(branch);
                                                                                            fKeepBranchIsInt.push_back(isInt); }
  inline    void    AddWeightBranch          ( TString branch )                           { fWeightBranch.push_back(branch);            }
  inline    void    AddRegion                ( TString regionName, TString regionCut )
                                                                { fRegionNames.push_back(regionName); fRegionCuts.push_back(regionCut); }
  inline    void    SetOutputWeightBranch    ( TString branch )                           { fOutputWeightBranch = branch;               }

  inline    void    ExceptionSkip            ( TString region, TString outTreeName )      { fExceptionSkip[region].insert(outTreeName); }
  inline    void    ExceptionAdd             ( TString region, TString fileName, TString outTreeName, Float_t XSec = -1 )
                                                                                 { fExceptionFileNames[region].push_back(fileName);
                                                                                   fExceptionTreeNames[region].push_back(outTreeName);
                                                                                   fExceptionXSecs[region].push_back(XSec);             }
  inline    void    AddExceptionDataCut      ( TString region, TString cut )              { fExceptionDataCuts[region] = cut;           }
  inline    void    AddExceptionWeightBranch ( TString region, TString weight )  { fExceptionWeightBranches[region].push_back(weight);  }
  /// @todo make all frequency reports in a centralized class
  inline    void    SetReportFrequency       ( UInt_t freq )                              { fReportFrequency = freq;                    }

 private:
  UInt_t                    fReportFrequency;
  TString                   fOutputFileName;
  TString                   fTreeName;
  std::vector<TString>      fFriendNames;
  std::vector<TString>      fKeepBranches;
  std::vector<Bool_t>       fKeepBranchIsInt;
  std::vector<TString>      fWeightBranch;
  std::vector<TString>      fRegionNames;
  std::vector<TString>      fRegionCuts;
  TString                   fOutputWeightBranch;

  std::map<TString,std::set<TString> >    fExceptionSkip;
  std::map<TString,std::vector<TString> > fExceptionFileNames;
  std::map<TString,std::vector<TString> > fExceptionTreeNames;
  std::map<TString,std::vector<Float_t> > fExceptionXSecs;

  std::map<TString,TString>               fExceptionDataCuts;
  std::map<TString,std::vector<TString> > fExceptionWeightBranches;

  ClassDef(LimitTreeMaker,1)
};

#endif
