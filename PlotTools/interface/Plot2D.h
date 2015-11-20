//--------------------------------------------------------------------------------------------------
// Plot2D
//
// Authors: D.Abercrombie
//--------------------------------------------------------------------------------------------------
#ifndef CROMBIETOOLS_PLOTTOOLS_PLOT2D_H
#define CROMBIETOOLS_PLOTTOOLS_PLOT2D_H

#include "TMatrixDSym.h"
#include "TF2.h"
#include "TF1.h"

#include "PlotBase.h"

class Plot2D : public PlotBase
{
 public:
  Plot2D();
  virtual ~Plot2D();
  
  void                         SetExprX                ( TString expr )                                 { fInExprX = expr;             }
  void                         AddExprX                ( TString expr )                                 { fInExprXs.push_back(expr);   }
  void                         SetInitialGuess         ( Int_t param, Double_t guess );
  void                         SetParameterLimits      ( Int_t param, Double_t low, Double_t high );
  void                         SetFunction             ( TString function )                             { fFunctionString = function;  }
  void                         SetLooseFit             ( TString function )                             { fLooseFunction = function;   }
  void                         SetLooseLimits          ( Int_t param, Double_t low, Double_t high );
  void                         AddMapping              ( Int_t from, Int_t to )                         { fParamFrom.push_back(from); 
                                                                                                          fParamTo.push_back(to);      }
  
  void                         DoFits                  ( Int_t NumXBins, Double_t *XBins,
							 Int_t NumYBins, Double_t MinY, Double_t MaxY );
  
  void                         DoFits                  ( Int_t NumXBins, Double_t MinX, Double_t MaxX,
							 Int_t NumYBins, Double_t MinY, Double_t MaxY );
  
/*   virtual    void              MakeCanvas              ( TString FileBase, TString ParameterExpr, */
/* 							 TString XLabel, TString YLabel, Double_t YMin, Double_t YMax, */
/* 							 Bool_t logY = false ); */

 protected:
  
  void                       MapTo                     ( TF1* fitFunc, TF1* looseFunc );
  virtual    void            DoFit                     ( TF1* fitFunc, TF1* looseFunc, TH2D* histToFit, 
                                                         TF1**& fitHolder, TMatrixDSym**& covHolder );

  virtual    TF1*            MakeFunction              ( TString function, Double_t MinX, Double_t MaxX, 
                                                         Double_t MinY, Double_t MaxY )
                                                                                { return new TF2("func",function,MinX,MaxX,MinY,MaxY); }

  virtual    void            ClearFits                 ();

  std::vector<TF1**>         fFits;
  std::vector<TMatrixDSym**> fCovs;

  std::vector<Int_t>         fGuessParams;
  std::vector<Double_t>      fGuesses;

  std::vector<Int_t>         fParams;                // This is vector used for setting parameter limits for fits
  std::vector<Double_t>      fParamLows;             // Low values of these parameters
  std::vector<Double_t>      fParamHighs;            // High values of these parameters
  std::vector<Int_t>         fLooseParams;           // This is vector used for setting parameter limits for loose fits
  std::vector<Double_t>      fLooseParamLows;        // Low values of these parameters
  std::vector<Double_t>      fLooseParamHighs;       // High values of these parameters
  
  TString                    fInExprX;               // X Expression should be constant
  std::vector<TString>       fInExprXs;
  TString                    fFunctionString;
  
  TString                    fLooseFunction;
  std::vector<Int_t>         fParamFrom;
  std::vector<Int_t>         fParamTo;
  
  ClassDef(Plot2D,1)
};

#endif