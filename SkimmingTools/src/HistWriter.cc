#include <fstream>
#include <iostream>
#include "TFile.h"
#include "TH1F.h"

#include "HistWriter.h"

ClassImp(HistWriter)

//--------------------------------------------------------------------
HistWriter::HistWriter(TString fileName, TString histName) :
  fFileName(fileName),
  fHistName(histName)
{ }
  
//--------------------------------------------------------------------
HistWriter::~HistWriter()
{ }

//--------------------------------------------------------------------
void
HistWriter::MakeHist(TString configName)
{
  std::vector<Float_t> xVals;
  std::vector<Float_t> binVals;

  std::ifstream configFile;
  configFile.open(configName.Data());
  TString xVal;
  TString binVal;
  Bool_t reading = true;
  while (reading) {
    configFile >> xVal >> binVal;
    xVals.push_back(xVal.Atof());
    if (binVal != "")
      binVals.push_back(binVal.Atof());
    else
      reading = false;
  }

  TFile* outFile = new TFile(fFileName,"UPDATE");
  TH1F*  outHist = new TH1F(fHistName,fHistName,binVals.size(),xVals.data());

  for (UInt_t iBin = 0; iBin != binVals.size(); ++iBin)
    outHist->SetBinContent(iBin + 1, binVals[iBin]);

  outFile->WriteTObject(outHist,fHistName,"Overwrite");
  outFile->Close();
}
