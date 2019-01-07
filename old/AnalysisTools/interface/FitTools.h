/**
  @file   FitTools.h
  Definition of FitTools class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H
#define CROMBIETOOLS_ANALYSISTOOLS_FITTOOLS_H

#include <vector>

#include "TString.h"

#include "RooWorkspace.h"

#include "HistAnalysis.h"

/**
   @ingroup analysisgroup
   @class FitTools
   Class for fitting things for an analysis.
   It has the same spirit as HistAnalysis, but is separate to cut down on RooFit library loading.
*/

using namespace RooFit;

class FitTools : public HistAnalysis
{
 public:
  FitTools();
  virtual ~FitTools();

  /// Get the weights that will reshape the backgrounds different categories to the shape of data in a histogram.
  void          FitCategories         ( Double_t Shape_Min, Double_t Shape_Max, const char* ShapeLabel = "" );

  /// Add a named category. This should be done in the same order as the categories are enumerated in the tree
  void          AddCategory           ( TString cat )                    { fCategoryNames.push_back(cat); }

  /// Set the branch to use for the category separation
  void          SetCategoryBranch     ( const char* branch )             { fCategoryBranch = branch;      }

 private:
  std::vector<TString> fCategoryNames;                    ///< The names of the various categories
  const char*   fCategoryBranch;                          ///< The name of the branch that separates categories

  ClassDef(FitTools,1)
};

#endif
