#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
#include "TTreeFormula.h"

#include "FlatSkimmer.h"

ClassImp(FlatSkimmer)

//--------------------------------------------------------------------
FlatSkimmer::FlatSkimmer() :
  fGoodLumiFilter(NULL),
  fInDirectory("."),
  fOutDirectory("."),
  fCut("1"),
  fTreeName("events"),
  fRunExpr("runNum"),
  fLumiExpr("lumiNum"),
  fReportFreq(100000)
{
  fCopyObjects.resize(0);
}

//--------------------------------------------------------------------
void
FlatSkimmer::Slim(TString fileName)
{
  TFile *inFile = TFile::Open(fInDirectory + "/" + fileName);
  TTree *inTree = (TTree*) inFile->Get(fTreeName);
  TTreeFormula *cutter = new TTreeFormula("cutter",fCut,inTree);

  Int_t runNum  = 0;
  Int_t lumiNum = 0;
  inTree->SetBranchAddress(fRunExpr,&runNum);
  inTree->SetBranchAddress(fLumiExpr,&lumiNum);

  TFile *outFile =  new TFile(fOutDirectory + "/" + fileName,"RECREATE");
  TTree *outTree = inTree->CloneTree(0);

  for (Int_t iEntry = 0; iEntry != inTree->GetEntriesFast(); ++iEntry) {
    if (iEntry % fReportFreq == 0)
      std::cout << float(iEntry)/inTree->GetEntriesFast() << std::endl;
    inTree->GetEntry(iEntry);
    if (fGoodLumiFilter->IsGood(runNum,lumiNum) && cutter->EvalInstance())
      outTree->Fill();
  }
  outFile->WriteTObject(outTree,fTreeName);
  for (UInt_t iObj = 0; iObj != fCopyObjects.size(); ++iObj) {
    if (inFile->Get(fCopyObjects[iObj]))
      outFile->WriteTObject(inFile->Get(fCopyObjects[iObj])->Clone());
    else
      std::cout << "Could not find " << fCopyObjects[iObj] << " in " << fileName << std::endl;
  }

  outFile->Close();
  inFile->Close();
}
