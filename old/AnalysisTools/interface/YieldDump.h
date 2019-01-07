/**
  @file   YieldDump.h

  Header file for the YieldDump class.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_YIELDDUMP_H
#define CROMBIETOOLS_ANALYSISTOOLS_YIELDDUMP_H

#include <sqlite3.h>

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
  void  AddRegion       (const char* region, const char* cut, const char* data_weight = "", const char* mc_weight = "")
                            { fRegionCuts.push_back(TCut(region, cut));
                              fDataWeights.push_back(TCut(data_weight));
                              fMCWeights.push_back(TCut(mc_weight));     }

  /// Clear out the regions stored
  void  ResetRegions    ()  { fRegionCuts.clear(); fDataWeights.clear(); fMCWeights.clear(); }

  /// Dump the yields into csv files in an output directory
  void  DumpYieldFiles  (const char* out_file, Int_t NumXBins, Double_t *XBins);

  /// Dump the yields into csv files in an output directory
  void  DumpYieldFiles  (const char* out_file, Int_t NumXBins, Double_t MinX, Double_t MaxX);

 private:
  std::vector<TCut>     fRegionCuts;   ///< A separate vector of cuts for different regions
  std::vector<TCut>     fDataWeights;  ///< The data weights for each region
  std::vector<TCut>     fMCWeights;    ///< The MC weights for each region

  void  SimpleExecute   (sqlite3* conn, const char* query);  ///< Wrapper that makes and executes statement

  ClassDef(YieldDump,1)
};

#endif
