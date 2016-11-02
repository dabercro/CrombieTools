/**
  @file   FitTools.h
  Definition of FitTools class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H
#define CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H

#include <vector>

#include "TString.h"

#include "RooRealVar.h"
#include "RooCategory.h"
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
  void             FitCategories      ( TString CategoryVar, Int_t NumCategories,
                                        Double_t Shape_Min, Double_t Shape_Max, TString ShapeLabel = "" );

  /// Add a named category. This should be done in the same order as the categories are enumerated in the tree
  void             AddCategory        ( const char* cat )             { fCategories.push_back(cat);                }

 private:
  RooRealVar  fVariable;         ///< The variable that we are fitting for
  RooCategory fCategory;         ///< The categories that are being floated

  std::vector<const char*> fCategories;

  /// Get the properly weighted kernel pdf from a bunch of different background ntuples
  RooAddPdf       *GetJointPdf        ( const char* name, std::vector<TString> files, FileType type = kBackground,
                                        RooRealVar &var, RooCategory &cat );

  ClassDef(FitTools,1)
};

#endif
