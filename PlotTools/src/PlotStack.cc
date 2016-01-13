#include <fstream>
#include <iostream>
#include <algorithm>

#include "TFile.h"
#include "TLegend.h"

#include "PlotStack.h"

ClassImp(PlotStack)

//--------------------------------------------------------------------
PlotStack::PlotStack() :
  fTreeName("events"),
  fAllHist("htotal"),
  fLuminosity(19700),
  fDataContainer(0),
  fMCContainer(0),
  fDebug(false)
{
  fFriends.resize(0);
  fDataFiles.resize(0);
  fMCFiles.resize(0);
  fXSecs.resize(0);
  fStackEntries.resize(0);
  fStackColors.resize(0);
}

//--------------------------------------------------------------------
PlotStack::~PlotStack()
{}

//--------------------------------------------------------------------
void
PlotStack::ReadMCConfig(TString config, TString fileDir)
{
  if (fileDir != "" && !fileDir.EndsWith("/"))
      fileDir = fileDir + "/";

  std::ifstream configFile;
  configFile.open(config.Data());
  TString FileName;
  TString XSec;
  TString LegendEntry;
  TString ColorEntry;
  while (!configFile.eof()) {
    configFile >> FileName >> XSec >> LegendEntry >> ColorEntry;
    if (ColorEntry != "")
      AddMCFile(fileDir + FileName, XSec.Atof(), LegendEntry, ColorEntry.Atoi());
  }
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotStack::GetHistList(std::vector<TString> FileList, Int_t NumXBins, Double_t *XBins, Bool_t isMC)
{
  TreeContainer *tempContainer = NULL;
  if (isMC)
    tempContainer = fMCContainer;
  else
    tempContainer = fDataContainer;

  for (UInt_t iFile = 0; iFile < FileList.size(); iFile++)
    tempContainer->AddFile(FileList[iFile]);

  SetTreeList(tempContainer->ReturnTreeList());
  std::vector<TFile*> theFiles = tempContainer->ReturnFileList();
  std::vector<TH1D*> theHists = MakeHists(NumXBins,XBins);
  for (UInt_t iFile = 0; iFile < theFiles.size(); iFile++) {
    if (isMC) {
      TH1D *allHist = (TH1D*) theFiles[iFile]->FindObjectAny(fAllHist);
      if (fDebug) {
        std::cout << "Integral before " << theHists[iFile]->Integral() << std::endl;
        std::cout << "Scale factor " << fLuminosity*fXSecs[iFile]/allHist->GetBinContent(1) << std::endl;
      }
      theHists[iFile]->Scale(fLuminosity*fXSecs[iFile]/allHist->GetBinContent(1));
      SetZeroError(theHists[iFile]);
      if (fDebug)
        std::cout << "Integral after " << theHists[iFile]->Integral() << std::endl;
    }
    else if (fDebug)
      std::cout << "Data yield " << theHists[iFile]->Integral() << std::endl;
  }
  return theHists;
}

//--------------------------------------------------------------------
void
PlotStack::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  fDataContainer = new TreeContainer();
  fDataContainer->SetTreeName(fTreeName);
  fMCContainer = new TreeContainer();
  fMCContainer->SetTreeName(fTreeName);
  for (UInt_t iFriend = 0; iFriend != fFriends.size(); ++iFriend) {
    fDataContainer->AddFriendName(fFriends[iFriend]);
    fMCContainer->AddFriendName(fFriends[iFriend]);
  }

  std::vector<TH1D*> DataHists = GetHistList(fDataFiles,NumXBins,XBins,false);
  std::vector<TH1D*>   MCHists = GetHistList(fMCFiles,NumXBins,XBins,true);

  std::vector<TH1D*> theHists;
  SetRatioIndex(0);
  SetOnlyRatioWithData(true);
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
    if (fStackEntries[iHist] != previousEntry) {
      previousEntry = fStackEntries[iHist];
      TString tempName;
      tempName.Format("StackedHist_%d",iHist);
      tempMCHist = (TH1D*) MCHists[iHist]->Clone(tempName);
      tempHistHolder = new HistHolder(tempMCHist,fStackEntries[iHist],fStackColors[iHist]);
      HistHolders.push_back(tempHistHolder);
    }
    else
      tempMCHist->Add(MCHists[iHist]);
  }

  std::sort(HistHolders.begin(),HistHolders.end(),SortHistHolders);

  std::vector<TH1D*> AllHists;
  for (UInt_t iLarger = 0; iLarger != HistHolders.size(); ++iLarger) {
    for (UInt_t iSmaller = iLarger + 1; iSmaller != HistHolders.size(); ++iSmaller)
      HistHolders[iLarger]->fHist->Add(HistHolders[iSmaller]->fHist);

    AllHists.push_back(HistHolders[iLarger]->fHist);
    AddLegendEntry(HistHolders[iLarger]->fEntry,HistHolders[iLarger]->fColor,1,1);
  }

  AddLegendEntry("Data",1);
  SetDataIndex(int(AllHists.size()));
  AllHists.push_back(DataHist);

  BaseCanvas(FileBase,AllHists,XLabel,YLabel,logY);

  for (UInt_t iHist = 0; iHist != AllHists.size(); ++iHist)
    delete AllHists[iHist];
  for (UInt_t iHist = 0; iHist != MCHists.size(); ++iHist)
    delete MCHists[iHist];
  for (UInt_t iHist = 0; iHist != DataHists.size(); ++iHist)
    delete DataHists[iHist];

  delete fDataContainer;
  delete fMCContainer;
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

