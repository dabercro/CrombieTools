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
  fLuminosity(2110.0),
  fDataContainer(0),
  fMCContainer(0),
  fDataWeights(""),
  fMCWeights(""),
  fMinLegendFrac(0.0),
  fForceTop(""),
  fDebug(false),
  fDumpRootName("")
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
  TString currLegend;
  TString currColor;
  while (!configFile.eof()) {
    configFile >> FileName >> XSec >> LegendEntry >> ColorEntry;
    if (LegendEntry == ".")
      LegendEntry = currLegend;
    else
      currLegend = LegendEntry;

    if (ColorEntry == ".")
      ColorEntry = currColor;
    else
      currColor = ColorEntry;

    if (ColorEntry != "" && !FileName.BeginsWith('#'))
      AddMCFile(fileDir + FileName, XSec.Atof(), LegendEntry.ReplaceAll("_"," "), ColorEntry.Atoi());
  }
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotStack::GetHistList(Int_t NumXBins, Double_t *XBins, Bool_t isMC)
{
  std::vector<TString> FileList;
  TreeContainer *tempContainer = NULL;
  TString tempCutHolder = "";
  if (isMC) {
    FileList = fMCFiles;
    tempContainer = fMCContainer;
    if (fMCWeights != "") {
      tempCutHolder = fDefaultCut;
      SetDefaultWeight(TString("(") + tempCutHolder + TString(")*(") + fMCWeights + TString(")"));
    }
  }
  else {
    FileList = fDataFiles;
    tempContainer = fDataContainer;
    if (fDataWeights != "") {
      tempCutHolder = fDefaultCut;
      SetDefaultWeight(TString("(") + tempCutHolder + TString(") && (") + fDataWeights + TString(")"));
    }
  }
  
  for (UInt_t iFile = 0; iFile < FileList.size(); iFile++)
    tempContainer->AddFile(FileList[iFile]);

  SetTreeList(tempContainer->ReturnTreeList());
  std::vector<TFile*> theFiles = tempContainer->ReturnFileList();
  std::vector<TH1D*> theHists = MakeHists(NumXBins,XBins);
  if (tempCutHolder != "")
    SetDefaultWeight(tempCutHolder);

  for (UInt_t iFile = 0; iFile < theFiles.size(); iFile++) {
    if (isMC) {
      TH1D *allHist = (TH1D*) theFiles[iFile]->FindObjectAny(fAllHist);
      if (fDebug) {
        std::cout << "Integral before " << theHists[iFile]->Integral() << std::endl;
        std::cout << "Scale factor " << fLuminosity*fXSecs[iFile]/allHist->GetBinContent(1) << std::endl;
      }
      theHists[iFile]->Scale(fLuminosity*fXSecs[iFile]/allHist->GetBinContent(1));
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
  fDataContainer = new TreeContainer();
  fDataContainer->SetTreeName(fTreeName);
  fMCContainer = new TreeContainer();
  fMCContainer->SetTreeName(fTreeName);

  for (UInt_t iFriend = 0; iFriend != fFriends.size(); ++iFriend) {
    fDataContainer->AddFriendName(fFriends[iFriend]);
    fMCContainer->AddFriendName(fFriends[iFriend]);
  }

  SetIncludeErrorBars(true);
  std::vector<TH1D*> DataHists = GetHistList(NumXBins,XBins,false);
  SetIncludeErrorBars(false);
  std::vector<TH1D*>   MCHists = GetHistList(NumXBins,XBins,true);

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
      tempHistHolder = new HistHolder(tempMCHist,fStackEntries[iHist],fStackColors[iHist],(fForceTop == fStackEntries[iHist]));
      HistHolders.push_back(tempHistHolder);
    }
    else
      tempMCHist->Add(MCHists[iHist]);
  }

  if (fDumpRootName != "") {
    TH1D* tempHist;
    TFile* dumpFile = new TFile(fDumpRootName,"RECREATE");
    for (UInt_t iHist = 0; iHist != HistHolders.size(); ++iHist) {
      tempHist = (TH1D*) HistHolders[iHist]->fHist->Clone();
      std::cout << HistHolders[iHist]->fEntry << "  :  " << tempHist->Integral(0,NumXBins + 1,"width") << std::endl;
      dumpFile->WriteTObject(tempHist,HistHolders[iHist]->fEntry);
    }
    tempHist = (TH1D*) DataHist->Clone();
    std::cout << "Data     :  " << tempHist->Integral(0,NumXBins + 1,"width") << std::endl;
    dumpFile->WriteTObject(tempHist,"Data");
    dumpFile->Close();
  }

  std::sort(HistHolders.begin(),HistHolders.end(),SortHistHolders);

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
        AddLegendEntry(HistHolders[iLarger]->fEntry,HistHolders[iLarger]->fColor,1,1);                       // Add legend properly
      else {                                                                                                 // Otherwise
        if (HistHolders[iLarger + 1]->fHist->Integral() > 0)                                                 // Check if the next histogram contribute
          AddLegendEntry("Others",HistHolders[iLarger]->fColor,1,1);                                         // If so, make others legend
        else                                                                                                 // If not,
          AddLegendEntry(HistHolders[iLarger]->fEntry,HistHolders[iLarger]->fColor,1,1);                     // Make normal legend entry

        break;                                                                                               // Stop adding histograms
      }
    }
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
