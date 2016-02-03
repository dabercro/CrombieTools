#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1.h"

#include "LimitTreeMaker.h"

ClassImp(LimitTreeMaker)

//--------------------------------------------------------------------
LimitTreeMaker::LimitTreeMaker(TString outputName) :
  fOutputFileName(outputName),
  fTreeName("events"),
  fOutputWeightBranch("weight"),
  fAllHistName("htotal"),
  fLuminosity(2000.0)
{
  fInFileNames.resize(0);
  fOutTreeNames.resize(0);
  fXSecs.resize(0);
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
  TFile* outClear = new TFile(fOutputFileName,"RECREATE");
  outClear->Close();

  for (UInt_t iRegion = 0; iRegion != fRegionNames.size(); ++iRegion) {
    TString regionName = fRegionNames[iRegion];

    std::cout << "Region: " << regionName << std::endl;

    UInt_t numFiles = fInFileNames.size() + fExceptionFileNames[regionName].size();
    for (UInt_t iFile = 0; iFile != numFiles; ++iFile) {
      TString fileName;
      TString outTreeName;
      Float_t XSec;

      if (iFile < fInFileNames.size()) {
        outTreeName = fOutTreeNames[iFile];
        if (fExceptionSkip[regionName].find(outTreeName) != fExceptionSkip[regionName].end())
          continue;
        fileName = fInFileNames[iFile];
        XSec = fXSecs[iFile];
      }
      else {
        fileName = (fExceptionFileNames[regionName])[iFile - fInFileNames.size()];
        outTreeName = (fExceptionTreeNames[regionName])[iFile - fInFileNames.size()];
        XSec =  (fExceptionXSecs[regionName])[iFile - fInFileNames.size()];
      }
      std::cout << "File " << fileName << std::endl;
      TFile* inFile = new TFile(fileName);
      if (!inFile) {
        std::cout << "Could not open file " << fInFileNames[iFile] << std::endl;
        std::cout << "I bet you gave me the wrong name..." << std::endl;
        exit(1);
      }
      
      TTree* inTree = (TTree*) inFile->Get(fTreeName);
      if (!inFile) {
        std::cout << "Could not find tree " << fTreeName << std::endl;
        std::cout << "in file " << fInFileNames[iFile] << std::endl;
        std::cout << "I bet you gave me the wrong name..." << std::endl;
        exit(1);
      }
      
      // Apply selection
      TFile* tempFile = new TFile("tempCopyFileDontUse.root","RECREATE");
      TString theCut = fRegionCuts[iRegion];
      if (XSec < 0 && fExceptionDataCuts[regionName] != "")
        theCut = TString("(") + theCut + ") && (" + fExceptionDataCuts[regionName] + ")";

      TTree* loopTree = inTree->CopyTree(fRegionCuts[iRegion]);
      // Initialize output tree
      TFile* outFile = new TFile(fOutputFileName,"UPDATE");
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
