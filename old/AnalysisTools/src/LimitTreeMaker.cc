#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1.h"

#include "LimitTreeMaker.h"

ClassImp(LimitTreeMaker)

//--------------------------------------------------------------------
LimitTreeMaker::LimitTreeMaker(TString outputName) :
  fReportFrequency(10000),
  fOutputFileName(outputName),
  fTreeName("events"),
  fOutputWeightBranch("weight")
{
  fFriendNames.resize(0);
  fKeepBranches.resize(0);
  fWeightBranch.resize(0);
  fRegionNames.resize(0);
  fRegionCuts.resize(0);
}

//--------------------------------------------------------------------
LimitTreeMaker::~LimitTreeMaker()
{ }

//--------------------------------------------------------------------
void
LimitTreeMaker::ReadExceptionConfig(const char* config, TString region, TString fileDir)
{
  if (fileDir != "")
    SetInDirectory(fileDir);

  Message(eInfo, "Opening %s for region %s", config, region.Data());

  std::ifstream configFile;
  configFile.open(config);
  TString LimitTreeName;
  TString FileName;
  TString XSec;
  TString LegendEntry;
  TString ColorStyleEntry;

  while (!configFile.eof()) {
    configFile >> LimitTreeName >> FileName;

    if (configFile.eof())
      continue;

    if (LimitTreeName == "skip") {
      Message(eDebug, "Going to skip %s in region %s", FileName.Data(), region.Data());
      ExceptionSkip(region, AddInDir(FileName));
    }
    else {
      Message(eDebug, "Adding %s as %s", FileName.Data(), LimitTreeName.Data());
      configFile >> XSec >> LegendEntry >> ColorStyleEntry;
      ExceptionAdd(region, AddInDir(FileName), LimitTreeName, XSec.Atof());
      if (ColorStyleEntry == "rgb") {
        for (Int_t iColor = 0; iColor != 3; ++iColor)
          configFile >> ColorStyleEntry;
      }
      Message(eDebug, "Now have %i exceptions", fExceptionFileNames[region].size());
    }
  }
  configFile.close();
}

