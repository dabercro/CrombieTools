#include <iostream>
#include <iomanip>

#include "TTreeFormula.h"
#include "TCanvas.h"
#include "TH1I.h"
#include "TH1.h"

#include "CutflowMaker.h"

ClassImp(CutflowMaker)

//--------------------------------------------------------------------
CutflowMaker::CutflowMaker()
{
  fCutNames.resize(0);
  fCuts.resize(0);
  fYields.resize(0);
}

//--------------------------------------------------------------------
void
CutflowMaker::GetCutflow()
{
  if (fYields.size() == 0 && fTree != NULL) {
    std::vector<TTreeFormula*> formulae;
    TTreeFormula *tempFormula;
    for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {
      tempFormula = new TTreeFormula(fCutNames[iCut],fCuts[iCut],fTree);
      formulae.push_back(tempFormula);
      fYields.push_back(0);
    }
    Int_t numEntries = fTree->GetEntriesFast();
    for (Int_t iEntry = 0; iEntry != numEntries; ++iEntry) {
      fTree->GetEntry(iEntry);
      for (UInt_t iCut = 0; iCut != formulae.size(); ++iCut) {
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
CutflowMaker::PrintCutflow(Bool_t OnlyNums)
{
  GetCutflow();
  std::cout << std::endl;
  for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {
    if (!OnlyNums) {
      std::cout << std::setw(20);
      std::cout << fCutNames[iCut] << ": ";
    }
    if (fTree != NULL) {
      std::cout << std::setw(15);
      std::cout << fYields[iCut];
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

//--------------------------------------------------------------------
void
CutflowMaker::MakePlot(TString name)
{
  GetCutflow();
  TH1I* theHist = new TH1I("cutflow",";;Number of Events",fCuts.size(),0,fCuts.size());
  for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {
    theHist->GetXaxis()->SetBinLabel(iCut+1,fCutNames[iCut]);
    theHist->SetBinContent(iCut+1,fYields[iCut]);
  }
  theHist->SetLineWidth(2);
  
  TCanvas* theCanvas = new TCanvas("canvas",";;Number of Events");
  theHist->Draw();

  theCanvas->SaveAs(name + ".pdf");
  theCanvas->SaveAs(name + ".png");
  theCanvas->SaveAs(name + ".C");
}
