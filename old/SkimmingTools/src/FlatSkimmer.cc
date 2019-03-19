/**
  @file   FlatSkimmer.cc
  Describes FlatSkimmer class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
#include "TTreeFormula.h"

#include "FlatSkimmer.h"

ClassImp(FlatSkimmer)

//--------------------------------------------------------------------
FlatSkimmer::FlatSkimmer()
{ }

//--------------------------------------------------------------------
FlatSkimmer::~FlatSkimmer()
{ }

//--------------------------------------------------------------------

/**
   Adds an event filter list to be removed during skimming.
   And event filter list must be a text file consisting of events to
   remove in the format \<RunNumber>:\<LumiNumber>:\<EventNumber>.
   Multiple event filters can be added.
*/

void FlatSkimmer::AddEventFilter(TString filterName){
  std::ifstream filterFile;
  filterFile.open(filterName.Data());
  TString eventString;
  while (!filterFile.eof()) {
    filterFile >> eventString;
    if (eventString != "")
      fEventFilter.insert(eventString);
  }
}

//--------------------------------------------------------------------

/**
   Skims a file from the input directory and places it in the output.
   Skimming is done using a cut, GoodLumiFilter, event filters, and
   possibly removing duplicate events. The duplicate events flag is false
   by default because it's a very expensive check.
   The function reports the number of events removed from the file from
   each mechanism.
*/

void FlatSkimmer::Skim(TString fileName)
{
  SetReportFile(fileName);
  TFile *inFile = TFile::Open(AddInDir(fileName));
  TTree *inTree = (TTree*) inFile->Get(fTreeName);

  if (fDisableFile != "") {
    std::ifstream disableFile(fDisableFile.Data());
    TString branchToDisable;
    while (!disableFile.eof()) {
      disableFile >> branchToDisable;
      if (branchToDisable != "")
        inTree->SetBranchStatus(branchToDisable, 0);
    }
    disableFile.close();
  }

  if (fKeepFile != "") {
    inTree->SetBranchStatus("*", 0);
    std::ifstream keepFile(fKeepFile.Data());
    TString branchToKeep;
    while (!keepFile.eof()) {
      keepFile >> branchToKeep;
      if (branchToKeep != "")
        inTree->SetBranchStatus(branchToKeep, 1);
    }
    keepFile.close();
  }

  for (auto* branch : *(inTree->GetListOfBranches())) {
    auto name = branch->GetName();
    if (fCut.Contains(name))
      inTree->SetBranchStatus(name, 1);
  }

  TTreeFormula *cutter = new TTreeFormula("cutter",fCut,inTree);

  UInt_t runNum  = 0;
  UInt_t lumiNum = 0;
  ULong64_t eventNum = 0;
  inTree->SetBranchAddress(fRunExpr,&runNum);
  inTree->SetBranchAddress(fLumiExpr,&lumiNum);
  inTree->SetBranchAddress(fEventExpr,&eventNum);

  TFile *outFile =  new TFile(AddOutDir(fileName),"RECREATE");
  TTree *outTree = inTree->CloneTree(0);

  std::set<TString> eventsRecorded;
  TString testString = "";

  Int_t badlumis   = 0;
  Int_t cutevents  = 0;
  Int_t filtered   = 0;
  Int_t duplicates = 0;

  Long64_t nentries = SetNumberOfEntries(inTree);

  for (Long64_t iEntry = 0; iEntry != nentries; ++iEntry) {
    ReportProgress(iEntry);
    inTree->GetEntry(iEntry);
    if (fGoodLumiFilter.IsGood(runNum,lumiNum)) {
      if (cutter->EvalInstance()) {
        testString = TString::Format("%i:%i:%llu",runNum,lumiNum,eventNum);
        if (fEventFilter.find(testString) != fEventFilter.end()) {
          filtered++;
          continue;
        }
        if (fCheckDuplicates) {
          if (eventsRecorded.find(testString) == eventsRecorded.end())
            eventsRecorded.insert(testString);
          else {
            std::cerr << "Duplicate: " << testString << std::endl;
            duplicates++;
            continue;
          }
        }
        outTree->Fill();
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
  std::cout << " Filter:     " << filtered << std::endl;
  std::cout << " Duplicates: ";
  if (fCheckDuplicates)
    std::cout << duplicates << std::endl;
  else
    std::cout << "Not Checking" << std::endl;

}

//--------------------------------------------------------------------
FlatSkimmer*
FlatSkimmer::Copy()
{
  FlatSkimmer *newSkimmer = new FlatSkimmer();
  *newSkimmer = *this;
  return newSkimmer;
}
