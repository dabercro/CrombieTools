/**
  @file   CutflowMaker.h

  Header file for the CutflowMaker class.
  @todo For plots in cutflow: <br>
        Add multiple lines for other files, can watch MC that way <br>
        Adjust to use triggers

  @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_ANALYSISTOOLS_CUTFLOWMAKER_H
#define CROMBIETOOLS_ANALYSISTOOLS_CUTFLOWMAKER_H

#include <vector>
#include "TTree.h"
#include "TString.h"

/**
   @class CutflowMaker
   Makes cutflow plots or tables from given trees.
   @todo make this derived from PlotBase to compare multiple cutflows. */

class CutflowMaker
{
 public:
  CutflowMaker();
  virtual ~CutflowMaker()  {}
  
  /**
     Add a cut to the cutflow.
     @param name is the name that the cut will have in the table or plot
     @param cut is the cutstring applied to the tree left from the previous cut */
  inline void   AddCut             ( TString name, TString cut )       { fCutNames.push_back(name); fCuts.push_back(cut); }
  
  /// Set the size of the canvas for the cutflow plots
  inline void   SetCanvasSize      ( Int_t width, Int_t height )       { fWidth = width; fHeight = height;                }
  inline void   SetTree            ( TTree* tree )                     { fTree = tree; fYields.resize(0);                 }

  void          PrintCutflow       ( Bool_t OnlyNums = false );

  enum   PlotType  { kAbsolute = 0, kFractional };
  void          MakePlot           ( TString name, PlotType type = kAbsolute );
  inline void   Reset              ()                          { fCutNames.resize(0); fCuts.resize(0); fYields.resize(0); }

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
