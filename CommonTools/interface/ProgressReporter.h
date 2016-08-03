/**
   @file ProgressReporter.h
   Header file that defines ProgressReporter class.
   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_PROGRESSREPORTER_H
#define CROMBIETOOLS_COMMONTOOLS_PROGRESSREPORTER_H

#include <iostream>
#include "TString.h"
#include "TTree.h"

/**
   @ingroup commongroup
   @class ProgressReporter
   This class is used to report progress of various codes that run over every event in a file 
   @todo Make this class load trees for files.
*/

class ProgressReporter
{
 public:
  ProgressReporter() {};
  virtual ~ProgressReporter() {};

  /// Sets the frequency of reporting progress to terminal
  void      SetReportFrequency   ( Long64_t freq )             { fReportFreq = freq;           }

 protected:
  /// Sets the number of entries to process
  void      SetNumberOfEntries   ( Long64_t nentries )         { fNumberOfEntries = nentries;  }
  /// Sets the number of entries to process from a tree
  Long64_t  SetNumberOfEntries   ( TTree *tree )      { fNumberOfEntries = tree->GetEntries();
                                                        return fNumberOfEntries;               }
  /// Sets the name of the file to report to the terminal
  void      SetReportFile        ( TString name )              { fReportFile = name;           }

  /// Sends report to the terminal if needed
  void   ReportProgress   ( Long64_t entry ) {
    if (entry % fReportFreq == 0)
      std::cout << "Processing " << fReportFile << " ... " << (float(entry)/fNumberOfEntries)*100 << "%" << std::endl;
  }

  Long64_t      fNumberOfEntries = 0;    ///< Number of entries being run over

 private:
  Long64_t      fReportFreq = 100000;    ///< Number of events between each frequency report
  TString       fReportFile = "";        ///< Name of the file being run on
};

#endif
