/**
  @file   PlotROC.h
  Header file for PlotROC class definition.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTROC_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTROC_H

#include "TGraph.h"
#include "TTree.h"
#include "TString.h"
#include "TCut.h"

#include "PlotHists.h"
#include "PlotBase.h"

/**
   @ingroup plotgroup
   @class PlotROC
   @brief Plotter for ROC curves and significance plots.

   The plotter takes signal and background trees and scans cuts over a given variable.
   Multiple variables can be scanned this way at the same time to determine the best discriminators.
*/

class PlotROC : public PlotBase
{
 public:
  PlotROC();
  virtual ~PlotROC();

  /// Makes the canvas with the ROC curves
  void                    MakeCanvas           ( TString FileBase, Int_t NumBins = 500, TString XLabel = "#epsilon_{Signal}",
                                                 TString YLabel = "#epsilon_{Background}", Bool_t logY = false, Bool_t logX = false );

  /// Set the pointer to the signal tree
  void                    SetSignalTree        ( TTree *tree )                     { fSignalTree = tree;           }
  /// Set the pointer to the background tree
  void                    SetBackgroundTree    ( TTree *tree )                     { fBackgroundTree = tree;       }
  /// Set the weight that defines the signal
  void                    SetSignalCut         ( TCut cut )                        { fSignalCut = cut;             }
  /// Set the weight that defines the background
  void                    SetBackgroundCut     ( TCut cut )                        { fBackgroundCut = cut;         }

  /// Reset the list of variables to be plotted
  void                    ResetVars            ()                                  { fROCVars.resize(0);           }
  /// Add a variable to be scanned over and plotted
  void                    AddVar               ( TString var )                     { fROCVars.push_back(var);      }

  /// Types of plots made by PlotROC
  enum  PlotType { kROC = 0, kSignificance };
  /// Set type of plot to be made
  void                    SetPlotType          ( PlotType type )                   { fPlotType = type;             }

 private:

  /// Make a single ROC curve, using a given cut variable
  TGraph*                 MakeROC              ( TString CutVar, Int_t NumBins );
  /// Make all of the ROC curves for the list of variables
  std::vector<TGraph*>    MakeROCs             ( Int_t NumBins );

  TTree*                  fSignalTree = NULL;               ///< Pointer to tree from which to extract signal
  TTree*                  fBackgroundTree = NULL;           ///< Pointer to tree from which to extract background
  TCut                    fSignalCut = "";                  ///< Cut used on signal tree to extract signal
  TCut                    fBackgroundCut = "";              ///< Cut used on background tree to extract background
  std::vector<TString>    fROCVars;                         ///< List of varaibles to make cuts on

  PlotHists               fPlotHists;                       ///< Plotter for histograms

  PlotType                fPlotType = kROC;                 ///< Holds enum for type of plot to be made

  ClassDef(PlotROC, 1)
};

#endif
