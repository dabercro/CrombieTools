/**
   @file   PlotBrazil.h
   Header file for the PlotBrazil class.
   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTBRAZIL_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTBRAZIL_H

#include "TString.h"

#include "PlotBase.h"

/**
   @ingroup plotgroup
   @class PlotBrazil
   @brief Can be created using the CrombieTools.PlotTools.PlotBrazil module.

   Class used to create Brazilian plots from a .txt file.
*/

class PlotBrazil : public PlotBase
{
 public:
  /// Default constructor
  PlotBrazil( TString fileName = "" );
  virtual ~PlotBrazil();

  /// Add a point to all of the appropriate TGraphs
  void                  AddPoint          ( Double_t point, Double_t observed, Double_t lowest, Double_t low,
                                            Double_t mid, Double_t high, Double_t highest );

  /// Reads the list of points from a configuration file to set up the TGraphs
  void                  ReadConfig        ( TString filename );

  /// Creates a Brazilian plot using the stored limits.
  void                  MakePlot          ( TString FileBase, TString XLabel, TString YLabel,
                                            Bool_t logY = false, Bool_t logX = false );

 private:
  TGraphErrors          fOneSigma;        ///< The TGraph holding the 1-sigma band
  TGraphErrors          fTwoSigma;        ///< The TGraph holding the 2-sigma band
  TGraphErrors          fExpected;        ///< The TGraph showing the expected limit
  TGraphErrors          fObserved;        ///< The TGraph showing the observed limit

  ClassDef(PlotBrazil, 1)
};

#endif
