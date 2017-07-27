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
YieldDump::SimpleExecute(sqlite3* conn, const char* query)
{
  int rc;
  sqlite3_stmt *stmt;

  Message(eInfo, "About to execute:\n%s", query);
  sqlite3_prepare_v2(conn, query, -1, &stmt, NULL);
  rc = sqlite3_step(stmt);
  Message(eInfo, "Return code: %i", rc);

  if (rc != SQLITE_OK and rc != SQLITE_DONE)
    Message(eError, "Error from connection:\n%s", sqlite3_errmsg(conn));

  rc = sqlite3_finalize(stmt);
  Message(eInfo, "Return code: %i", rc);

}

//--------------------------------------------------------------------
void
YieldDump::DumpYieldFiles(std::string out_directory, Int_t NumXBins, Double_t *XBins)
{
  std::vector<std::pair<FileType, std::string>> types_files = {
    std::make_pair(kBackground, "background"),
    std::make_pair(kSignal, "signal"),
    std::make_pair(kData, "data")
  };

  assert(out_directory != "");

  std::string delim = ",";

  sqlite3 *conn;
  if(sqlite3_open((out_directory + "/test.db").data(), &conn) != SQLITE_OK) {
    Message(eError, "Can't open database in %s", out_directory.data());
    sqlite3_close(conn);
    exit(50);
  }

  // Create table with all the uncertainties

  SimpleExecute(conn, "CREATE TABLE types (type INT PRIMARY KEY, name VARCHAR(32))");
  SimpleExecute(conn, R"SQL(
CREATE TABLE yields (
region VARCHAR(64),
process VARCHAR(64),
bin INT,
contents DOUBLE,
stat_unc DOUBLE,
type INT,
PRIMARY KEY (region, process, bin)
)
)SQL");

  int rc; 

  for (auto iTypeFile = types_files.begin(); iTypeFile != types_files.end(); ++iTypeFile) {

    // Create the custom enum table
    sqlite3_stmt *types_stmt;
    sqlite3_prepare_v2(conn, "INSERT INTO types VALUES(?, ?)", -1, &types_stmt, NULL);

    sqlite3_bind_int(types_stmt, 1, iTypeFile->first);
    sqlite3_bind_text(types_stmt, 2, iTypeFile->second.data(), -1, NULL);
    rc = sqlite3_step(types_stmt);

    if (rc != SQLITE_OK and rc != SQLITE_DONE)
      Message(eError, "Error from connection:\n%s", sqlite3_errmsg(conn));

    sqlite3_finalize(types_stmt);

    // Get the histogram
    auto processes = ReturnTreeNames(iTypeFile->first);
    std::stringstream out_string;

    // Make the first row of the datacard
    for (int iBin = 1; iBin < NumXBins + 1; iBin++) {
      if (out_string.str() != "")
        out_string << delim;
      out_string << "bin_" << iBin << delim << "bin_" << iBin << ".stat_unc";
    }
    out_string << "\n";

    for (unsigned int iCut = 0; iCut != fRegionCuts.size(); ++iCut) {

      SetDefaultCut(fRegionCuts[iCut]);
      SetDataWeight(fDataWeights[iCut]);
      SetMCWeight(fMCWeights[iCut]);

      for (auto iProcess = processes.begin(); iProcess != processes.end(); ++iProcess) {

        Message(eInfo, "Getting histograms from %s for %s region.", iProcess->Data(), fRegionCuts[iCut].GetName());
        auto proc_hist = GetHist(NumXBins, XBins, iTypeFile->first, *iProcess, kLimitName);

        // Get the contents of each histogram to dump into file
        out_string << fRegionCuts[iCut].GetName() << "/" << *iProcess;
        for (int iBin = 1; iBin < NumXBins + 1; iBin++) {
          out_string << delim << proc_hist->GetBinContent(iBin) << delim << proc_hist->GetBinError(iBin);

          // Dump into sqlite3 database
          sqlite3_stmt *yield_stmt;
          sqlite3_prepare_v2(conn, "INSERT INTO yields VALUES(?, ?, ?, ?, ?, ?)", -1, &yield_stmt, NULL);
          sqlite3_bind_text(yield_stmt, 1, fRegionCuts[iCut].GetName(), -1, NULL);
          sqlite3_bind_text(yield_stmt, 2, iProcess->Data(), -1, NULL);
          sqlite3_bind_int(yield_stmt, 3, iBin);
          sqlite3_bind_double(yield_stmt, 4, proc_hist->GetBinContent(iBin));
          sqlite3_bind_double(yield_stmt, 5, proc_hist->GetBinError(iBin));
          sqlite3_bind_int(yield_stmt, 6, iTypeFile->first);

          rc = sqlite3_step(yield_stmt);
          if (rc != SQLITE_OK and rc != SQLITE_DONE)
            Message(eError, "Error from connection:\n%s", sqlite3_errmsg(conn));

          sqlite3_finalize(yield_stmt);
          
        }

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

  sqlite3_close(conn);
}

//--------------------------------------------------------------------
void
YieldDump::DumpYieldFiles(std::string out_directory, Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  DumpYieldFiles(out_directory, NumXBins, XBins);
}
