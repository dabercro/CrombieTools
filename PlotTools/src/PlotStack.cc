/**
   @file   PlotStack.cc

   Source file of PlotStack, including the functions used for creating the histograms.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <sqlite3.h>

#include "TFile.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TRegexp.h"

#include "PlotStack.h"

ClassImp(PlotStack)

//--------------------------------------------------------------------

/**
   The default constructor for PlotStack.
   It changes the value of PlotBase::fMakeRatio to be true by default.
 */

PlotStack::PlotStack()
{
  fMakeRatio = true;
}

//--------------------------------------------------------------------
PlotStack::~PlotStack()
{ }

//--------------------------------------------------------------------
std::vector<HistHolder*>
PlotStack::MergeHistograms(FileType type, std::vector<TH1D*> hists) {

  DisplayFunc(__func__);

  std::vector<HistHolder*> HistHolders;

  auto *fileInfo = GetFileInfo(type);
  TString previousEntry = "";
  TH1D *tempMCHist = NULL;
  HistHolder *tempHistHolder = NULL;

  for (UInt_t iHist = 0; iHist != hists.size(); ++iHist) {

    Message(eDebug, "About to process Histogram", iHist, "out of", hists.size());
    Message(eDebug, "Entry is", (*fileInfo)[iHist]->fEntry, "Previous entry is",previousEntry, "");
    Message(eDebug, "Integral is", hists[iHist]->Integral());

    if (hists[iHist]->Integral() < 0) {
      Message(eError, "Histogram for", (*fileInfo)[iHist]->fFileName, "has negate integral", hists[iHist]->Integral());
      continue;
    }

    if ((*fileInfo)[iHist]->fEntry != previousEntry) {

      Message(eDebug, "Creating a new histogram");

      previousEntry = (*fileInfo)[iHist]->fEntry;
      TString tempName;
      tempName.Format("StackedHist_%d", iHist);
      tempMCHist = (TH1D*) hists[iHist]->Clone(tempName);
      tempHistHolder = new HistHolder(tempMCHist, (*fileInfo)[iHist]->fEntry,
                                      (*fileInfo)[iHist]->fColorStyle,
                                      (*fileInfo)[iHist]->fTreeName,
                                      (fForceTop == (*fileInfo)[iHist]->fEntry),
                                      (type == kSignal));
      HistHolders.push_back(tempHistHolder);

    } else {

      tempMCHist->Add(hists[iHist]);
      Message(eDebug, "Added to previous histogram.");

    }

    Message(eDebug, "Number of unique entries so far:", HistHolders.size());

  }

  return HistHolders;
}

//--------------------------------------------------------------------
void
PlotStack::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  bool reset_per = (fEventsPer == 0);
  if (reset_per)
    SetEventsPer((MaxX - MinX)/NumXBins);

  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  MakeCanvas(FileBase, NumXBins, XBins, XLabel, YLabel, logY);

  if (reset_per)
    SetEventsPer(0);
}

//--------------------------------------------------------------------
void
PlotStack::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString YLabel, Bool_t logY) {
  SetIncludeErrorBars(true);
  std::vector<TH1D*> DataHists = GetHistList(NumXBins, XBins, kData);
  Message(eDebug, "Number of Data Histograms:", DataHists.size());
  SetIncludeErrorBars(false);
  std::vector<TH1D*> MCHists = GetHistList(NumXBins, XBins, kBackground);
  Message(eDebug, "Number of MC Histograms:", MCHists.size());
  std::vector<TH1D*> SignalHists;
  if (fSignalFileInfo.size() != 0)
    SignalHists = GetHistList(NumXBins, XBins, kSignal);
  Message(eDebug, "Number of Signal Histograms:", SignalHists.size());

  MakeCanvas(FileBase, DataHists, MCHists, SignalHists, XLabel, YLabel, logY);
}

//--------------------------------------------------------------------
void
PlotStack::MakeCanvas(TString FileBase, TString XLabel, TString YLabel, Bool_t logY) {
  try {
    SetIncludeErrorBars(true);
    std::vector<TH1D*> DataHists = GetHistList(FileBase, kData);
    Message(eDebug, "Number of Data Histograms:", DataHists.size());
    SetIncludeErrorBars(false);
    std::vector<TH1D*> MCHists = GetHistList(FileBase, kBackground);
    Message(eDebug, "Number of MC Histograms:", MCHists.size());
    std::vector<TH1D*> SignalHists;
    if (fSignalFileInfo.size() != 0)
      SignalHists = GetHistList(FileBase, kSignal);
    Message(eDebug, "Number of Signal Histograms:", SignalHists.size());

    MakeCanvas(FileBase, DataHists, MCHists, SignalHists, XLabel, YLabel, logY);
  }
  catch(out_of_range e) {
    Message(eError, "Didn't get", FileBase, "from histograms...");
  }
}

