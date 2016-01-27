#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreeFormula.h"
#include "TH1.h"

#include "TreeContainer.h"
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
LimitTreeMaker::MakeTree()
{
  TFile* theFile = new TFile(fOutputFileName,"RECREATE");
  TreeContainer* tempContainer = new TreeContainer();
  for (UInt_t iFile = 0; iFile != fInFileNames.size(); ++iFile)
    tempContainer->AddFile(fInFileNames[iFile]);
  std::vector<TFile*> theFiles  = tempContainer->ReturnFileList();

  for (UInt_t iRegion = 0; iRegion != fRegionNames.size(); ++iRegion) {
    std::cout << "Region " << fRegionNames[iRegion] << std::endl;
    for (UInt_t iTree = 0; iTree != theFiles.size(); ++iTree) {
      std::cout << "File " << theFiles[iTree]->GetName() << std::endl;
      theFile->cd();
      TTree* outTree  = new TTree(fOutTreeNames[iTree] + "_" + fRegionNames[iTree],fOutTreeNames[iTree] + "_" + fRegionNames[iTree]);
      TTree* tempTree   = (TTree*) theFiles[iTree]->Get("events");
      TTree* weightTree = (TTree*) theFiles[iTree]->Get("corrections");
      std::cout << tempTree << " ... " << weightTree << std::endl;
      std::map<TString, Float_t> addresses;
      TTreeFormula* cutForm = new TTreeFormula(fRegionCuts[iRegion],fRegionCuts[iRegion],tempTree);
      for (UInt_t iKeep = 0; iKeep != fKeepBranches.size(); ++iKeep) {
        addresses[fKeepBranches[iKeep]] = 0.0;
        outTree->Branch(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]],fKeepBranches[iKeep] + "/F");
        tempTree->SetBranchAddress(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]]);
        std::cout << "Added " << fKeepBranches[iKeep] << std::endl;
      }

      TH1* allHist = (TH1*) theFiles[iTree]->Get(fAllHistName);
      Float_t mcScale = fLuminosity*fXSecs[iTree]/(allHist->GetBinContent(1));
      Float_t mcWeight = 1.0;
      TBranch* weightBranch = outTree->Branch(fOutputWeightBranch,&mcWeight,fOutputWeightBranch+"/F");
      TTreeFormula* tempForm;
      for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight) {
        addresses[fWeightBranch[iWeight]];
        weightTree->SetBranchAddress(fWeightBranch[iWeight],&addresses[fWeightBranch[iWeight]]);
      }

      std::cout << "About to cycle through trees" << std::endl;
      UInt_t nentries = tempTree->GetEntriesFast();
      for (UInt_t iEntry = 0; iEntry != nentries; ++iEntry) {
        tempTree->GetEntry(iEntry);
        weightTree->GetEntry(iEntry);
        if (mcScale > 0) {
          mcWeight = mcScale;
          for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight)
            mcWeight *= addresses[fWeightBranch[iWeight]];
        }
        else
          mcWeight = 1.0;

        if (cutForm->EvalInstance())
          outTree->Fill();
      }

      theFile->cd();
      outTree->Write(fOutTreeNames[iTree] + "_" + fRegionNames[iRegion],TObject::kOverwrite);
      std::cout << "Wrote to file." << std::endl;
    }
  }
  theFile->Close();
  delete tempContainer;
}
