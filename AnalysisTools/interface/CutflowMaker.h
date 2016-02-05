#ifndef CROMBIETOOLS_ANALYSISTOOLS_CUTFLOWMAKER_H
#define CROMBIETOOLS_ANALYSISTOOLS_CUTFLOWMAKER_H

#include <vector>
#include "TTree.h"
#include "TString.h"

class CutflowMaker
{
 public:
  CutflowMaker();
  virtual ~CutflowMaker()  {}
  
  void      AddCut             ( TString name, TString cut )       { fCutNames.push_back(name); fCuts.push_back(cut); }
  void      SetCanvasSize      ( Int_t width, Int_t height )       { fWidth = width; fHeight = height;                }
  void      SetTree            ( TTree* tree )                     { fTree = tree; fYields.resize(0);                 }
  void      PrintCutflow       ( Bool_t OnlyNums = false );
  enum   PlotType  { kAbsolute = 0; kFractional }
  void      MakePlot           ( TString name, PlotType type = kAbsolute );
  void      Reset              ()                          { fCutNames.resize(0); fCuts.resize(0); fYields.resize(0); }

 private:
  Int_t                        fWidth;
  Int_t                        fHeight;

  TTree*                       fTree;
  std::vector<TString>         fCutNames;
  std::vector<TString>         fCuts;
  std::vector<UInt_t>          fYields;
  void      GetCutflow         ();

  ClassDef(CutflowMaker,1)
};

#endif
