#ifndef CROMBIETOOLS_SKIMMINGTOOLS_CUTFLOWMAKER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_CUTFLOWMAKER_H

#include <vector>
#include "TTree.h"
#include "TString.h"

class CutflowMaker
{
 public:
  CutflowMaker();
  virtual ~CutflowMaker()  {}
  
  void      AddCut             ( TString name, TString cut )       { fCutNames.push_back(name); fCuts.push_back(cut); }
  void      PrintCutflow       ( TTree* tree = 0, Bool_t OnlyNums = false );

 private:
  std::vector<TString>         fCutNames;
  std::vector<TString>         fCuts;

  ClassDef(CutflowMaker,1)
};

#endif
