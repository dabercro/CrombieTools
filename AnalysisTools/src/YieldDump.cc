#include <set>
#include <utility>
#include <string>

#include "TString.h"

#include "YieldDump.h"

ClassImp(YieldDump)

//--------------------------------------------------------------------
void
YieldDump::DumpYieldFiles(const char* out_directory, Int_t NumXBins, Double_t *XBins)
{
  std::vector<std::pair<FileType, std::string>> types_files = {
    std::make_pair(kData, "data"),
    std::make_pair(kBackground, "background"),
    std::make_pair(kSignal, "signal")
  };

  for (auto iTypeFile = types_files.begin(); iTypeFile != types_files.end(); ++iTypeFile) {
    std::set<TString> processes = ReturnTreeNames(iTypeFile->first);

    for (auto iCut = fInCuts.begin(); iCut != fInCuts.end(); ++iCut) {
      SetDefaultCut(*iCut);
      for (auto iProcess = processes.begin(); iProcess != processes.end(); ++iProcess) {

        TH1D* proc_hist = GetHist(NumXBins, XBins, iTypeFile->first, *iProcess, kLimitName);

        // Get the contents of each histogram to dump into file

      }
    }
  }
}

//--------------------------------------------------------------------
void
YieldDump::DumpYieldFiles(const char* out_directory, Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  DumpYieldFiles(out_directory, NumXBins, XBins);
}
