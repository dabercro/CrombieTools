//--------------------------------------------------------------------------------------------------
// Plot2D
//
// Authors: D.Abercrombie
//--------------------------------------------------------------------------------------------------
#ifndef CROMBIETOOLS_PLOTTOOLS_PLOT2D_H
#define CROMBIETOOLS_PLOTTOOLS_PLOT2D_H

#include "TMatrixDSym.h"
#include "TGraphErrors.h"
#include "TF2.h"
#include "TF1.h"

#include "FileConfigReader.h"

class Plot2D : public FileConfigReader
{
 public:
  Plot2D();
  virtual ~Plot2D();

  void                         SetExprX                ( TString expr )                             { fInExprX = expr;                    }
  void                         AddExprX                ( TString expr )                             { fInExprXs.push_back(expr);          }
  void                         ResetInitialGuess       ()                                           { fGuessParams.resize(0);
                                                                                                      fGuesses.resize(0);                 }
  void                         SetInitialGuess         ( Int_t param, Double_t guess )              { fGuessParams.push_back(param);
                                                                                                      fGuesses.push_back(guess);          }

  void                         SetLooseGuess           ( Int_t param, Double_t guess )              { fLooseGuessParams.push_back(param);
                                                                                                      fLooseGuesses.push_back(guess);     }
  void                         ResetParameterLimits    ();
  void                         SetParameterLimits      ( Int_t param, Double_t low, Double_t high );
  void                         SetFunction             ( TString function )                         { fFunctionString = function;         }
  void                         SetLooseFit             ( TString function )                         { fLooseFunction = function;          }
  void                         SetLooseLimits          ( Int_t param, Double_t low, Double_t high );
  void                         AddMapping              ( Int_t from, Int_t to, Double_t fact = 1.0 );

  virtual  std::vector<TGraphErrors*>  MakeGraphs      ( TString ParameterExpr );

  void                         DoFits                  ( Int_t NumXBins, Double_t *XBins,
							 Int_t NumYBins, Double_t MinY, Double_t MaxY );

  void                         DoFits                  ( Int_t NumXBins, Double_t MinX, Double_t MaxX,
							 Int_t NumYBins, Double_t MinY, Double_t MaxY );

  void                         MakeCanvas              ( TString FileBase, std::vector<TGraphErrors*> theGraphs,
                                                         TString XLabel, TString YLabel, Double_t YMin, Double_t YMax,
                                                         Bool_t logY = false );

  void                         MakeCanvas              ( TString FileBase, TString ParameterExpr, TString XLabel, TString YLabel,
                                                         Double_t YMin, Double_t YMax, Bool_t logY = false );

  void                         MakeCanvas              ( TString FileBase, Int_t ParameterNum, TString XLabel, TString YLabel,
                                                         Double_t YMin, Double_t YMax, Bool_t logY = false );

  void                         SetDumpingFits          ( Bool_t dump )                               { fDumpingFits = dump;               }
  void                         SetNumPoints            ( Int_t num )                                 { fNumPoints = num;                  }
  void                         SetFitOptions           ( TString opts )                              { fFitOptions = opts;                }

 protected:

  std::vector<TF1*>          MakeFuncs               ( TString ParameterExpr, Double_t MinX, Double_t MaxX );

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
  std::vector<Int_t>         fLooseGuessParams;
  std::vector<Double_t>      fLooseGuesses;

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
  std::vector<Double_t>      fParamFactor;

  Bool_t                     fDumpingFits;           // Bool used to dump .png files if you want to check fits
  Int_t                      fNumFitDumps;           // int to keep track of different number of fits
  TString                    fFitOptions;

 private:

  Int_t                      fNumPoints;

  ClassDef(Plot2D, 1)
};

#endif
