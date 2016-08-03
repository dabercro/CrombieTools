/**
  @file   HistAnalysis.h
  Definition of HistAnalysis class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/


#ifndef CROMBIETOOLS_ANALYSISTOOLS_HISTANALYSIS_H
#define CROMBIETOOLS_ANALYSISTOOLS_HISTANALYSIS_H

#include <vector>

#include "TString.h"

#include "FileConfigReader.h"
#include "PlotHists.h"

/**
   @ingroup analysisgroup
   @class HistAnalysis
   Class for histogram-based analysis.
   This class handles MCConfigs and data and performs useful tasks
   with the resulting histograms.
   @todo Much of this class is thrown together fast so make it cleaner and integrate with PlotStack.
*/

class HistAnalysis : public FileConfigReader, private PlotHists
{
 public:
  HistAnalysis()           { SetMultiplyLumi(true); }
  virtual ~HistAnalysis()  {}

  /// Sets the signal MC based on the limit tree name and which config to use
  void      SetSignalName      ( TString name, FileType type = kBackground )    { fSignalName = name; fSignalType = type; }

  /// Sets the base cut
  void      SetBaseCut         ( TString cut )                                  { fBaseCut = cut; fDataBaseCut = cut;     }
  /// Sets the base cut for MC and Data
  void      SetBaseCut         ( TString cut, TString datacut )                 { fBaseCut = cut; fDataBaseCut = datacut; }
  /// Sets the MC weight
  void      SetMCWeight        ( TString weight )                               { fMCWeight = weight;                     }

  /// Sets the format string for the scale factor table
  void      SetFormat          ( TString format )                               { fFormat = format;                       }

  /// Different methods of doing cut and count
  enum      ScaleFactorMethod { kCutAndCount = 0 };
  /// Does scale factors between background-subtracted data and signal MC
  Double_t  DoScaleFactors     ( TString PlotVar, Int_t NumBins, Double_t *XBins,
                                 ScaleFactorMethod  method = kCutAndCount, Bool_t NormalizeBackground = true,
                                 TString TreeName = "events" );
  /// Does scale factors between background-subtracted data and signal MC with easier binning
  Double_t  DoScaleFactors     ( TString PlotVar, Int_t NumBins, Double_t MinX, Double_t MaxX,
                                 ScaleFactorMethod  method = kCutAndCount, Bool_t NormalizeBackground = true,
                                 TString TreeName = "events" );

  /// Sets various other cut levels for scalefactors.
  void      AddScaleFactorCut  (  TString name, TString cut, TString datacut = "" );
  /// Resets the values saved for the scale factor cuts
  void      ResetScaleFactorCuts  ();

  /// Reweights based on some expression in MC to match data shape and makes a histogram for CorrectionApplicator to use
  void      MakeReweightHist   ( TString OutFile, TString OutHist, TString PlotVar,
                                 Int_t NumBins, Double_t *XBins, TString TreeName = "events" );

  /// Reweights based on some expression in MC with easier to use binning
  void      MakeReweightHist   ( TString OutFile, TString OutHist, TString PlotVar,
                                 Int_t NumBins, Double_t MinX, Double_t MaxX, TString TreeName = "events" );

  /// Different methods of printing the scale factor tables
  enum      PrintingMethod { kNone = 0, kPresentation, kNote };
  /// Sets whether to print any tables for notes or presentations
  void      SetPrintingMethod  ( PrintingMethod method )                         { fPrintingMethod = method;               }

  /// Sets the amount the background is scaled by
  void      ChangeBackground   ( Double_t factor )                               { fBackgroundChange = factor;             }

 private:

  TString   fSignalName = "";             ///< Legend entry of the signal that we are using from the MCConfig
  FileType  fSignalType = kBackground;    ///< Which MC list to get the signal files from
  TString   fBaseCut = "1";               ///< Sets the cut to use for MC events in the analysis
  TString   fDataBaseCut = "1";           ///< Sets the cut to use for data in the analysis
  TString   fMCWeight = "1";              ///< Weight for the MC histograms
  std::vector<TString> fScaleFactorCuts;  ///< Vector of cuts to do scale factor measurements on
  std::vector<TString> fDataSFCuts;       ///< Vector of cuts on data to do scale factor measurements on
  std::vector<TString> fCutNames;         ///< Vector of cut names to print out table

  TString   fFormat = "%.2f";                        ///< Format string for the output tables
  PrintingMethod fPrintingMethod = kPresentation;    ///< Stores type of method to use for making scale factor tables

  Double_t  fBackgroundChange = 0.0;      ///< Amount the background is scaled by to test scale factor systematic uncertainties

  ClassDef(HistAnalysis,1)
};

#endif
