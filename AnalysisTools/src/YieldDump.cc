#include <set>
#include <utility>

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
YieldDump::DumpYieldFiles(const char* out_file, Int_t NumXBins, Double_t *XBins)
{
  std::vector<std::pair<FileType, std::string>> types_files = {
    std::make_pair(kBackground, "background"),
    std::make_pair(kSignal, "signal"),
    std::make_pair(kData, "data")
  };

  std::string delim = ",";

  sqlite3 *conn;
  if(sqlite3_open(out_file, &conn) != SQLITE_OK) {
    Message(eError, "Can't open database in %s", out_file);
    sqlite3_close(conn);
    exit(50);
  }

  // Create table with all the uncertainties

  SimpleExecute(conn, R"SQL(
CREATE TABLE IF NOT EXISTS yields (
region VARCHAR(64),
process VARCHAR(64),
bin INT,
contents DOUBLE,
stat_unc DOUBLE,
type VARCHAR(32),
PRIMARY KEY (region, process, bin)
)
)SQL");

  int rc; 

  for (auto iTypeFile = types_files.begin(); iTypeFile != types_files.end(); ++iTypeFile) {

    // Get the histogram
    auto processes = ReturnTreeNames(iTypeFile->first);

    for (unsigned int iCut = 0; iCut != fRegionCuts.size(); ++iCut) {

      SetDefaultCut(fRegionCuts[iCut]);
      SetDataWeight(fDataWeights[iCut]);
      SetMCWeight(fMCWeights[iCut]);

      for (auto iProcess = processes.begin(); iProcess != processes.end(); ++iProcess) {

        Message(eInfo, "Getting histograms from %s for %s region.",
                iProcess->Data(), fRegionCuts[iCut].GetName());

        auto proc_hist = GetHist(NumXBins, XBins, iTypeFile->first, *iProcess, kLimitName);

        // Get the contents of each histogram to dump into file
        for (int iBin = 1; iBin < NumXBins + 1; iBin++) {

          // Dump into sqlite3 database
          sqlite3_stmt *yield_stmt;
          sqlite3_prepare_v2(conn, "INSERT INTO yields VALUES(?, ?, ?, ?, ?, ?)", -1, &yield_stmt, NULL);
          sqlite3_bind_text(yield_stmt, 1, fRegionCuts[iCut].GetName(), -1, NULL);
          sqlite3_bind_text(yield_stmt, 2, iProcess->Data(), -1, NULL);
          sqlite3_bind_int(yield_stmt, 3, iBin);
          sqlite3_bind_double(yield_stmt, 4, proc_hist->GetBinContent(iBin));
          sqlite3_bind_double(yield_stmt, 5, proc_hist->GetBinError(iBin));
          sqlite3_bind_text(yield_stmt, 6, iTypeFile->second.data(), -1, NULL);

          rc = sqlite3_step(yield_stmt);
          if (rc != SQLITE_OK and rc != SQLITE_DONE)
            Message(eError, "Error from connection:\n%s", sqlite3_errmsg(conn));

          sqlite3_finalize(yield_stmt);
          
        }

        delete proc_hist;
      }
    }
  }

  sqlite3_close(conn);
}

//--------------------------------------------------------------------
void
YieldDump::DumpYieldFiles(const char* out_file, Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  DumpYieldFiles(out_file, NumXBins, XBins);
}
