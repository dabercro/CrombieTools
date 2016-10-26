//--------------------------------------------------------------------------------------------------
// PlotFitParameters
//
// This class Plots the parameter of some fit as a function of some variable.
// These are defined by the user.
//
// Authors: D.Abercrombie
//--------------------------------------------------------------------------------------------------
#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTFITPARAMETERS_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTFITPARAMETERS_H

#include "TH1.h"

#include "Plot2D.h"

class PlotFitParameters : public Plot2D
{
 public:
  PlotFitParameters();
  virtual ~PlotFitParameters();

  /// Read the fit results to make graphs of parameter expressions
  std::vector<TGraphErrors*>   MakeGraphs            ( TString ParameterExpr );
  /// Read the fit results to make graphs of parameters
  std::vector<TGraphErrors*>   MakeGraphs            ( Int_t ParameterNum );

  /// Various ways to merge the 2D histogram into 1D histograms for fits
  enum CutStyle { kBinned = 0, kLessThan, kGreaterThan };

  /// Set the CutStyle used by PlotFitParameters::DoFit()
  void                         SetCutStyle           ( CutStyle style )                   { fCutStyle = style;                       }

  /// Add a component of the fit function to also plot in any resulting dump
  void                         AddFunctionComponent  ( TString function )                 { fFunctionComponents.push_back(function); }

 private:

  void                       GetMeans                ( Int_t NumXBins, const Double_t *XBins );
  void                       DoFit                   ( TF1* fitFunc, TF1* looseFunc, TH2D* histToFit,
                                                       TF1**& fitHolder, TMatrixDSym**& covHolder );

  void                       ClearFits               ();

  TF1*                       MakeFunction            ( TString function, Double_t /* MinX */, Double_t /* MaxX */,
                                                       Double_t MinY, Double_t MaxY )
                                                                                          { return new TF1("func",function,MinY,MaxY); }
  Int_t                      fFitXBins;               ///< Holds the number of XBins in fFits for cleaning
  std::vector<TH1*>          fMeans;                  ///< Means of x variable bins for setting x values of TGraphs
  std::vector<TString>       fFunctionComponents;

  CutStyle                   fCutStyle = kBinned;     ///< Sets the way that the 2D histograms is treated in PlotFitParameters::DoFit()

  ClassDef(PlotFitParameters, 1)
};

#endif
