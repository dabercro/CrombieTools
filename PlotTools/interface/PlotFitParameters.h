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

#include "TProfile.h"

#include "Plot2D.h"

class PlotFitParameters : public Plot2D
{
 public:
  PlotFitParameters();
  virtual ~PlotFitParameters();
  
  void                         AddFunctionComponent  ( TString function )                 { fFunctionComponents.push_back(function); }
  
  std::vector<TGraphErrors*>   MakeGraphs            ( TString ParameterExpr );
  std::vector<TGraphErrors*>   MakeGraphs            ( Int_t ParameterNum );

  //// 'using' doesn't seem to work for some reason
  void                         MakeCanvas            ( TString FileBase, std::vector<TGraphErrors*> theGraphs,
                                                       TString XLabel, TString YLabel, Double_t YMin, Double_t YMax, Bool_t logY = false )
                                                                { Plot2D::MakeCanvas(FileBase,theGraphs,XLabel,YLabel,YMin,YMax,logY); }
  void                         MakeCanvas            ( TString FileBase, TString ParameterExpr, TString XLabel, TString YLabel, 
                                                       Double_t YMin, Double_t YMax, Bool_t logY = false )
                                                            { Plot2D::MakeCanvas(FileBase,ParameterExpr,XLabel,YLabel,YMin,YMax,logY); }
  ////

  void                         MakeCanvas            ( TString FileBase, Int_t ParameterNum, TString XLabel, TString YLabel,
							 Double_t YMin, Double_t YMax, Bool_t logY = false );

 private:
  
  void                       GetMeans                ( Int_t NumXBins, const Double_t *XBins );
  void                       DoFit                   ( TF1* fitFunc, TF1* looseFunc, TH2D* histToFit, 
                                                       TF1**& fitHolder, TMatrixDSym**& covHolder );

  TF1*                       MakeFunction            ( TString function, Double_t /* MinX */, Double_t /* MaxX */, 
                                                       Double_t MinY, Double_t MaxY )
                                                                                          { return new TF1("func",function,MinY,MaxY); }
  void                       ClearFits               ();

  Int_t                      fFitXBins;           // Hold the number of XBins in fFits for cleaning
  std::vector<TProfile*>     fMeans;

  std::vector<TString>       fFunctionComponents;
  
  ClassDef(PlotFitParameters,1)
};

#endif
