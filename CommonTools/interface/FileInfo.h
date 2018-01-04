/**
   @file FileInfo.h

   Defines the FileInfo class and a function to get cross section weight.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_FILEINFO_H
#define CROMBIETOOLS_COMMONTOOLS_FILEINFO_H

#include <iostream>
#include <sys/stat.h>

#include "TFile.h"
#include "TH1D.h"
#include "TH1.h"
#include "TString.h"

#include "UncertaintyInfo.h"

/**
   @ingroup commongroup
   Returns the XSection weight of each event.

   @param fileName is the name of the file containing all
   events in the sample.
   @param XSec is the cross section in pb in the sample.
   @param allHistName is the name of the histogram counting
   all events that were generated for the sample.

   @returns output that should be multiplied by the
   luminosity (in pb) to get the overall scaling of the histogram.
   This multiplication is done already in FileReader.
*/

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
   @ingroup commongroup
   @struct FileInfo
   Structure holding all the information desired from each file.
*/

struct FileInfo
{
  FileInfo () {}
  /// The constructor fills all of the entries
  FileInfo ( TString treeName, TString fileName, Double_t XSec,
             TString entry, Int_t colorstyle, TString allHist,
             UncertaintyInfo *uncInfo = nullptr )
  : fTreeName{treeName},
    fFileName{fileName},
    fXSec{XSec},
    fEntry{entry},
    fColorStyle{colorstyle},
    fXSecWeight{(allHist == "" || fXSec < 0) ? 1.0 : GetXSecWeight(fFileName, fXSec, allHist)},
    fUncertaintyInfo{uncInfo}
  {
    struct stat file_stat;
    stat(fFileName.Data(), &file_stat);
    fSize = file_stat.st_size;
  }

  ~FileInfo () {
    if (fUncertaintyInfo)
      delete fUncertaintyInfo;
  }

  TString  fTreeName;                ///< Base name for the Limit Tree made by LimitTreeMaker
  TString  fFileName;                ///< Name of the file
  Double_t fXSec;                    ///< Cross section of the sample contained in the file
  TString  fEntry;                   ///< Legend entry for that file
  Int_t    fColorStyle;              ///< Fill color or line style (if signal) for that file
  Double_t fXSecWeight;              ///< Weight determined by GetXSecWeight()
  UncertaintyInfo *fUncertaintyInfo; ///< Structure to supply uncertainty weight to a file
  unsigned long    fSize;            ///< Size of the file
};

inline Bool_t operator<(const FileInfo& a, const FileInfo& b) { return a.fSize < b.fSize; }

#endif
