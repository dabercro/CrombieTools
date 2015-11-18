//--------------------------------------------------------------------------------------------------
// PlotFitParameters
//
// This class Plots the parameter of some fit as a function of some variable.
// These are defined by the user.
//
// Authors: D.Abercrombie
//--------------------------------------------------------------------------------------------------
#ifndef MITPLOTS_PLOT_PLOTFITPARAMETERS_H
#define MITPLOTS_PLOT_PLOTFITPARAMETERS_H

#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TProfile.h"

#include "PlotBase.h"

class PlotFitParameters : public PlotBase
{
 public:
  PlotFitParameters();
  virtual ~PlotFitParameters();
  
  void                         SetExprX                ( TString expr )                                 { fInExprX = expr;             }
  void                         AddExprX                ( TString expr )                                 { fInExprXs.push_back(expr);   }
  void                         SetInitialGuess         ( Int_t param, Double_t guess );
  void                         SetParameterLimits      ( Int_t param, Double_t low, Double_t high );
  void                         SetFunction             ( TString function )                             { fFunctionString = function;  }
  void                         AddFunctionComponent    ( TString function )                 { fFunctionComponents.push_back(function); }
  void                         SetLooseFit             ( TString function )                             { fLooseFunction = function;   }
  void                         SetLooseLimits          ( Int_t param, Double_t low, Double_t high );
  void                         AddMapping              ( Int_t from, Int_t to )                         { fParamFrom.push_back(from); 
                                                                                                          fParamTo.push_back(to);      }
  
  void                         DoFits                  ( Int_t NumXBins, Double_t *XBins,
							 Int_t NumYBins, Double_t MinY, Double_t MaxY );
  
  void                         DoFits                  ( Int_t NumXBins, Double_t MinX, Double_t MaxX,
							 Int_t NumYBins, Double_t MinY, Double_t MaxY );
  
  std::vector<TGraphErrors*>   MakeGraphs              ( TString ParameterExpr );
  std::vector<TGraphErrors*>   MakeGraphs              ( Int_t ParameterNum );

  void                         MakeCanvas              ( TString FileBase, std::vector<TGraphErrors*> theGraphs, 
							 TString XLabel, TString YLabel, Double_t YMin, Double_t YMax,
							 Bool_t logY = false );

  void                         MakeCanvas              ( TString FileBase, TString ParameterExpr,
							 TString XLabel, TString YLabel, Double_t YMin, Double_t YMax,
							 Bool_t logY = false );

  void                         MakeCanvas              ( TString FileBase, Int_t ParameterNum, TString XLabel, TString YLabel,
							 Double_t YMin, Double_t YMax, Bool_t logY = false );
  
  void                         SetDumpingFits          ( Bool_t dump )                                  { fDumpingFits = dump;         }
  
 private:
  
  void                       ClearFits();
  Int_t                      fFitXBins;           // Hold the number of XBins in fFits for cleaning
  std::vector<TProfile*>     fMeans;
  std::vector<TF1**>         fFits;

  std::vector<Int_t>         fGuessParams;
  std::vector<Double_t>      fGuesses;

  std::vector<Int_t>         fParams;             // This is vector used for setting parameter limits for fits
  std::vector<Double_t>      fParamLows;          // Low values of these parameters
  std::vector<Double_t>      fParamHighs;         // High values of these parameters
  std::vector<Int_t>         fLooseParams;        // This is vector used for setting parameter limits for loose fits
  std::vector<Double_t>      fLooseParamLows;     // Low values of these parameters
  std::vector<Double_t>      fLooseParamHighs;    // High values of these parameters
  
  TString                    fInExprX;            // X Expression should be constant
  std::vector<TString>       fInExprXs;
  TString                    fFunctionString;
  std::vector<TString>       fFunctionComponents;
  
  TString                    fLooseFunction;
  std::vector<Int_t>         fParamFrom;
  std::vector<Int_t>         fParamTo;
  
  Bool_t                     fDumpingFits;        // Bool used to dump .png files if you want to check fits
  Int_t                      fNumFitDumps;        // int to keep track of different number of fits
  
  ClassDef(PlotFitParameters,1)
};

#endif
