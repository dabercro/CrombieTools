#include <iostream>
#include <iomanip>

#include "TTreeFormula.h"

#include "CutflowMaker.h"

ClassImp(CutflowMaker)

//--------------------------------------------------------------------
CutflowMaker::CutflowMaker()
{
  fCutNames.resize(0);
  fCuts.resize(0);
}

//--------------------------------------------------------------------
void
CutflowMaker::PrintCutflow(TTree* tree, Bool_t OnlyNums)
{

  std::vector<Int_t> results;

  if (tree != NULL) {
    std::vector<TTreeFormula*> formulae;
    TTreeFormula *tempFormula;
    for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {
      tempFormula = new TTreeFormula(fCutNames[iCut],fCuts[iCut],tree);
      formulae.push_back(tempFormula);
      results.push_back(0);
    }
    Int_t numEntries = tree->GetEntriesFast();
    for (Int_t iEntry = 0; iEntry != numEntries; ++iEntry) {
      tree->GetEntry(iEntry);
      for (UInt_t iCut = 0; iCut != formulae.size(); ++iCut) {
        if (formulae[iCut]->EvalInstance() == 0)
          break;
        ++results[iCut];
      }
    }
  }

  std::cout << std::setw(15);
  for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {
    if (!OnlyNums)
      std::cout << fCutNames[iCut];
    if (tree != NULL)
      std::cout << results[iCut];
    std::cout << std::endl;
  }
}
