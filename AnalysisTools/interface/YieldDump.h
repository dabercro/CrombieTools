/**
  @file   YieldDump.h

  Header file for the YieldDump class.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_YIELDDUMP_H
#define CROMBIETOOLS_ANALYSISTOOLS_YIELDDUMP_H

#include "TCut.h"

#include "FileConfigReader.h"

/**
   @class YieldDump
   Dumps yields for analysis in R.
*/

class YieldDump : public FileConfigReader
{
 public:
  YieldDump()           {}
  virtual ~YieldDump()  {}

  /// Add a region and selection cut to the list of regions to plot
  void  AddRegion       (const char* region, const char* cut)  { AddWeight(TCut(region, cut)); }

  /// Dump the yields into csv files in an output directory
  void  DumpYieldFiles  (const char* out_directory, Int_t NumXBins, Double_t *XBins);

  /// Dump the yields into csv files in an output directory
  void  DumpYieldFiles  (const char* out_directory, Int_t NumXBins, Double_t MinX, Double_t MaxX);

  ClassDef(YieldDump,1)
};

#endif
