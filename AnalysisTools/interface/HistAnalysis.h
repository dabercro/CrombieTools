/**
  @file   HistAnalysis.h
  Definition of HistAnalysis class.
  @author Daniel Abercrombie <dabercro@mit.edu> */


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
   with the resulting histograms. */

class HistAnalysis : public FileConfigReader, private PlotHists
{
 public:
  HistAnalysis()           {}
  virtual ~HistAnalysis()  {}

  /// Sets the signal MC based on the limit tree name and which config to use
  void    SetSignalName      ( TString name, FileType type = kBackground )    { fSignalName = name; fSignalType = type; }

  /// Sets the base cut
  void    SetBaseCut         ( TString cut )                                  { fBaseCut = cut;                         }
  /// Sets the MC weight
  void    SetMCWeight        ( TString weight )                               { fMCWeight = weight;                     }

  /// Sets various other cut levels for scalefactors
  void    AddScaleFactorCut  ( TString cut, TString name )                    { fScaleFactorCuts.push_back(cut);
                                                                                fCutNames.push_back(name);              }
  /// Different methods of doing cut and count
  enum    ScaleFactorMethod { kCutAndCount = 0 };
  /// Does scale factors between background-subtracted data and signal MC
  void    DoScaleFactors     ( TString PlotVar, Int_t NumBins, Double_t *XBins,
                               ScaleFactorMethod  method = kCutAndCount, Bool_t NormalizeBackground = true,
                               TString TreeName = "events" );
  /// Does scale factors between background-subtracted data and signal MC with easier binning
  void    DoScaleFactors     ( TString PlotVar, Int_t NumBins, Double_t MinX, Double_t MaxX,
                               ScaleFactorMethod  method = kCutAndCount, Bool_t NormalizeBackground = true,
                               TString TreeName = "events" );
  
  void    SetFormat          ( TString format )                               { fFormat = format;                       }

 private:

  TString   fSignalName = "";             ///< Legend entry of the signal that we are using from the MCConfig
  FileType  fSignalType = kBackground;    ///< Which MC list to get the signal files from
  TString   fBaseCut = "1";               ///< Sets the cut to use for all events in the analysis
  TString   fMCWeight = "1";              ///< Weight for the MC histograms
  std::vector<TString> fScaleFactorCuts;  ///< Vector of cuts to do scale factor measurements on
  std::vector<TString> fCutNames;         ///< Vector of cut names to print out table

  TString   fFormat = "%.2f";             ///< Format string for the output tables

  ClassDef(HistAnalysis,1)
};

#endif
