#include <iostream>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "CorrectorApplicator.h"

ClassImp(CorrectorApplicator)

//--------------------------------------------------------------------
CorrectorApplicator::CorrectorApplicator(TString name, Bool_t saveAll) :
  fName(name),
  fSaveAll(saveAll),
  fInputTreeName("events"),
  fOutputTreeName("corrections"),
  fReportFrequency(100000)
{
  fCorrectors.resize(0);
}
  
//--------------------------------------------------------------------
CorrectorApplicator::~CorrectorApplicator()
{ }

//--------------------------------------------------------------------
void
CorrectorApplicator::ApplyCorrections(TString fileName)
{
  TFile* theFile = new TFile(fileName,"UPDATE");
  TTree* theTree = (TTree*) theFile->Get(fInputTreeName);
  TTree* outTree;
  // If the input and output trees are the same, check if branches already exist
  if (fInputTreeName == fOutputTreeName) {
    if (fName != "") {
      if (!theTree->GetBranch(fName)) {
        std::cout << "Branch " << fName << " already exists in " << fileName << std::endl;
        theFile->Close();
        exit(1);
      }
    }
    if (fSaveAll) {
      for (UInt_t iCorrector = 0; iCorrector != fCorrectors.size(); ++iCorrector) {
        if (!theTree->GetBranch(fCorrectors[iCorrector]->GetName())) {
          std::cout << "Branch " << fCorrectors[iCorrector]->GetName() << " already exists in " << fileName << std::endl;
          theFile->Close();
          exit(1);
        }
        if (fCorrectors[iCorrector]->GetName() == fName) {
          std::cout << "Corrector has same name as merged name: " << fName << std::endl;
          theFile->Close();
          exit(2);
        }
      }
    }
    // If the branches don't exist, everything is good to go
    outTree = theTree;
  }
  else
    outTree = new TTree(fOutputTreeName,fOutputTreeName);

  // Now set the correctors and make branches for them
  std::vector<TBranch*> fillBranches;
  std::map<TString, Float_t> Addresses;
  if (fName != "") {
    Addresses[fName] = 1.0;
    fillBranches.push_back(outTree->Branch(fName,&Addresses[fName],fName+"/F"));
  }
  for (UInt_t iCorrector = 0; iCorrector != fCorrectors.size(); ++iCorrector) {
    fCorrectors[iCorrector]->SetInTree(theTree);
    if (fSaveAll) {
      TString checkName = fCorrectors[iCorrector]->GetName();
      if (!outTree->GetBranch(checkName)) {
        Addresses[checkName] = 1.0;
        fillBranches.push_back(outTree->Branch(checkName,&Addresses[checkName],checkName+"/F"));
      }
    }
  }

  // Now loop through the tree and apply the corrections
  Long64_t nentries = theTree->GetEntriesFast();
  Float_t tempValue = 1.0;
  for (Long64_t iEntry = 0; iEntry != nentries; ++iEntry) {
    theTree->GetEntry(iEntry);
    // First reset all of the addressed floats to 1.0
    if (fName != "")
      Addresses[fName] = 1.0;
    if (fSaveAll) {
      for (UInt_t iCorrector = 0; iCorrector != fCorrectors.size(); ++iCorrector)
        Addresses[fCorrectors[iCorrector]->GetName()] = 1.0;
    }
    
    // Evaluate the correctors and save the factor values
    for (UInt_t iCorrector = 0; iCorrector != fCorrectors.size(); ++iCorrector) {
      tempValue = fCorrectors[iCorrector]->Evaluate();
      if (fName != "")
        Addresses[fName] *= tempValue;
      if (fSaveAll)
        Addresses[fCorrectors[iCorrector]->GetName()] *= tempValue;
    }

    // Fill all the branches
    for (UInt_t iBranch = 0; iBranch != fillBranches.size(); ++iBranch)
      fillBranches[iBranch]->Fill();
  }

  theFile->cd();
  if (fOutputTreeName == fInputTreeName)
    outTree->Write();
  else
    outTree->Write(fOutputTreeName,TObject::kOverwrite);
  theFile->Close();

}
