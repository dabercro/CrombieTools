/**
  @file   HistAnalysis.h
  Definition of HistAnalysis class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_HISTANALYSIS_H
#define CROMBIETOOLS_ANALYSISTOOLS_HISTANALYSIS_H

#include <vector>

#include "TString.h"
#include "TH1D.h"

#include "FileConfigReader.h"

/**
   @ingroup analysisgroup
   @class HistAnalysis
   Class for histogram-based analysis.
   This class handles MCConfigs and data and performs useful tasks
   with the resulting histograms.
*/

class HistAnalysis : public FileConfigReader
{
 public:
  HistAnalysis()           { SetMultiplyLumi(true); }
  virtual ~HistAnalysis()  {}

  /// Sets the signal MC based on the limit tree name and which config to use
  inline void      SetSignalName      ( TString name, FileType type = kBackground )    { fSignalName = name; fSignalType = type; }

  /// Sets the base cut
  inline void      SetBaseCut         ( TCut cut )                                     { fBaseCut = cut; fDataBaseCut = cut;     }
  /// Sets the base cut for MC and Data
  inline void      SetBaseCut         ( TCut cut, TCut datacut )                       { fBaseCut = cut; fDataBaseCut = datacut; }

  /// Sets the format string for the scale factor table
  inline void      SetFormat          ( TString format )                               { fFormat = format;                       }

  /// Sets the type of way to identify signal and backgrounds
  inline void      SetSearchBy        ( SearchBy search )                              { fSearchBy = search;                     }

  /// Different methods of doing cut and count
  enum      ScaleFactorMethod { 
    kCutAndCount = 0,                     ///< Does scale factor through a simple cut and count with background subtraction
  };
  /// Does scale factors between background-subtracted data and signal MC for each bin
  TH1D* DoScaleFactors     ( TString PlotVar, Int_t NumBins, Double_t *XBins,
                             ScaleFactorMethod  method = kCutAndCount );
  /// Does scale factors between background-subtracted data and signal MC with easier binning
  TH1D* DoScaleFactors     ( TString PlotVar, Int_t NumBins, Double_t MinX, Double_t MaxX,
                             ScaleFactorMethod  method = kCutAndCount );

  /// Sets various other cut levels for scalefactors.
  void      AddScaleFactorCut  (  TString name, TCut cut, TCut datacut = "" );
  /// Resets the values saved for the scale factor cuts
  void      ResetScaleFactorCuts  ();

  /// Reweights based on some expression in MC to match data shape and makes a histogram for CorrectionApplicator to use
  void             MakeReweightHist   ( TString OutFile, TString OutHist, TString PlotVar,
                                        Int_t NumBins, Double_t *XBins );

  /// Reweights based on some expression in MC with easier to use binning
  void             MakeReweightHist   ( TString OutFile, TString OutHist, TString PlotVar,
                                        Int_t NumBins, Double_t MinX, Double_t MaxX );

  /// Different methods of printing the scale factor tables
  enum  PrintingMethod { 
    kNone = 0,                            ///< Does not print output from hist analysis
    kPresentation,                        ///< Prints output narrow enough for a presentation
    kNote,                                ///< Prints output detailed enough for an analysis note
  };
  /// Sets whether to print any tables for notes or presentations
  inline void      SetPrintingMethod  ( PrintingMethod method )                         { fPrintingMethod = method;               }

  /// Sets the amount the background is scaled by
  inline void      ChangeBackground   ( Double_t factor )                               { fBackgroundChange = factor;             }

  /// Sets whether or not to normalize backgrounds before comparing to data
  inline void      SetNormalized      ( Bool_t norm )                                   { fNormalized = norm;                     }

  /// Sets the base cut with character array
  inline void      SetBaseCut         ( const char* cut )                              { SetBaseCut(TCut(cut));                  }
  /// Sets the base cut for MC and Data with character array
  inline void      SetBaseCut         ( const char* cut, const char* datacut )         { SetBaseCut(TCut(cut), TCut(datacut));   }

  /// Sets various other cut levels for scalefactors.
  void      AddScaleFactorCut  (  TString name, const char* cut, const char* datacut = 0 )
                                                                            { AddScaleFactorCut(name, TCut(cut), TCut(datacut)); }

 private:
  TString          fSignalName = "";             ///< Legend entry of the signal that we are using from the MCConfig
  FileType         fSignalType = kBackground;    ///< Which MC list to get the signal files from
  TCut             fBaseCut = "1";               ///< Sets the cut to use for MC events in the analysis
  TCut             fDataBaseCut = "1";           ///< Sets the cut to use for data in the analysis
  std::vector<TCut> fScaleFactorCuts;            ///< Vector of cuts to do scale factor measurements on
  std::vector<TCut> fDataSFCuts;                 ///< Vector of cuts on data to do scale factor measurements on
  std::vector<TString> fCutNames;                ///< Vector of cut names to print out table

  SearchBy         fSearchBy = kLimitName;       ///< Defines the string that is compared when identifying signal

  TString          fFormat = "%.2f";                   ///< Format string for the output tables
  PrintingMethod   fPrintingMethod = kPresentation;    ///< Stores type of method to use for making scale factor tables

  Double_t         fBackgroundChange = 0.0;      ///< Amount the background is scaled by to test scale factor systematic uncertainties
  Bool_t           fNormalized = true;           ///< Determines whether or not to normalize backgrounds before comparing to data

  ClassDef(HistAnalysis,1)
};

#endif
