#ifndef CROMBIETOOLS_COMMONTOOLS_MCFILEINFO_H
#define CROMBIETOOLS_COMMONTOOLS_MCFILEINFO_H

#include <iostream>

#include "TFile.h"
#include "TH1.h"
#include "TString.h"

Double_t GetXSecWeight(TString fileName, Double_t XSec, TString allHistName)
// Returns the XSection weight of each event.
// Just multiply the output by the luminosity (in pb).
// This multiplication is done already in MCFileReader
{
  TFile *theFile = TFile::Open(fileName);
  if (!theFile) {
    std::cout << "Cannot open file " << fileName << std::endl;
    exit(1);
  }
  TH1* allHist = (TH1*) theFile->Get(allHistName);
  Double_t weight = -1;
  if (allHist)
    weight = XSec/allHist->GetBinContent(1);
  else
    std::cout << "Just so you know, I can't find " << allHistName << std::endl;

  theFile->Close();
  return weight;
}

struct MCFileInfo
{
  MCFileInfo ( TString treeName, TString fileName, Double_t XSec, 
               TString entry, Int_t colorstyle, TString allHist ) {
    fTreeName = treeName;
    fFileName = fileName;
    fXSec = XSec;
    fEntry = entry;
    fColorStyle = colorstyle;
    if (allHist == "" || fXSec < 0)
      fXSecWeight = -1;
    else
      fXSecWeight = GetXSecWeight(fFileName, fXSec, allHist);
  }
  virtual ~MCFileInfo()  {}

  TString  fTreeName;
  TString  fFileName;
  Double_t fXSec;
  TString  fEntry;
  Int_t    fColorStyle;
  Double_t fXSecWeight;
};

#endif
