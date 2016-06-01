/**
  @file   XSecAdder.cc
  Describes the XSecAdder class.
  @author Daniel Abercrombie <dabercro@mit.edu> */

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "XSecAdder.h"

ClassImp(XSecAdder)

//--------------------------------------------------------------------
XSecAdder::XSecAdder(TString name) :
  fBranchName(name)
{ 
  SetKeepAllFiles(true);
  SetMultiplyLumi(false);
}
  
//--------------------------------------------------------------------
XSecAdder::~XSecAdder()
{ }

//--------------------------------------------------------------------
void XSecAdder::AddXSecs()
{
  for (std::vector<FileInfo*>::iterator iInfo = fMCFileInfo.begin(); iInfo != fMCFileInfo.end(); ++iInfo) {

    TFile *file = new TFile((*iInfo)->fFileName,"UPDATE");
    if (!file)
      exit(0);

    TTree *tree = (TTree*) file->Get(fInTreeName);
    TTree *outTree;

    if (fOutTreeName != fInTreeName)
      outTree = new TTree(fOutTreeName, fOutTreeName);
    else
      outTree = tree;

    Int_t nentries = tree->GetEntries();
    Float_t weight = (*iInfo)->fXSecWeight;

    if (fOutTreeName != fInTreeName || (!tree->GetBranch(fBranchName))) {

      Float_t XSecWeight = 1.;
      TBranch *XSecWeightBr = outTree->Branch(fBranchName, &XSecWeight,fBranchName + "/F");
      
      for (int entry = 0; entry < nentries; entry++) {
        if (entry % 500000 == 0)  /// @todo Make a ReportFrequency class
          std::cout << "Processing... " << float(entry)/float(nentries)*100 << "%" << std::endl;
        XSecWeight =  weight;
        if (fOutTreeName == fInTreeName)
          XSecWeightBr->Fill();
        else
          outTree->Fill();
      }
      if (fOutTreeName == fInTreeName)
        outTree->Write();
      else
        outTree->Write(fOutTreeName, TObject::kOverwrite);
    }
    
    file->Close();
  }
}