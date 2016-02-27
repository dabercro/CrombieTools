/**
   @file MCFileInfo.h

   Defines the MCFileInfo class and a function to get cross section weight.

   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_COMMONTOOLS_MCFILEINFO_H
#define CROMBIETOOLS_COMMONTOOLS_MCFILEINFO_H

#include <iostream>

#include "TFile.h"
#include "TH1D.h"
#include "TH1.h"
#include "TString.h"

/**
   Returns the XSection weight of each event.

   @param fileName is the name of the MC File containing all
   events in the sample. 
   @param XSec is the cross section in pb in the sample. 
   @param allHistName is the name of the histogram counting 
   all events that were generated for the sample.

   @returns output that should be multiplied by the
   luminosity (in pb) to get the overall scaling of the histogram.
   This multiplication is done already in MCFileReader. */

Double_t GetXSecWeight(TString fileName, Double_t XSec, TString allHistName)
{
  TFile *theFile = TFile::Open(fileName);
  if (!theFile) {
    std::cout << "Cannot open file " << fileName << std::endl;
    exit(1);
  }
  TH1D* allHist = (TH1D*) theFile->Get(allHistName);
  Double_t weight = -1;
  if (allHist)
    weight = XSec/allHist->GetBinContent(1);
  else
    std::cout << "Just so you know, I can't find " << allHistName << std::endl;

  theFile->Close();
  return weight;
}

/**
   @struct MCFileInfo
   Structure holding all the information desired from each MC File. */
struct MCFileInfo
{
  /// The constructor fills all of the entries
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

  TString  fTreeName;    ///< Base name for the Limit Tree made by LimitTreeMaker
  TString  fFileName;    ///< Name of the MC file
  Double_t fXSec;        ///< Cross section of the sample contained in the MC file
  TString  fEntry;       ///< Legend entry for that file
  Int_t    fColorStyle;  ///< Fill color or line style (if signal) for that file
  Double_t fXSecWeight;  ///< Weight determined by GetXSecWeight()
};

#endif
