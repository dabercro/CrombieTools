#ifndef CROMBIETOOLS_ANALYSISTOOLS_LIMITTREEMAKER_H
#define CROMBIETOOLS_ANALYSISTOOLS_LIMITTREEMAKER_H

#include <map>
#include <set>
#include <vector>
#include "TString.h"

class LimitTreeMaker
{
 public:
  LimitTreeMaker()                                                             { LimitTreeMaker("limittree.root");           }
  LimitTreeMaker( TString outputName );
  virtual ~LimitTreeMaker();

  void      AddFile                  ( TString fileName, TString outTreeName, Float_t XSec = -1 )
                           { fInFileNames.push_back(fileName); fOutTreeNames.push_back(outTreeName); fXSecs.push_back(XSec); }
  void      SetTreeName              ( TString tree )                          { fTreeName = tree;                           }
  void      AddFriendName            ( TString tree )                          { fFriendNames.push_back(tree);               }
  void      AddKeepBranch            ( TString branch )                        { fKeepBranches.push_back(branch);            }
  void      AddWeightBranch          ( TString branch )                        { fWeightBranch.push_back(branch);            }
  void      AddRegion                ( TString regionName, TString regionCut )
                                                     { fRegionNames.push_back(regionName); fRegionCuts.push_back(regionCut); }
  void      SetOutputWeightBranch    ( TString branch )                        { fOutputWeightBranch = branch;               }
  void      SetAllHistName           ( TString name )                          { fAllHistName = name;                        }
  void      SetLuminosity            ( Float_t lumi )                          { fLuminosity = lumi;                         }
  void      MakeTrees                ();

  void      ExceptionSkip            ( TString region, TString outTreeName )   { fExceptionSkip[region].insert(outTreeName); }
  void      ExceptionAdd             ( TString region, TString fileName, TString outTreeName, Float_t XSec = -1 )
                                                                       { fExceptionFileNames[region].push_back(fileName);
                                                                         fExceptionTreeNames[region].push_back(outTreeName);
                                                                         fExceptionXSecs[region].push_back(XSec);            }

  void      AddExceptionDataCut      ( TString region, TString cut )           { fExceptionDataCuts[region] = cut;           }
  void      AddExceptionWeightBranch     ( TString region, TString weight )
                                                                       { fExceptionWeightBranches[region].push_back(weight); }
 private:
  TString                   fOutputFileName;
  std::vector<TString>      fInFileNames;
  std::vector<TString>      fOutTreeNames;
  std::vector<Float_t>      fXSecs;
  TString                   fTreeName;
  std::vector<TString>      fFriendNames;
  std::vector<TString>      fKeepBranches;
  std::vector<TString>      fWeightBranch;
  std::vector<TString>      fRegionNames;
  std::vector<TString>      fRegionCuts;
  TString                   fOutputWeightBranch;
  TString                   fAllHistName;
  Float_t                   fLuminosity;

  std::map<TString,std::set<TString> >    fExceptionSkip;
  std::map<TString,std::vector<TString> > fExceptionFileNames;
  std::map<TString,std::vector<TString> > fExceptionTreeNames;
  std::map<TString,std::vector<Float_t> > fExceptionXSecs;

  std::map<TString,TString>               fExceptionDataCuts;
  std::map<TString,std::vector<TString> > fExceptionWeightBranches;

  ClassDef(LimitTreeMaker,1)
};

#endif
