/**
   @file   PlotStack.cc

   Source file of PlotStack, including the functions used for creating the histograms.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <algorithm>

#include "TFile.h"
#include "TLegend.h"
#include "TProfile.h"

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

  std::vector<HistHolder*> HistHolders;

  auto *fileInfo = GetFileInfo(type);
  TString previousEntry = "";
  TH1D *tempMCHist = NULL;
  HistHolder *tempHistHolder = NULL;

  for (UInt_t iHist = 0; iHist != hists.size(); ++iHist) {

    Message(eDebug, "About to process Histogram %i out of %i", iHist, hists.size()); 
    Message(eDebug, "Entry is \"%s\". Previous entry is \"%s\"",
            (*fileInfo)[iHist]->fEntry.Data(), previousEntry.Data());

    if ((*fileInfo)[iHist]->fEntry != previousEntry) {

      Message(eDebug, "Creating a new histogram");

      previousEntry = (*fileInfo)[iHist]->fEntry;
      TString tempName;
      tempName.Format("StackedHist_%d",iHist);
      tempMCHist = (TH1D*) hists[iHist]->Clone(tempName);
      tempHistHolder = new HistHolder(tempMCHist, (*fileInfo)[iHist]->fEntry,
                                      (*fileInfo)[iHist]->fColorStyle,
                                      (*fileInfo)[iHist]->fTreeName,
                                      (fForceTop == (*fileInfo)[iHist]->fEntry));
      HistHolders.push_back(tempHistHolder);

    } else {

      tempMCHist->Add(hists[iHist]);
      Message(eDebug, "Added to previous histogram.");

    }

    Message(eDebug, "Number of unique entries so far: %i", HistHolders.size());

  }

  return HistHolders;
}

//--------------------------------------------------------------------
void
PlotStack::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  DisplayFunc(__func__);
  Message(eInfo, "Making File :   %s", FileBase.Data());
  Message(eInfo, "Plotting    :   %s", fDefaultExpr.Data());
  Message(eInfo, "Labeled     :   %s", XLabel.Data());
  Message(eInfo, "With cut    :   %s", fDefaultCut.GetTitle());

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

  SetLumiLabel(float(fLuminosity/1000.0));
  ResetLegend();

  fRatioLines.clear();

  std::vector<TFile*> TemplateFiles;
  TFile *templateFile = NULL;

  Message(eDebug, "Number of Templates: %i", fTemplateEntries.size());

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp) {
    Message(eDebug, "Getting template: %i", iTemp);
    templateFile = TFile::Open(fTemplateFiles[iTemp]);
    TemplateFiles.push_back(templateFile);
  }

  SetIncludeErrorBars(true);
  std::vector<TH1D*> DataHists = GetHistList(NumXBins, XBins, kData);
  Message(eDebug, "Number of Data Histograms: %i", DataHists.size());
  SetIncludeErrorBars(false);
  std::vector<TH1D*> MCHists = GetHistList(NumXBins, XBins, kBackground);
  Message(eDebug, "Number of MC Histograms: %i", MCHists.size());
  std::vector<TH1D*> SignalHists;
  if (fSignalFileInfo.size() != 0)
    SignalHists = GetHistList(NumXBins, XBins, kSignal);
  Message(eDebug, "Number of Signal Histograms: %i", SignalHists.size());

  SetLegendFill(true);
  TH1D *DataHist = (TH1D*) DataHists[0]->Clone("DataHist");
  Message(eDebug, "Final Data Histogram created at %p", DataHist);
  DataHist->Reset("M");

  for (UInt_t iHist = 0; iHist < DataHists.size(); iHist++)
    DataHist->Add(DataHists[iHist]);

  Message(eInfo, "Number of data events: %i, integral: %f", (Int_t) DataHist->GetEntries(), DataHist->Integral("width"));

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

  if (fDumpRootName != "") {

    Message(eInfo, "Dumping histograms into %s", fDumpRootName.Data());

    TH1D* tempHist;
    TFile* dumpFile = new TFile(fDumpRootName,"RECREATE");

    // Background Histograms

    for (UInt_t iHist = 0; iHist != HistHolders.size(); ++iHist) {

      tempHist = (TH1D*) HistHolders[iHist]->fHist->Clone();

      Message(eInfo, "%s  :  %f", HistHolders[iHist]->fEntry.Data(),
              tempHist->Integral(0, NumXBins + 1, "width"));

      dumpFile->WriteTObject(tempHist, TString::Format("%s-%s",
                                                       fDefaultExpr.Data(),
                                                       HistHolders[iHist]->fTree.Data()
                                                       )
                             );
    }

    // Signal Histograms

    for (UInt_t iHist = 0; iHist != SignalHists.size(); ++iHist) {

      tempHist = (TH1D*) SignalHists[iHist]->Clone();

      Message(eInfo, "%s  :  %f", fSignalFileInfo[iHist]->fTreeName.Data(),
              tempHist->Integral(0, NumXBins + 1, "width"));

      dumpFile->WriteTObject(tempHist, TString::Format("%s-%s",
                                                       fDefaultExpr.Data(),
                                                       fSignalFileInfo[iHist]->fTreeName.Data()
                                                       )
                             );

    }

    // Data Histogram

    tempHist = (TH1D*) DataHist->Clone();
    Message(eInfo, "Data     :  %f", tempHist->Integral(0, NumXBins + 1, "width"));
    dumpFile->WriteTObject(tempHist, TString::Format("%s-data", fDefaultExpr.Data()));
    dumpFile->Close();

  }

  if (fSortBackground) {
    std::sort(HistHolders.begin(), HistHolders.end(), SortHistHolders);
    Message(eInfo, "Backgrounds sorted");
  }

  Message(eDebug, "Number of unique histograms: %i", HistHolders.size());

  std::vector<TH1D*> AllHists;
  for (UInt_t iLarger = 0; iLarger != HistHolders.size(); ++iLarger) {

    Message(eDebug, "Stacking up histogram: %i", iLarger);
    Message(eInfo, "Entry %s has total integral %f",
            HistHolders[iLarger]->fEntry.Data(),
            HistHolders[iLarger]->fHist->Integral("width"));

    for (UInt_t iSmaller = iLarger + 1; iSmaller != HistHolders.size(); ++iSmaller) {
      Message(eDebug, "Adding %i to %i", iSmaller, iLarger);
      HistHolders[iLarger]->fHist->Add(HistHolders[iSmaller]->fHist);
    }

    Message(eDebug, "Histogram %i has integral %f", iLarger, HistHolders[iLarger]->fHist->Integral());

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

    Message(eDebug, "There are now %i total histograms to plot.", AllHists.size());

  }

  Message(eInfo, "Total background contribution: %f", HistHolders[0]->fHist->Integral("width"));

  AddLegendEntry("Data", 1);
  SetDataIndex(int(AllHists.size()));
  AllHists.push_back(DataHist);

  std::vector<HistHolder*> SignalHolders = MergeHistograms(kSignal, SignalHists);

  if (fSortSignal) {
    std::sort(SignalHolders.begin(), SignalHolders.end(), SortHistHolders);
    Message(eInfo, "Signals sorted");
  }

  for (auto iHist = SignalHolders.begin(); iHist != SignalHolders.end(); ++iHist) {

    if (fAddSignal) {                            // Do some clever things if we're adding signal to backgrounds

      if (fMakeRatio)                            // All possible signals show up on the ratio pad
        AddRatioLine(int(AllHists.size()));
      if (AllHists.size() > 1)
        (*iHist)->fHist->Add(AllHists[0]);    // Add the background to the signals

    }

    AllHists.push_back((*iHist)->fHist);
    AddLegendEntry((*iHist)->fEntry, 1, 2, (*iHist)->fColor);
    Message(eDebug, "There are now %i total histograms to plot.", AllHists.size());
  }

  Message(eDebug, "There are now %i total histograms to plot.", AllHists.size());

  if (fMakeRatio)
    AddRatioLine(fDataIndex);

  SetRatioIndex(0);

  BaseCanvas(AddOutDir(FileBase), AllHists, XLabel, YLabel, logY);

  for (UInt_t iHist = 0; iHist != AllHists.size(); ++iHist)
    delete AllHists[iHist];
  for (UInt_t iHist = 0; iHist != MCHists.size(); ++iHist)
    delete MCHists[iHist];
  for (UInt_t iHist = 0; iHist != DataHists.size(); ++iHist)
    delete DataHists[iHist];

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp)
    TemplateFiles[iTemp]->Close();

  CloseFiles();
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
PlotStack*
PlotStack::Copy()
{
  PlotStack *newPlotter = new PlotStack();
  *newPlotter = *this;
  return newPlotter;
}