//--------------------------------------------------------------------
void
LimitTreeMaker::MakeTrees()
{
  if (fReportFrequency < 1)
    fReportFrequency = 10000;
  TFile* outClear = new TFile(AddOutDir(fOutputFileName),"RECREATE");
  outClear->Close();

  for (UInt_t iRegion = 0; iRegion != fRegionNames.size(); ++iRegion) {
    TString regionName = fRegionNames[iRegion];

    Message(eInfo, "Entering region: %s", regionName.Data());

    UInt_t numData = fDataFileInfo.size();
    UInt_t numDataBackground = numData + fMCFileInfo.size();
    UInt_t sumOfStandard = numDataBackground + fSignalFileInfo.size();
    UInt_t numFiles = sumOfStandard + fExceptionFileNames[regionName].size();

    Message(eInfo, "Number of Data Files: %i", numData);
    Message(eInfo, "Number of Background Files: %i", numDataBackground);
    Message(eInfo, "Number of Signal Files: %i", fSignalFileInfo.size());
    Message(eInfo, "Number of Standard Files (sum of previous): %i", sumOfStandard);
    Message(eInfo, "Number of files in this region (adding exception): %i", numFiles);

    for (UInt_t iFile = 0; iFile != numFiles; ++iFile) {
      Message(eDebug, "Starting file %i/%i", iFile,  numFiles);

      if (iFile % fReportFrequency == 0 && fReportFrequency < numFiles) {
        std::cout << fOutputFileName << " : ";
        for (UInt_t jRegion = 0; jRegion != fRegionNames.size(); ++jRegion) {
          if (jRegion == iRegion)
            std::cout << "[[" << fRegionNames[jRegion] << "]], ";
          else
            std::cout << fRegionNames[jRegion] << ", ";
        }
        std::cout << "   " << iFile << " / " << numFiles << " Files." << std::endl;
      }

      TString fileName;
      TString outTreeName;
      Float_t XSec;

      if (iFile < numData) {
        Message(eDebug, "Opening data file");
        outTreeName = fDataFileInfo[iFile]->fTreeName;
        fileName = fDataFileInfo[iFile]->fFileName;
        XSec = fDataFileInfo[iFile]->fXSec;
      }
      else if (iFile < numDataBackground) {
        Message(eDebug, "Opening background file");
        outTreeName = fMCFileInfo[iFile - numData]->fTreeName;
        fileName = fMCFileInfo[iFile - numData]->fFileName;
        XSec = fMCFileInfo[iFile - numData]->fXSec;
      }
      else if (iFile < sumOfStandard) {
        Message(eDebug, "Opening signal file");
        outTreeName = fSignalFileInfo[iFile - numDataBackground]->fTreeName;
        fileName = fSignalFileInfo[iFile - numDataBackground]->fFileName;
        XSec = fSignalFileInfo[iFile - numDataBackground]->fXSec;
      }
      else {
        Message(eDebug, "Opening exception file");
        fileName = (fExceptionFileNames[regionName])[iFile - sumOfStandard];
        outTreeName = (fExceptionTreeNames[regionName])[iFile - sumOfStandard];
        XSec =  (fExceptionXSecs[regionName])[iFile - sumOfStandard];
      }

      Message(eDebug, "Name: %s, Out Tree: %s, Cross Section: %f",
              fileName.Data(), outTreeName.Data(), XSec);

      if (iFile < sumOfStandard) {
        if (fExceptionSkip[regionName].find(outTreeName) != fExceptionSkip[regionName].end()) {
          Message(eInfo, "Found %s in skip list for Region %s... Skipping.",
                  outTreeName.Data(), regionName.Data());
          continue;
        }
        if (fExceptionSkip[regionName].find(fileName) != fExceptionSkip[regionName].end()) {
          Message(eInfo, "Found %s in skip list for Region %s... Skipping.",
                  fileName.Data(), regionName.Data());
          continue;
        }
      }

      TFile* inFile = new TFile(fileName);
      if (!inFile) {
        Message(eError, "Could not open file %s", fileName.Data());
        exit(1);
      }

      TTree* inTree = (TTree*) inFile->Get(fTreeName);
      if (!inTree) {
        Message(eError, "Could not find tree %s\nin file %s", fTreeName.Data(), fileName.Data());
        inFile->Close();
        exit(1);
      }

      // Apply selection
      TFile* tempFile = new TFile(TString("/tmp/") + getenv("USER") + "/" +
                                  fOutputFileName + ".tempCopyFileDontUse.root","RECREATE");
      TString theCut = fRegionCuts[iRegion];
      if (XSec < 0 && fExceptionDataCuts[regionName] != "")
        theCut = TString("(") + theCut + ") && (" + fExceptionDataCuts[regionName] + ")";

      TTree* loopTree = inTree->CopyTree(theCut);
      // Initialize output tree
      TFile* outFile = new TFile(AddOutDir(fOutputFileName),"UPDATE");
      TTree* outTree = new TTree(outTreeName + "_" + regionName,outTreeName + "_" + regionName);

      // Setup the branches to keep
      std::map<TString, Float_t> addresses;
      std::map<TString, Int_t>   intAddresses;
      for (UInt_t iKeep = 0; iKeep != fKeepBranches.size(); ++iKeep) {
        if (fKeepBranchIsInt[iKeep]) {
          intAddresses[fKeepBranches[iKeep]] = 0.0;
          outTree->Branch(fKeepBranches[iKeep],&intAddresses[fKeepBranches[iKeep]],fKeepBranches[iKeep] + "/F");
          loopTree->SetBranchAddress(fKeepBranches[iKeep],&intAddresses[fKeepBranches[iKeep]]);
        }
        else {
          addresses[fKeepBranches[iKeep]] = 0.0;
          outTree->Branch(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]],fKeepBranches[iKeep] + "/F");
          loopTree->SetBranchAddress(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]]);
        }
      }

      // Get the all histogram and calculate x-section weight
      TH1* allHist = (TH1*) inFile->Get(fAllHistName);
      Float_t mcScale = fLuminosity*XSec/(allHist->GetBinContent(1));
      // Setup the output weight branch
      Float_t mcWeight = 1.0;
      TBranch* weightBranch = outTree->Branch(fOutputWeightBranch,&mcWeight,fOutputWeightBranch+"/F");
      // Get branches for weights 
     for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight) {
        addresses[fWeightBranch[iWeight]] = 0.0;
        loopTree->SetBranchAddress(fWeightBranch[iWeight],&addresses[fWeightBranch[iWeight]]);
      }
      for (UInt_t iWeight = 0; iWeight != fExceptionWeightBranches[regionName].size(); ++iWeight) {
        addresses[(fExceptionWeightBranches[regionName])[iWeight]] = 0.0;
        loopTree->SetBranchAddress((fExceptionWeightBranches[regionName])[iWeight],
                                   &addresses[(fExceptionWeightBranches[regionName])[iWeight]]);
      }

      UInt_t nentries = loopTree->GetEntries();
      for (UInt_t iEntry = 0; iEntry != nentries; ++iEntry) {
        loopTree->GetEntry(iEntry);
        if (mcScale > 0) {
          mcWeight = mcScale;
          for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight)
            mcWeight *= addresses[fWeightBranch[iWeight]];
          for (UInt_t iWeight = 0; iWeight != fExceptionWeightBranches[regionName].size(); ++iWeight)
            mcWeight *= addresses[(fExceptionWeightBranches[regionName])[iWeight]];
        }
        else
          mcWeight = 1.0;

        outTree->Fill();
      }
      outFile->cd();
      outFile->WriteTObject(outTree,outTreeName + "_" + regionName,"Overwrite");
      outFile->Close();

      tempFile->Close();
      inFile->Close();
      addresses.clear();
      intAddresses.clear();
    }
  }
}
