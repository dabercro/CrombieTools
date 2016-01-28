#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreeFormula.h"
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
LimitTreeMaker::MakeTree()
{
  TFile* theFile = new TFile(fOutputFileName,"RECREATE");

  for (UInt_t iFile = 0; iFile != fInFileNames.size(); ++iFile) {
    std::cout << "File " << fInFileNames[iFile] << std::endl;
    TFile* inFile = new TFile(fInFileNames[iFile]);
    TTree* tempTree   = (TTree*) inFile->Get("events");
    TTree* weightTree = (TTree*) inFile->Get("events");
    std::cout << tempTree << " ... " << weightTree << std::endl;
    theFile->cd();
    for (UInt_t iRegion = 0; iRegion != fRegionNames.size(); ++iRegion) {
      std::map<TString, Float_t> addresses;
      std::cout << "Region " << fRegionNames[iRegion] << std::endl;
      TTree* outTree  = new TTree(fOutTreeNames[iFile] + "_" + fRegionNames[iFile],fOutTreeNames[iFile] + "_" + fRegionNames[iFile]);
      std::cout << outTree << std::endl;
      TTreeFormula* cutForm = new TTreeFormula(fRegionCuts[iRegion],fRegionCuts[iRegion],tempTree);
      std::cout << cutForm << std::endl;
      for (UInt_t iKeep = 0; iKeep != fKeepBranches.size(); ++iKeep) {
        std::cout << outTree << " ... " << tempTree << std::endl;
        addresses[fKeepBranches[iKeep]] = 0.0;
        outTree->Branch(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]],fKeepBranches[iKeep] + "/F");
        tempTree->SetBranchAddress(fKeepBranches[iKeep],&addresses[fKeepBranches[iKeep]]);
        std::cout << "Added " << fKeepBranches[iKeep] << std::endl;
      }
      
      TH1* allHist = (TH1*) inFile->Get(fAllHistName);
      Float_t mcScale = fLuminosity*fXSecs[iFile]/(allHist->GetBinContent(1));
      Float_t mcWeight = 1.0;
      TBranch* weightBranch = outTree->Branch(fOutputWeightBranch,&mcWeight,fOutputWeightBranch+"/F");
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
      outTree->Write(fOutTreeNames[iFile] + "_" + fRegionNames[iRegion],TObject::kOverwrite);

      std::cout << "Wrote to file." << std::endl;
      addresses.clear();
      delete cutForm;
    }
    inFile->Close();
  }
  theFile->Close();
}