//--------------------------------------------------------------------
void
PlotStack::MakeCanvas(TString FileBase, std::vector<TH1D*> DataHists, std::vector<TH1D*> MCHists, std::vector<TH1D*> SignalHists,
                      TString XLabel, TString YLabel, Bool_t logY) {
  DisplayFunc(__func__);
  Message(eInfo, "Making File :", FileBase);
  Message(eInfo, "Plotting    :", fDefaultExpr);
  Message(eInfo, "Labeled     :", XLabel);
  Message(eInfo, "With cut    :", fDefaultCut.GetTitle());

  if (YLabel == "") {
    if (fEventsPer == 0)
      YLabel = "Events/Bin";
    else if (fEventsPer >= 10)
      YLabel.Form("Events/%.0f", fEventsPer);
    else if (fEventsPer < 0.1)
      YLabel.Form("Events/%.3f", fEventsPer);
    else if (fEventsPer < 1.0)
      YLabel.Form("Events/%.2f", fEventsPer);
    else
      YLabel.Form("Events/%.1f", fEventsPer);
  }

  const Int_t NumXBins = MCHists[0]->GetNbinsX();
  const Double_t *XBins = MCHists[0]->GetXaxis()->GetXbins()->GetArray();

  SetLumiLabel(float(fLuminosity/1000.0));
  ResetLegend();

  fRatioLines.clear();

  std::vector<TFile*> TemplateFiles;
  TFile *templateFile = NULL;

  Message(eDebug, "Number of Templates:", fTemplateEntries.size());

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp) {
    Message(eDebug, "Getting template:", iTemp);
    templateFile = TFile::Open(fTemplateFiles[iTemp]);
    TemplateFiles.push_back(templateFile);
  }

  SetLegendFill(true);
  TH1D *DataHist = (TH1D*) DataHists[0]->Clone("DataHist");
  Message(eDebug, "Final Data Histogram created at", DataHist);
  DataHist->Reset("M");

  for (UInt_t iHist = 0; iHist < DataHists.size(); iHist++)
    DataHist->Add(DataHists[iHist]);

  Message(eInfo, "Number of data events: ", (Int_t) DataHist->GetEntries(), ", integral:", DataHist->Integral("width")/(fEventsPer ? fEventsPer : 1.0));

  std::vector<HistHolder*> HistHolders = MergeHistograms(kBackground, MCHists);

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp) {

    for (UInt_t iHist = 0; iHist != HistHolders.size(); ++iHist) {

      if (fTemplateEntries[iTemp] == HistHolders[iHist]->fEntry) {

        Message(eInfo, "Replacing histogram %s with a template",
                fTemplateEntries[iTemp].Data());

        TH1D *templateHist = (TH1D*) TemplateFiles[iTemp]->Get(fTemplateHists[iTemp]);
        TH1D *toFormat = (TH1D*) templateHist->Rebin(NumXBins, "", XBins);
        toFormat->SetFillStyle(HistHolders[iHist]->fHist->GetFillStyle());
        toFormat->SetFillColor(HistHolders[iHist]->fHist->GetFillColor());
        toFormat->SetMarkerSize(HistHolders[iHist]->fHist->GetMarkerSize());
        HistHolders[iHist]->fHist = toFormat;

      }
    }
  }

  std::vector<HistHolder*> SignalHolders = MergeHistograms(kSignal, SignalHists);

  if (fDumpRootName != "") {

    Message(eInfo, "Dumping histograms into", fDumpRootName);

    TH1D* tempHist;
    TFile* dumpFile = new TFile(fDumpRootName,"RECREATE");

    // Background Histograms

    auto variable = fDefaultExpr;
    if (variable.EndsWith("Up") or variable.EndsWith("Down"))
      variable = variable(0, variable.Index(TRegexp("_[^_]+$")));

    for (UInt_t iHist = 0; iHist != HistHolders.size(); ++iHist) {

      tempHist = (TH1D*) HistHolders[iHist]->fHist->Clone();

      Message(eInfo, HistHolders[iHist]->fEntry, " : ",
              tempHist->Integral(0, NumXBins + 1, "width")/(fEventsPer ? fEventsPer : 1.0));

      dumpFile->WriteTObject(tempHist, TString::Format("%s__%s__%s",
                                                       variable.Data(),
                                                       HistHolders[iHist]->fTree.Data(),
                                                       fHistSuff.Data()
                                                       )
                             );
    }

    // Signal Histograms

    for (auto iHist = SignalHolders.begin(); iHist != SignalHolders.end(); ++iHist) {

      tempHist = (TH1D*) (*iHist)->fHist->Clone();

      Message(eInfo, (*iHist)->fTree, " : ",
              tempHist->Integral(0, NumXBins + 1, "width")/(fEventsPer ? fEventsPer : 1.0));

      dumpFile->WriteTObject(tempHist, TString::Format("%s__%s__%s",
                                                       variable.Data(),
                                                       (*iHist)->fTree.Data(),
                                                       fHistSuff.Data()
                                                       )
                             );

    }

    // Data Histogram

    tempHist = (TH1D*) DataHist->Clone();
    Message(eInfo, "Data     : ", tempHist->Integral(0, NumXBins + 1, "width")/(fEventsPer ? fEventsPer : 1.0));
    dumpFile->WriteTObject(tempHist, TString::Format("%s__data_obs__%s",
                                                     variable.Data(),
                                                     fHistSuff.Data()
                                                     )
                           );
    dumpFile->Close();

  }

  if (fSortBackground) {
    std::sort(HistHolders.begin(), HistHolders.end(), SortHistHolders);
    Message(eInfo, "Backgrounds sorted");
  }

  Message(eDebug, "Number of unique histograms: %i", HistHolders.size());

  std::vector<TH1D*> AllHists;
  for (UInt_t iLarger = 0; iLarger != HistHolders.size(); ++iLarger) {

    Message(eDebug, "Stacking up histogram:", iLarger);
    Message(eInfo, "Entry", HistHolders[iLarger]->fEntry, "has total integral",
            HistHolders[iLarger]->fHist->Integral("width")/(fEventsPer ? fEventsPer : 1.0));

    for (UInt_t iSmaller = iLarger + 1; iSmaller != HistHolders.size(); ++iSmaller) {
      Message(eDebug, "Adding", iSmaller, "to", iLarger);
      HistHolders[iLarger]->fHist->Add(HistHolders[iSmaller]->fHist);
    }

    Message(eDebug, "Histogram", iLarger, "has integral", HistHolders[iLarger]->fHist->Integral());

    if (HistHolders[iLarger]->fHist->Integral() > 0 || iLarger == 0) {

      if (iLarger != 0)
        SetZeroError(HistHolders[iLarger]->fHist);
      AllHists.push_back(HistHolders[iLarger]->fHist);

      if ((HistHolders[iLarger]->fHist->Integral() > fMinLegendFrac * HistHolders[0]->fHist->Integral()) ||  // If more than the fraction set
          (iLarger == HistHolders.size() - 1))                                                               // or the last histogram
        AddLegendEntry(HistHolders[iLarger]->fEntry, 1, fStackLineWidth, 1);                                 // Add legend properly
      else if ((HistHolders[iLarger]->fHist->Integral() > fIgnoreInLinear * HistHolders[0]->fHist->Integral()) ||
               logY) {                                                                                       // Otherwise if not ignored
        if (HistHolders[iLarger + 1]->fHist->Integral() > 0) {                                               // Check if the next histogram contribute
          if (fOthersColor != 0)
            HistHolders[iLarger]->fHist->SetFillColor(fOthersColor);
          AddLegendEntry("Others", 1, fStackLineWidth, 1);                                                   // If so, make others legend
        }
        else                                                                                                 // If not,
          AddLegendEntry(HistHolders[iLarger]->fEntry, HistHolders[iLarger]->fColor, 1, 1);                  // Make normal legend entry
        break;                                                                                               // Stop adding histograms
      } else {
        AllHists.pop_back();
        break;
      }
    }

    Message(eDebug, "There are now", AllHists.size() ,"total histograms to plot.");

  }

  Message(eInfo, "Total background contribution:", HistHolders[0]->fHist->Integral("width")/(fEventsPer ? fEventsPer : 1.0));

  if (not DataHist->Integral()) {
    auto* match = AllHists[0];
    for (int i_bin = 1; i_bin <= NumXBins; ++i_bin) {
      auto content = match->GetBinContent(i_bin);
      DataHist->SetBinContent(i_bin, content);
      DataHist->SetBinError(i_bin, sqrt(content));
    }
  }

  AddLegendEntry("Data", 1);
  SetDataIndex(int(AllHists.size()));
  AllHists.push_back(DataHist);

  if (fSortSignal) {
    std::sort(SignalHolders.begin(), SignalHolders.end(), SortHistHolders);
    Message(eInfo, "Signals sorted");
  }

  for (UInt_t iHist = 0; iHist != SignalHolders.size(); ++iHist) {

    if (fAddSignal) {                            // Do some clever things if we're adding signal to backgrounds

      if (fMakeRatio)                            // All possible signals show up on the ratio pad
        AddRatioLine(int(AllHists.size()));
      if (AllHists.size() > 1)
        SignalHolders[iHist]->fHist->Add(AllHists[0]);    // Add the background to the signals

    }

    AllHists.push_back(SignalHolders[iHist]->fHist);
    AddLegendEntry(SignalHolders[iHist]->fEntry, 1, 2, SignalHolders[iHist]->fColor);
    Message(eDebug, "There are now", AllHists.size(), "total histograms to plot.");
  }

  if (fPostFitFile != "") {
    TString tempName = TempHistName();
    TH1D* post_fit = new TH1D(tempName, tempName, NumXBins, XBins);

    const char* signal_name = fSignalFileInfo.size() == 0 ? "" : fSignalFileInfo[0]->fTreeName.Data();

    sqlite3 *conn;
    if(sqlite3_open(fPostFitFile.Data(), &conn) != SQLITE_OK) {
      Message(eError, "Can't open database in", fPostFitFile);
      sqlite3_close(conn);
      exit(50);
    }

    for (int i_bin = 1; i_bin <= NumXBins; i_bin++) {
      sqlite3_stmt *fetch_stmt;
      sqlite3_prepare_v2(conn, R"SQL(
SELECT yield FROM postfit_yields
WHERE region = ? AND bin = ? AND signal = ?
)SQL", -1, &fetch_stmt, NULL);

      sqlite3_bind_text(fetch_stmt, 1, fRegion.Data(), -1, NULL);
      sqlite3_bind_int(fetch_stmt, 2, i_bin);
      sqlite3_bind_text(fetch_stmt, 3, signal_name, -1, NULL);

      int rc = sqlite3_step(fetch_stmt);

      post_fit->SetBinContent(i_bin, sqlite3_column_double(fetch_stmt, 0));

      sqlite3_finalize(fetch_stmt);
    }

    sqlite3_close(conn);

    post_fit->SetLineWidth(2);
    post_fit->SetLineColor(2);

    if (fMakeRatio)
      AddRatioLine(int(AllHists.size()));
    AllHists.push_back(post_fit);
    AddLegendEntry("Post Fit", 2);

  }

  Message(eDebug, "There are now", AllHists.size() ,"total histograms to plot.");

  if (fMakeRatio)
    AddRatioLine(fDataIndex);

  SetRatioIndex(0);

  if (not std::getenv("blind"))
    BaseCanvas(AddOutDir(FileBase), AllHists, XLabel, YLabel, logY);

  if (not fPrepared) { // If Prepared, stored and owned properly by PlotPreparer base
    for (UInt_t iHist = 0; iHist != AllHists.size(); ++iHist) {
      delete AllHists[iHist];
      AllHists[iHist] = nullptr;
    }
    for (UInt_t iHist = 0; iHist != MCHists.size(); ++iHist) {
      delete MCHists[iHist];
      MCHists[iHist] = nullptr;
    }
    for (UInt_t iHist = 0; iHist != DataHists.size(); ++iHist) {
      delete DataHists[iHist];
      DataHists[iHist] = nullptr;
    }
  }

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp)
    TemplateFiles[iTemp]->Close();

  CloseFiles();
}

//--------------------------------------------------------------------
PlotStack*
PlotStack::Copy()
{
  PlotStack *newPlotter = new PlotStack();
  *newPlotter = *this;
  return newPlotter;
}
