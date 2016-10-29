/**
  @file   FitTools.h
  Definition of FitTools class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H
#define CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H

#include "TString.h"

#include "HistAnalysis.h"

/**
   @ingroup analysisgroup
   @class FitTools
   Class for fitting things for an analysis.
   It has the same spirit as HistAnalysis, but is separate to cut down on RooFit library loading.
   @todo Create a reweighter that floats contributions from different categories
         to match a spectrum in data
*/

class FitTools : public HistAnalysis
{
 public:
  FitTools();
  virtual ~FitTools();

  /// Get the weights that will reshape the backgrounds different categories to the shape of data in a histogram
  void             FitCategories      ( TString CategoryVar, Int_t NumCategories,
                                        TString ShapeVar, TString ShapeLabel = "" );

 private:

  ClassDef(FitTools,1)
};

#endif
