#include <iostream>
#include <algorithm>

#include "TFile.h"
#include "TLegend.h"

#include "HistHolder.h"
#include "PlotStack.h"

ClassImp(PlotStack)

//--------------------------------------------------------------------
PlotStack::PlotStack() :
  fTreeName("events"),
  fDataContainer(0),
  fMCContainer(0),
  fSignalContainer(0),
  fDataWeights(""),
  fMCWeights(""),
  fForceTop(""),
  fMinLegendFrac(0.0),
  fIgnoreInLinear(0.0),
  fStackLineWidth(1),
  fOthersColor(0),
  fUsingLumi(true),
  fLimitFile(0),
  fLimitRegion(""),
  fLimitTreeDir(""),
  fDebug(false),
  fDumpRootName("")
{ }

//--------------------------------------------------------------------
PlotStack::~PlotStack()
{
  if (fLimitFile)
    fLimitFile->Close();
}

//--------------------------------------------------------------------
void
PlotStack::UseLimitTree(TString limitFile, TString region, TString mcConfig, TString signalConfig)
{
  fUsingLumi = false;
  if (!limitFile.Contains('/'))
    limitFile = fLimitTreeDir + limitFile;

  fLimitFile = TFile::Open(limitFile);
  fLimitRegion = region;
  ReadMCConfig(mcConfig,FileConfigReader::kBackground);
  if (signalConfig != "")
    ReadMCConfig(signalConfig,FileConfigReader::kSignal);
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotStack::GetHistList(Int_t NumXBins, Double_t *XBins, HistType type)
{
  std::vector<FileInfo*> *theFileInfo = &fMCFileInfo;
  TString tempCutHolder = "";
  UInt_t numFiles = 0;

  if (type == kSignal)
    theFileInfo = &fSignalFileInfo;

  if (fLimitFile) {
    ResetTree();
    if (type == kData)
      AddTree((TTree*) fLimitFile->Get(TString("data_") + fLimitRegion));
    else {
      numFiles = (*theFileInfo).size();
      for (UInt_t iFile = 0; iFile != numFiles; ++iFile)
        AddTree((TTree*) fLimitFile->Get((*theFileInfo)[iFile]->fTreeName + "_" + fLimitRegion));
    }
  }
  else {
    TreeContainer *tempContainer = NULL;

    if (type == kData) {
      numFiles = fDataFileInfo.size();
      tempContainer = fDataContainer;
      for (UInt_t iFile = 0; iFile != numFiles; ++iFile)
        tempContainer->AddFile(fDataFileInfo[iFile]->fFileName);
    }
    else {
      if (type == kMC)
        tempContainer = fMCContainer;
      
      else
        tempContainer = fSignalContainer;
      
      numFiles = (*theFileInfo).size();
      for (UInt_t iFile = 0; iFile != numFiles; ++iFile)
        tempContainer->AddFile((*theFileInfo)[iFile]->fFileName);
    }
    SetTreeList(tempContainer->ReturnTreeList());
  }

  if (type == kData && fDataWeights != "") {
    tempCutHolder = fDefaultCut;
    SetDefaultWeight(TString("(") + tempCutHolder + TString(") && (") + fDataWeights + TString(")"));
  }
  else if (type != kData && fMCWeights != "") {
    tempCutHolder = fDefaultCut;
    SetDefaultWeight(TString("(") + tempCutHolder + TString(")*(") + fMCWeights + TString(")"));
  }
  std::vector<TH1D*> theHists = MakeHists(NumXBins,XBins);
  if (tempCutHolder != "")
    SetDefaultWeight(tempCutHolder);

  if (fUsingLumi && type != kData) {
    for (UInt_t iFile = 0; iFile < numFiles; iFile++)
      theHists[iFile]->Scale((*theFileInfo)[iFile]->fXSecWeight);
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
  fSignalContainer = new TreeContainer();
  fSignalContainer->SetTreeName(fTreeName);

  std::vector<TFile*> TemplateFiles;
  TFile *templateFile = NULL;

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp) {
    templateFile = TFile::Open(fTemplateFiles[iTemp]);
    TemplateFiles.push_back(templateFile);
  }

  for (UInt_t iFriend = 0; iFriend != fFriends.size(); ++iFriend) {
    fDataContainer->AddFriendName(fFriends[iFriend]);
    fMCContainer->AddFriendName(fFriends[iFriend]);
    fSignalContainer->AddFriendName(fFriends[iFriend]);
  }

  SetIncludeErrorBars(true);
  std::vector<TH1D*> DataHists = GetHistList(NumXBins,XBins,kData);
  SetIncludeErrorBars(false);
  std::vector<TH1D*> MCHists = GetHistList(NumXBins,XBins,kMC);
  std::vector<TH1D*> SignalHists;
  if (fSignalFileInfo.size() != 0)
    SignalHists = GetHistList(NumXBins,XBins,kSignal);

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
    if (fMCFileInfo[iHist]->fEntry != previousEntry) {
      previousEntry = fMCFileInfo[iHist]->fEntry;
      TString tempName;
      tempName.Format("StackedHist_%d",iHist);
      tempMCHist = (TH1D*) MCHists[iHist]->Clone(tempName);
      tempHistHolder = new HistHolder(tempMCHist,fMCFileInfo[iHist]->fEntry,fMCFileInfo[iHist]->fColorStyle,
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
        TH1D *toFormat = (TH1D*) templateHist->Rebin(NumXBins,"",XBins);
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
    AllHists.push_back(SignalHists[iHist]);
    AddLegendEntry(fSignalFileInfo[iHist]->fEntry,1,2,fSignalFileInfo[iHist]->fColorStyle);
  }

  BaseCanvas(AddOutDir(FileBase),AllHists,XLabel,YLabel,logY);

  for (UInt_t iHist = 0; iHist != AllHists.size(); ++iHist)
    delete AllHists[iHist];
  for (UInt_t iHist = 0; iHist != MCHists.size(); ++iHist)
    delete MCHists[iHist];
  for (UInt_t iHist = 0; iHist != DataHists.size(); ++iHist)
    delete DataHists[iHist];

  delete fDataContainer;
  delete fMCContainer;
  delete fSignalContainer;

  for (UInt_t iTemp = 0; iTemp != fTemplateEntries.size(); ++iTemp)
    TemplateFiles[iTemp]->Close();
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
