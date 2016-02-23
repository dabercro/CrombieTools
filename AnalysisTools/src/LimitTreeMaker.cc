#include <iostream>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1.h"

#include "LimitTreeMaker.h"

ClassImp(LimitTreeMaker)

//--------------------------------------------------------------------
LimitTreeMaker::LimitTreeMaker(TString outputName) :
  fReportFrequency(10),
  fOutDirectory(""),
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
LimitTreeMaker::MakeTrees()
{
  TFile* outClear = new TFile(fOutDirectory + fOutputFileName,"RECREATE");
  outClear->Close();

  for (UInt_t iRegion = 0; iRegion != fRegionNames.size(); ++iRegion) {
    TString regionName = fRegionNames[iRegion];

    UInt_t numFiles = fMCFileInfo.size() + fSignalFileInfo.size() + fExceptionFileNames[regionName].size();
    UInt_t sumofMC = fMCFileInfo.size() + fSignalFileInfo.size();
    for (UInt_t iFile = 0; iFile != numFiles; ++iFile) {
      try  {
        if ((Int_t) iFile % fReportFrequency == 0) {
          std::cout << fOutputFileName << " : ";
          for (UInt_t jRegion = 0; jRegion != fRegionNames.size(); ++jRegion) {
            if (jRegion == iRegion)
              std::cout << "[[" << fRegionNames[jRegion] << "]], ";
            else
              std::cout << fRegionNames[jRegion] << ", ";
          }
          std::cout << "   " << iFile << " / " << numFiles << " Files." << std::endl;
        }
      }
      catch (...) {
        std::cout << "Exception occured on file " << iFile << std::endl;
      }
      TString fileName;
      TString outTreeName;
      Float_t XSec;

      if (iFile < fMCFileInfo.size()) {
        outTreeName = fMCFileInfo[iFile]->fTreeName;
        if (fExceptionSkip[regionName].find(outTreeName) != fExceptionSkip[regionName].end())
          continue;
        fileName = fMCFileInfo[iFile]->fFileName;
        XSec = fMCFileInfo[iFile]->fXSec;
      }
      else if (iFile < sumofMC) {
        outTreeName = fSignalFileInfo[iFile - fMCFileInfo.size()]->fTreeName;
        if (fExceptionSkip[regionName].find(outTreeName) != fExceptionSkip[regionName].end())
          continue;
        fileName = fSignalFileInfo[iFile - fMCFileInfo.size()]->fFileName;
        XSec = fSignalFileInfo[iFile - fMCFileInfo.size()]->fXSec;
      }
      else {
        fileName = (fExceptionFileNames[regionName])[iFile - sumofMC];
        outTreeName = (fExceptionTreeNames[regionName])[iFile - sumofMC];
        XSec =  (fExceptionXSecs[regionName])[iFile - sumofMC];
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
      TFile* outFile = new TFile(fOutDirectory + fOutputFileName,"UPDATE");
      TTree* outTree = new TTree(outTreeName + "_" + regionName,outTreeName + "_" + regionName);
      
      // Setup the branches to keep
      std::map<TString, Float_t> addresses;
      for (UInt_t iKeep = 0; iKeep != fKeepBranches.size(); ++iKeep) {
        addresses[fKeepBranches[iKeep]] = 0.0;
        outTree->Branch(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]],fKeepBranches[iKeep] + "/F");
        loopTree->SetBranchAddress(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]]);
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
    }
  }
}
