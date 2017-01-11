#include <iostream>
#include <iomanip>

#include "TTreeFormula.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH1.h"

#include "CutflowMaker.h"

ClassImp(CutflowMaker)

//--------------------------------------------------------------------
void
CutflowMaker::GetCutflow(UInt_t index)
{
  TTree *inTree = fDefaultTree;

  if (fInTrees.size() != 0)
    inTree = fInTrees[index];

  if (fYields.size() == 0) {

    std::vector<TTreeFormula*> formulae;
    TTreeFormula *tempFormula;

    for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {

      tempFormula = new TTreeFormula(fCutNames[iCut], fCuts[iCut], inTree);
      tempFormula->SetQuickLoad(true);
      formulae.push_back(tempFormula);
      fYields.push_back(0);

    }

    Int_t numEntries = inTree->GetEntriesFast();

    for (Int_t iEntry = 0; iEntry != numEntries; ++iEntry) {

      inTree->GetEntry(iEntry);

      for (UInt_t iCut = 0; iCut != formulae.size(); ++iCut) {

        // Should get a vector filled so we can do triggers
        formulae[iCut]->GetNdata();

        if (formulae[iCut]->EvalInstance() == 0)
          break;

        ++fYields[iCut];

      }
    }

    for (UInt_t iFormula = 0; iFormula != formulae.size(); ++iFormula)
      delete formulae[iFormula];
  }
}

//--------------------------------------------------------------------
void
CutflowMaker::PrintCutflow(TableType table)
{
  GetCutflow(0);
  std::cout << std::endl;

  for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {

    if (table != kNumbers) {

      std::cout << std::setw(20);
      std::cout << fCutNames[iCut];

        if (table == kLatex)
          std::cout << " & ";
        else
          std::cout << ": ";

    }

    std::cout << std::setw(15);
    std::cout << fYields[iCut];
    if (table == kLatex)
      std::cout << " \\";

    std::cout << std::endl;

  }
  std::cout << std::endl;
}

//--------------------------------------------------------------------
void
CutflowMaker::MakePlot(TString name, PlotType type)
{

  std::vector<TH1D*> theHists;

  for (UInt_t iTree = 0; iTree != fInTrees.size(); ++iTree) {

    GetCutflow(iTree);

    TH1D* theHist = new TH1D(TString::Format("cutflow_%d", iTree),";;Number of Events",
                             fCuts.size(), 0, fCuts.size());

    for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {

      theHist->GetXaxis()->SetBinLabel(iCut+1,fCutNames[iCut]);

      if (type == kAbsolute)
        theHist->SetBinContent(iCut+1,fYields[iCut]);

      if (type == kFractional) {

        if (iCut == 0)
          theHist->SetBinContent(iCut+1,float(fYields[iCut])/fInTrees[iTree]->GetEntriesFast());
        else
          theHist->SetBinContent(iCut+1,float(fYields[iCut])/fYields[iCut - 1]);

      }
    }

    theHists.push_back(theHist);
    fYields.resize(0);

  }

  BaseCanvas(name, theHists, "", "Yield");

  for (UInt_t iHist = 0; iHist != theHists.size(); ++iHist)
    delete theHists[iHist];
}
