#include <iostream>
#include <cstdlib>
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
LimitTreeMaker::ReadExceptionConfig(TString config, TString region, TString fileDir)
{
  if (fileDir != "")
    SetInDirectory(fileDir);

  std::ifstream configFile;
  configFile.open(config.Data());
  TString LimitTreeName;
  TString FileName;
  TString XSec;
  TString LegendEntry;
  TString ColorStyleEntry; 

  while (!configFile.eof()) {
    configFile >> LimitTreeName >> FileName;
    if (LimitTreeName == "skip") {
      ExceptionSkip(region,AddInDir(FileName));
    }
    else {
      configFile >> XSec >> LegendEntry >> ColorStyleEntry;
      ExceptionAdd(region,AddInDir(FileName),LimitTreeName,XSec.Atof());
      if (ColorStyleEntry == "rgb") {
        for (Int_t iColor = 0; iColor != 3; ++iColor)
          configFile >> ColorStyleEntry;
      }
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

    UInt_t numData = fDataFileInfo.size();
    UInt_t numDataBackground = numData + fMCFileInfo.size();
    UInt_t sumOfStandard = numDataBackground + fSignalFileInfo.size();
    UInt_t numFiles = sumOfStandard + fExceptionFileNames[regionName].size();
    for (UInt_t iFile = 0; iFile != numFiles; ++iFile) {
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
        outTreeName = fDataFileInfo[iFile]->fTreeName;
        fileName = fDataFileInfo[iFile]->fFileName;
        XSec = fDataFileInfo[iFile]->fXSec;
      }
      else if (iFile < numDataBackground) {
        outTreeName = fMCFileInfo[iFile - numData]->fTreeName;
        fileName = fMCFileInfo[iFile - numData]->fFileName;
        XSec = fMCFileInfo[iFile - numData]->fXSec;
      }
      else if (iFile < sumOfStandard) {
        outTreeName = fSignalFileInfo[iFile - numDataBackground]->fTreeName;
        fileName = fSignalFileInfo[iFile - numDataBackground]->fFileName;
        XSec = fSignalFileInfo[iFile - numDataBackground]->fXSec;
      }
      else {
        fileName = (fExceptionFileNames[regionName])[iFile - sumOfStandard];
        outTreeName = (fExceptionTreeNames[regionName])[iFile - sumOfStandard];
        XSec =  (fExceptionXSecs[regionName])[iFile - sumOfStandard];
      }

      if (iFile < sumOfStandard) {
        if (fExceptionSkip[regionName].find(outTreeName) != fExceptionSkip[regionName].end())
          continue;
        if (fExceptionSkip[regionName].find(fileName) != fExceptionSkip[regionName].end())
          continue;
      }

      TFile* inFile = new TFile(fileName);
      if (!inFile) {
        std::cout << "Could not open file " << fileName << std::endl;
        exit(1);
      }
      
      TTree* inTree = (TTree*) inFile->Get(fTreeName);
      if (!inTree) {
        std::cout << "Could not find tree " << fTreeName << std::endl;
        std::cout << "in file " << inFile << std::endl;
        inFile->Close();
        exit(1);
      }
      
      // Apply selection
      TFile* tempFile = new TFile(TString("/tmp/") + getenv("USER") + "/" + fOutputFileName + ".tempCopyFileDontUse.root","RECREATE");
      TString theCut = fRegionCuts[iRegion];
      if (XSec < 0 && fExceptionDataCuts[regionName] != "")
        theCut = TString("(") + theCut + ") && (" + fExceptionDataCuts[regionName] + ")";

      TTree* loopTree = inTree->CopyTree(fRegionCuts[iRegion]);
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
