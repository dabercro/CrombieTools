/**
  @file   CutflowMaker.h

  Header file for the CutflowMaker class.
  @todo For plots in cutflow: <br>
        Add multiple lines for other files, can watch MC that way <br>
        Adjust to use triggers

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_CUTFLOWMAKER_H
#define CROMBIETOOLS_ANALYSISTOOLS_CUTFLOWMAKER_H

#include <vector>
#include "TTree.h"
#include "TString.h"

#include "PlotBase.h"

/**
   @class CutflowMaker
   Makes cutflow plots or tables from given trees.
   @todo make this derived from PlotBase to compare multiple cutflows.
*/

class CutflowMaker : public PlotBase
{
 public:
  CutflowMaker()           {}
  virtual ~CutflowMaker()  {}
  
  /**
     Add a cut to the cutflow.
     @param name is the name that the cut will have in the table or plot
     @param cut is the cutstring applied to the tree left from the previous cut
  */
  inline void   AddCut             ( TString name, TString cut )       { fCutNames.push_back(name); fCuts.push_back(cut); }
  
  /// Types of cutflow tables to print
  enum TableType {
    kNormal = 0,                                    ///< Just displays the cut name and result in two columns
    kNumbers,                                       ///< Shows only numbers for copying into people's Excel sheets
    kLatex,                                         ///< Prints a table that can be copied into LaTeX
  };
  /// Prints the cutflow for a single line given
  void          PrintCutflow       ( TableType table = kNormal );

  /// Type of plot to show from the cutflow
  enum   PlotType  {
    kAbsolute = 0,                                  ///< Counts the number of events for the cutflow
    kFractional,                                    ///< Shows the fraction of events since the last cut
  };
  /// Makes cutflow plots. Multiple lines can be used here.
  void          MakePlot           ( TString name, PlotType type = kAbsolute );
  /// Resets the cutflow cuts
  inline void   Reset              ()                          { fCutNames.resize(0); fCuts.resize(0); fYields.resize(0); }

 private:
  std::vector<TString>         fCutNames;           ///< Names of the cuts to show up in legends or tables
  std::vector<TString>         fCuts;               ///< Cut strings
  std::vector<UInt_t>          fYields;             ///< Holds yields
  void      GetCutflow         ( UInt_t index );    ///< Get the cutflow for a given line index

  ClassDef(CutflowMaker,1)
};

#endif
