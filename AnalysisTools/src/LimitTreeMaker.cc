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

  for (UInt_t iFile = 0; iFile != fInFileNames.size(); ++iFile) {
    std::cout << "File " << fInFileNames[iFile] << std::endl;
    TFile* inFile = new TFile(fInFileNames[iFile]);
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

    for (UInt_t iRegion = 0; iRegion != fRegionNames.size(); ++iRegion) {
      std::cout << "Region: " << fRegionNames[iRegion] << std::endl;
      inFile->cd();
      // Apply selection
      TFile* tempFile = new TFile("tempCopyFileDontUse.root","RECREATE");
      TTree* loopTree = inTree->CopyTree(fRegionCuts[iRegion]);
      // Initialize output tree
      TFile* outFile = new TFile(fOutputFileName,"UPDATE");
      TTree* outTree = new TTree(fOutTreeNames[iFile] + "_" + fRegionNames[iRegion],fOutTreeNames[iFile] + "_" + fRegionNames[iRegion]);

      // Setup the branches to keep
      std::map<TString, Float_t> addresses;
      for (UInt_t iKeep = 0; iKeep != fKeepBranches.size(); ++iKeep) {
        addresses[fKeepBranches[iKeep]] = 0.0;
        outTree->Branch(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]],fKeepBranches[iKeep] + "/F");
        loopTree->SetBranchAddress(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]]);
      }
      
      // Get the all histogram and calculate x-section weight
      TH1* allHist = (TH1*) inFile->Get(fAllHistName);
      Float_t mcScale = fLuminosity*fXSecs[iFile]/(allHist->GetBinContent(1));
      // Setup the output weight branch
      Float_t mcWeight = 1.0;
      TBranch* weightBranch = outTree->Branch(fOutputWeightBranch,&mcWeight,fOutputWeightBranch+"/F");
      // Get branches for weights
      for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight) {
        addresses[fWeightBranch[iWeight]];
        loopTree->SetBranchAddress(fWeightBranch[iWeight],&addresses[fWeightBranch[iWeight]]);
      }

      UInt_t nentries = loopTree->GetEntries();
      for (UInt_t iEntry = 0; iEntry != nentries; ++iEntry) {
        loopTree->GetEntry(iEntry);
        if (mcScale > 0) {
          mcWeight = mcScale;
          for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight)
            mcWeight *= addresses[fWeightBranch[iWeight]];
        }
        else
          mcWeight = 1.0;

        outTree->Fill();
      }
      outFile->cd();
      outFile->WriteTObject(outTree,fOutTreeNames[iFile] + "_" + fRegionNames[iRegion],"Overwrite");
      outFile->Close();

      tempFile->Close();
      addresses.clear();
    }
    inFile->Close();
  }
}
