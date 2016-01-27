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
  TFile* theFile = new TFile(fOutputFileName,"UPDATE");
  theFile->cd();
  for (UInt_t iRegion = 0; iRegion != fRegionNames.size(); ++iRegion) {
    TreeContainer* tempContainer = new TreeContainer();
    tempContainer->SetTreeName(fTreeName);
    for (UInt_t iFriend = 0; iFriend != fFriendNames.size(); ++iFriend)
      tempContainer->AddFriendName(fFriendNames[iFriend]);
    for (UInt_t iKeep = 0; iKeep != fKeepBranches.size(); ++iKeep)
      tempContainer->AddKeepBranch(fKeepBranches[iKeep]);
    for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight)
      tempContainer->AddKeepBranch(fWeightBranch[iWeight]);

    tempContainer->SetSkimmingCut(fRegionCuts[iRegion]);
    for (UInt_t iFile = 0; iFile != fInFileNames.size(); ++iFile)
      tempContainer->AddFile(fInFileNames[iFile]);

    std::vector<TTree*> tempTrees = tempContainer->ReturnTreeList();
    std::vector<TFile*> theFiles  = tempContainer->ReturnFileList();
    for (UInt_t iTree = 0; iTree != tempTrees.size(); ++iTree) {
      TTree* tempTree = tempTrees[iTree];

      TH1* allHist = (TH1*) theFiles[iTree]->Get(fAllHistName);
      Float_t mcScale = fLuminosity*fXSecs[iTree]/(allHist->GetBinContent(1));
      Float_t mcWeight = 1.0;
      TBranch* weightBranch = tempTree->Branch(fOutputWeightBranch,&mcWeight,fOutputWeightBranch+"/F");
      std::vector<TTreeFormula*> weightsForm;
      TTreeFormula* tempForm;
      for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight) {
        tempForm = new TTreeFormula(fWeightBranch[iWeight],fWeightBranch[iWeight],tempTree);
        weightsForm.push_back(tempForm);
      }

      UInt_t nentries = tempTree->GetEntriesFast();
      for (UInt_t iEntry = 0; iEntry != nentries; ++iEntry) {
        tempTree->GetEntry(iEntry);
        mcWeight = mcScale;
        for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight)
          mcWeight *= weightsForm[iWeight]->EvalInstance();
        
        weightBranch->Fill();
      }
      for (UInt_t iWeight = 0; iWeight != fWeightBranch.size(); ++iWeight) {
        delete weightsForm[iWeight];
        tempTree->SetBranchStatus(fWeightBranch[iWeight],0);
      }

      theFile->cd();
      tempTree->Write(fOutTreeNames[iTree] + "_" + fRegionNames[iTree],TObject::kOverwrite);
    }
    delete tempContainer;
  }
  theFile->Close();
}
