/**
  @file   PlotStack.cc

  Source file of PlotStack, including the functions used for creating the histograms.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <iostream>
#include <algorithm>

#include "TFile.h"
#include "TLegend.h"
#include "TProfile.h"

#include "HistHolder.h"
#include "PlotStack.h"

ClassImp(PlotStack)

//--------------------------------------------------------------------
PlotStack::PlotStack() :
  fForceTop("")
{ }

//--------------------------------------------------------------------
PlotStack::~PlotStack()
{ }

//--------------------------------------------------------------------
void
PlotStack::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  std::cout << std::endl;
  std::cout << " C R O M B I E   S T A C K " << std::endl;
  std::cout << std::endl;
  std::cout << "   Making File :   " << FileBase << std::endl;
  std::cout << "   Plotting    :   " << fDefaultExpr << std::endl;
  std::cout << "   Labeled     :   " << XLabel << std::endl;  
  std::cout << "   With cut    :   " << fDefaultCut << std::endl;
  std::cout << std::endl;

  SetLumiLabel(float(fLuminosity/1000.0));
  ResetLegend();

  std::vector<TFile*> TemplateFiles;
  TFile *templateFile = NULL;

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp) {
    templateFile = TFile::Open(fTemplateFiles[iTemp]);
    TemplateFiles.push_back(templateFile);
  }

  SetIncludeErrorBars(true);
  std::vector<TH1D*> DataHists = GetHistList(NumXBins, XBins, kData);
  SetIncludeErrorBars(false);
  std::vector<TH1D*> MCHists = GetHistList(NumXBins, XBins, kBackground);
  std::vector<TH1D*> SignalHists;
  if (fSignalFileInfo.size() != 0)
    SignalHists = GetHistList(NumXBins, XBins, kSignal);

  SetLegendFill(true);
  TH1D *DataHist = (TH1D*) DataHists[0]->Clone("DataHist");
  DataHist->Reset("M");

  for (UInt_t iHist = 0; iHist < DataHists.size(); iHist++)
    DataHist->Add(DataHists[iHist]);

  TString previousEntry = "";
  TH1D *tempMCHist = 0;
  HistHolder *tempHistHolder = 0;
  std::vector<HistHolder*> HistHolders;
  HistHolders.resize(0);

  for (UInt_t iHist = 0; iHist != MCHists.size(); ++iHist) {

    if (fMCFileInfo[iHist]->fEntry != previousEntry) {

      previousEntry = fMCFileInfo[iHist]->fEntry;
      TString tempName;
      tempName.Format("StackedHist_%d",iHist);
      tempMCHist = (TH1D*) MCHists[iHist]->Clone(tempName);
      tempHistHolder = new HistHolder(tempMCHist, fMCFileInfo[iHist]->fEntry, fMCFileInfo[iHist]->fColorStyle,
                                      (fForceTop == fMCFileInfo[iHist]->fEntry));
      HistHolders.push_back(tempHistHolder);

    }
    else
      tempMCHist->Add(MCHists[iHist]);

  }

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp) {

    for (UInt_t iHist = 0; iHist != HistHolders.size(); ++iHist) {

      if (fTemplateEntries[iTemp] == HistHolders[iHist]->fEntry) {

        /// @todo Make sure to include a template in the tests
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

    TH1D* tempHist;
    TFile* dumpFile = new TFile(fDumpRootName,"RECREATE");

    for (UInt_t iHist = 0; iHist != HistHolders.size(); ++iHist) {

      tempHist = (TH1D*) HistHolders[iHist]->fHist->Clone();
      std::cout << HistHolders[iHist]->fEntry << "  :  " << tempHist->Integral(0, NumXBins + 1, "width") << std::endl;
      dumpFile->WriteTObject(tempHist,HistHolders[iHist]->fEntry);

    }

    tempHist = (TH1D*) DataHist->Clone();
    std::cout << "Data     :  " << tempHist->Integral(0, NumXBins + 1, "width") << std::endl;
    dumpFile->WriteTObject(tempHist, "Data");
    dumpFile->Close();

  }

  if (fSortBackground)
    std::sort(HistHolders.begin(), HistHolders.end(), SortHistHolders);

  std::vector<TH1D*> AllHists;
  for (UInt_t iLarger = 0; iLarger != HistHolders.size(); ++iLarger) {
    for (UInt_t iSmaller = iLarger + 1; iSmaller != HistHolders.size(); ++iSmaller)
      HistHolders[iLarger]->fHist->Add(HistHolders[iSmaller]->fHist);

    if (HistHolders[iLarger]->fHist->Integral() > 0) {
      if (iLarger != 0)
        SetZeroError(HistHolders[iLarger]->fHist);
      AllHists.push_back(HistHolders[iLarger]->fHist);

      if ((HistHolders[iLarger]->fHist->Integral() > fMinLegendFrac * HistHolders[0]->fHist->Integral()) ||  // If less than the fraction set
          (iLarger == HistHolders.size() - 1))                                                               // or the last histogram
        AddLegendEntry(HistHolders[iLarger]->fEntry,1,fStackLineWidth,1);                                    // Add legend properly
      else if ((HistHolders[iLarger]->fHist->Integral() > fIgnoreInLinear * HistHolders[0]->fHist->Integral()) ||
               logY) {                                                                                       // Otherwise if not ignored
        if (HistHolders[iLarger + 1]->fHist->Integral() > 0) {                                               // Check if the next histogram contribute
          if (fOthersColor != 0)
            HistHolders[iLarger]->fHist->SetFillColor(fOthersColor);
          AddLegendEntry("Others",1,fStackLineWidth,1);                                                      // If so, make others legend
        }
        else                                                                                                 // If not,
          AddLegendEntry(HistHolders[iLarger]->fEntry,HistHolders[iLarger]->fColor,1,1);                     // Make normal legend entry

        break;                                                                                               // Stop adding histograms
      }
      else {
        AllHists.pop_back();
        break;
      }
    }
  }

  AddLegendEntry("Data",1);
  SetDataIndex(int(AllHists.size()));
  AllHists.push_back(DataHist);

  for (UInt_t iHist = 0; iHist != SignalHists.size(); ++iHist) {
    if (fMakeRatio)
      AddRatioLine(int(AllHists.size()));
    if (AllHists.size() != 0)
      SignalHists[iHist]->Add(AllHists[0]);
    AllHists.push_back(SignalHists[iHist]);
    AddLegendEntry(fSignalFileInfo[iHist]->fEntry,1,2,fSignalFileInfo[iHist]->fColorStyle);
  }

  if (fMakeRatio) {
    SetRatioIndex(0);
    AddRatioLine(fDataIndex);
  }

  BaseCanvas(AddOutDir(FileBase),AllHists,XLabel,YLabel,logY);

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
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins,MinX,MaxX,XBins);
  MakeCanvas(FileBase,NumXBins,XBins,XLabel,YLabel,logY);
}

//--------------------------------------------------------------------
PlotStack*
PlotStack::Copy()
{
  PlotStack *newPlotter = new PlotStack();
  *newPlotter = *this;
  return newPlotter;
}
