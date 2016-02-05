#include <iostream>
#include <iomanip>

#include "TTreeFormula.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH1.h"

#include "CutflowMaker.h"

ClassImp(CutflowMaker)

//--------------------------------------------------------------------
CutflowMaker::CutflowMaker() :
  fWidth(600),
  fHeight(600)
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
CutflowMaker::MakePlot(TString name, PlotStyle type)
{
  if (fTree == NULL) {
    std::cout << "Tree has not been set." << std::endl;
    exit(1);
  }
  GetCutflow();
  TH1F* theHist = new TH1F("cutflow",";;Number of Events",fCuts.size(),0,fCuts.size());
  for (UInt_t iCut = 0; iCut != fCuts.size(); ++iCut) {
    theHist->GetXaxis()->SetBinLabel(iCut+1,fCutNames[iCut]);
    if (PlotStyle == kAbsolute)
      theHist->SetBinContent(iCut+1,fYields[iCut]);
    if (PlotStyle == kFractional) {
      if (iCut == 0)
        theHist->SetBinContent(iCut+1,float(fYields[iCut])/fTree->GetEntriesFast());
      else
        theHist->SetBinContent(iCut+1,float(fYields[iCut])/fYields[iCut - 1]);
    }
  }
  theHist->SetLineWidth(2);
  
  TCanvas* theCanvas = new TCanvas("canvas",";;Number of Events",fWidth,fHeight);
  theHist->Draw();

  theCanvas->SaveAs(name + ".pdf");
  theCanvas->SaveAs(name + ".png");
  theCanvas->SaveAs(name + ".C");
}
