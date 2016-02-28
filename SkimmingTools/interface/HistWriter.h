/**
   @file   HistWriter.h
   Header file for the HistWriter class.
   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_HISTWRITER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_HISTWRITER_H

#include <vector>
#include "TString.h"

/**
   @class HistWriter
   Class used to write a histogram into a .root file from a .txt file. */

class HistWriter
{
 public:
  /// Constructor where the output file and histogram names are set.
  HistWriter( TString fileName, TString histName );
  /// Writes a histogram based on a configuration file.
  void                  MakeHist          ( TString configName );

  /// Default constructor
  HistWriter()                                                   { HistWriter("output.root", "corrections");  }
  virtual ~HistWriter() {}

  /// Sets the output names and reads the configuration file at the same time.
  void                  MakeHist          ( TString fileName, TString histName, TString configName )
                                        { SetFileName(fileName); SetHistName(histName); MakeHist(configName); }

  /// Sets the output .root file name.
  void                  SetFileName       ( TString fileName )   { fFileName = fileName;                      }
  /// Sets the output histogram name.
  void                  SetHistName       ( TString histName )   { fHistName = histName;                      }

 private:
  TString               fFileName;        ///< Output .root file name
  TString               fHistName;        ///< Output histogram name

  ClassDef(HistWriter,1)
};

#endif
