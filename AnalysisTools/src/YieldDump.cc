#include <set>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>

#include "TString.h"

#include "YieldDump.h"

ClassImp(YieldDump)

//--------------------------------------------------------------------
void
YieldDump::DumpYieldFiles(std::string out_directory, Int_t NumXBins, Double_t *XBins)
{
  std::vector<std::pair<FileType, std::string>> types_files = {
    std::make_pair(kData, "data"),
    std::make_pair(kBackground, "background"),
    std::make_pair(kSignal, "signal")
  };

  assert(out_directory != "");

  std::string delim = ",";

  for (auto iTypeFile = types_files.begin(); iTypeFile != types_files.end(); ++iTypeFile) {
    auto processes = ReturnTreeNames(iTypeFile->first);
    std::stringstream out_string;

    // Make the first row of the datacard
    for (int iBin = 1; iBin < NumXBins + 1; iBin++) {
      if (out_string.str() != "")
        out_string << delim;
      out_string << "bin_" << iBin << delim << "bin_" << iBin << ".stat_unc";
    }
    out_string << "\n";

    for (auto iCut = fRegionCuts.begin(); iCut != fRegionCuts.end(); ++iCut) {
      SetDefaultCut(*iCut);
      for (auto iProcess = processes.begin(); iProcess != processes.end(); ++iProcess) {

        Message(eInfo, "Getting histograms from %s for %s region.", iProcess->Data(), iCut->GetName());
        auto proc_hist = GetHist(NumXBins, XBins, iTypeFile->first, *iProcess, kLimitName);

        // Get the contents of each histogram to dump into file
        out_string << iCut->GetName() << "/" << *iProcess;
        for (int iBin = 1; iBin < NumXBins + 1; iBin++)
          out_string << delim << proc_hist->GetBinContent(iBin) << delim << proc_hist->GetBinError(iBin);

        delete proc_hist;
        out_string << "\n";
      }
    }
    std::string output_name = out_directory + "/datacard_" + iTypeFile->second + ".csv";
    Message(eInfo, "Writing output file to %s", output_name.data());
    std::ofstream output_file(output_name);
    output_file << out_string.str();
    output_file.close();
  }
}

//--------------------------------------------------------------------
void
YieldDump::DumpYieldFiles(std::string out_directory, Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  DumpYieldFiles(out_directory, NumXBins, XBins);
}
