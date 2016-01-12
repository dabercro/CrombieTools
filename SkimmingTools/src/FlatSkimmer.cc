#include <iostream>
#include <set>

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
  fEventExpr("eventNum"),
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
  Int_t eventNum = 0;
  inTree->SetBranchAddress(fRunExpr,&runNum);
  inTree->SetBranchAddress(fLumiExpr,&lumiNum);
  inTree->SetBranchAddress(fEventExpr,&eventNum);

  TFile *outFile =  new TFile(fOutDirectory + "/" + fileName,"RECREATE");
  TTree *outTree = inTree->CloneTree(0);

  std::set<TString> eventsRecorded;
  TString testString = "";

  Int_t badlumis   = 0;
  Int_t cutevents  = 0;
  Int_t duplicates = 0;

  for (Int_t iEntry = 0; iEntry != inTree->GetEntriesFast(); ++iEntry) {
    if (iEntry % fReportFreq == 0)
      std::cout << float(iEntry)/inTree->GetEntriesFast() << std::endl;
    inTree->GetEntry(iEntry);
    if (fGoodLumiFilter->IsGood(runNum,lumiNum)) {
      if (cutter->EvalInstance()) {
        testString = TString::Format("%i:%i:%i",runNum,lumiNum,eventNum);
        if (eventsRecorded.find(testString) == eventsRecorded.end()) {
          outTree->Fill();
          eventsRecorded.insert(testString);
        }
        else
          duplicates++;
      }
      else
        cutevents++;
    }
    else
      badlumis++;
  }
  outFile->WriteTObject(outTree,fTreeName,"Overwrite");
  for (UInt_t iObj = 0; iObj != fCopyObjects.size(); ++iObj) {
    if (inFile->Get(fCopyObjects[iObj]))
      outFile->WriteTObject(inFile->Get(fCopyObjects[iObj])->Clone());
    else
      std::cout << "Could not find " << fCopyObjects[iObj] << " in " << fileName << std::endl;
  }

  outFile->Close();
  inFile->Close();

  std::cout << fileName << " events removed for" << std::endl;
  std::cout << " Bad Runs:   " << badlumis << std::endl;
  std::cout << " Event Cut:  " << cutevents << std::endl;
  std::cout << " Duplicates: " << duplicates << std::endl;
}
