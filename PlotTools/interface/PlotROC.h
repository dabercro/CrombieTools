#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTROC_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTROC_H

#include "TGraph.h"
#include "TTree.h"
#include "TString.h"

#include "PlotHists.h"

class PlotROC : private PlotHists
{
 public:
  PlotROC();
  virtual ~PlotROC()   {}
  
  void                    SetSignalTree        ( TTree *tree )                     { fInTrees[0] = tree;           }
  void                    SetBackgroundTree    ( TTree *tree )                     { fInTrees[1] = tree;           }
  void                    SetSignalCut         ( TString cut )                     { fInCuts[0] = cut;             }
  void                    SetBackgroundCut     ( TString cut )                     { fInCuts[1] = cut;             }
  
  void                    ResetVars            ()                                  { fROCVars.resize(0);           }
  void                    AddVar               ( TString var )                     { fROCVars.push_back(var);      }
  
  TGraph                 *MakeROC              ( TString CutVar, Int_t NumBins = 500 );
  std::vector<TGraph*>    MakeROCs             ( Int_t NumBins = 500 );
  
  void                    MakeCanvas           ( TString FileBase, Int_t NumBins = 500, TString XLabel = "#epsilon_{Signal}", 
                                                 TString YLabel = "#epsilon_{Background}", Bool_t logY = false, Bool_t logX = false );
  
 private:
  
  std::vector<TString>    fROCVars;

  using PlotHists::MakeCanvas;
  
  ClassDef(PlotROC, 1)
};

#endif
