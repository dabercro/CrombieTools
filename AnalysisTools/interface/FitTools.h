/**
  @file   FitTools.h
  Definition of FitTools class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H
#define CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H

#include <vector>

#include "TString.h"

#include "RooAddPdf.h"

#include "HistAnalysis.h"
/**
   @ingroup analysisgroup
   @class FitTools
   Class for fitting things for an analysis.
   It has the same spirit as HistAnalysis, but is separate to cut down on RooFit library loading.
   @todo Create a reweighter that floats contributions from different categories
         to match a spectrum in data
*/

using namespace RooFit;

class FitTools : public HistAnalysis
{
 public:
  FitTools();
  virtual ~FitTools();

  /// Get the weights that will reshape the backgrounds different categories to the shape of data in a histogram
  void             FitCategories      ( TString CategoryVar, Int_t NumCategories, TString ShapeLabel = "" );

 private:
  /// Get the properly weighted kernel pdf from a bunch of different background ntuples
  RooAddPdf       *GetJointPdf        ( const char* name, std::vector<TString> files, FileType type = kBackground,
                                        TString CategoryVar = "", Int_t NumCategories = 0 );

  ClassDef(FitTools,1)
};

#endif
