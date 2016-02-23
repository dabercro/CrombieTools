#ifndef CROMBIETOOLS_COMMONTOOLS_MCREADER_H
#define CROMBIETOOLS_COMMONTOOLS_MCREADER_H

#include <fstream>
#include <vector>
#include "TColor.h"
#include "TString.h"

#include "MCFileInfo.h"

class MCReader
{
 public:
  MCReader();
  virtual ~MCReader();

  // Set the all histogram and luminosity for normalization
  void       SetAllHistName       ( TString name )                      { fAllHistName = name;     }
  void       SetLuminosity        ( Double_t lum )                      { fLuminosity = lum;       }

  enum MCType { kBackground = 0, kSignal };
  void       SetMCType            ( MCType type )                       { fMCType = type;          }

  // This is the default MC File adder
  void       AddMCFile            ( TString treeName, TString fileName, Double_t XSec, 
                                    TString entry = "", Int_t colorstyle = 0 );
  // Default File adder with type changing
  void       AddMCFile            ( TString treeName, TString fileName, Double_t XSec, 
                                    TString entry, Int_t colorstyle, MCType type )
                            { SetMCType(type); AddMCFile(treeName,fileName,XSec,entry,colorstyle); }
  // This is when you don't care about limit trees and are adding by hand
  void       AddMCFile            ( TString fileName, Double_t XSec, TString entry, Int_t colorstyle )
                                                   { AddMCFile("",fileName,XSec,entry,colorstyle); }
  // Same as before with type changing
  void       AddMCFile            ( TString fileName, Double_t XSec, TString entry, 
                                    Int_t colorstyle, MCType type )
                                     { SetMCType(type); AddMCFile(fileName,XSec,entry,colorstyle); }
    
  void       SetInDirectory       ( TString inDir )                      { fInDirectory = inDir;   }
  void       ReadMCConfig         ( TString config, TString fileDir = "" );
  void       ReadMCConfig         ( TString config,  MCType type, TString fileDir = "" ) 
                                                  { SetMCType(type); ReadMCConfig(config,fileDir); }

 protected:
  Double_t   fLuminosity;
  TString    fAllHistName;
  std::vector<MCFileInfo*>  fMCFileInfo;
  std::vector<MCFileInfo*>  fSignalFileInfo;
  TString    fInDirectory;

 private:
  MCType     fMCType;
  
};

//--------------------------------------------------------------------
MCReader::MCReader() :
  fLuminosity(2245.0),
  fAllHistName("htotal"),
  fMCType(kBackground)
{
  fMCFileInfo.clear();
  fSignalFileInfo.clear();
}

//--------------------------------------------------------------------
MCReader::~MCReader()
{
  for (UInt_t iInfo = 0; iInfo != fMCFileInfo.size(); ++iInfo)
    delete fMCFileInfo[iInfo];

  for (UInt_t iInfo = 0; iInfo != fSignalFileInfo.size(); ++iInfo)
    delete fSignalFileInfo[iInfo];
}

//--------------------------------------------------------------------
void
MCReader::AddMCFile(TString treeName, TString fileName, Double_t XSec, 
                    TString entry, Int_t colorstyle)
{
  MCFileInfo* tempInfo = new MCFileInfo(treeName,fileName,XSec,
                                        entry,colorstyle,fAllHistName);
  tempInfo->fXSecWeight *= fLuminosity;
  if (fMCType == kBackground)
    fMCFileInfo.push_back(tempInfo);
  else if (fMCType == kSignal)
    fSignalFileInfo.push_back(tempInfo);
  else {
    std::cout << "Don't have a correct MC Type. Not saving fileInfo." << std::endl;
    delete tempInfo;
  }
}

//--------------------------------------------------------------------
void
MCReader::ReadMCConfig(TString config, TString fileDir)
{
  if (fileDir == "")
    fileDir = fInDirectory;
  
  if (fileDir != "" && !fileDir.EndsWith("/"))
    fileDir = fileDir + "/";

  std::ifstream configFile;
  configFile.open(config.Data());
  TString LimitTreeName;
  TString FileName;
  TString XSec;
  TString LegendEntry;
  TString ColorStyleEntry; 
  TString currLegend;
  TString currColorStyle;
  TString red;
  TString green;
  TString blue;
  Int_t newColors = 0;
  std::vector<MCFileInfo*> *FileInfo = &fMCFileInfo;
  if (fMCType == kSignal)
    FileInfo = &fSignalFileInfo;

  while (!configFile.eof()) {
    configFile >> LimitTreeName >> FileName;
    if (LimitTreeName == "skip") {
      for (UInt_t iFile = 0; iFile != (*FileInfo).size(); ++iFile) {
        if ((*FileInfo)[iFile]->fFileName == FileName) {
          delete (*FileInfo)[iFile];
          (*FileInfo).erase((*FileInfo).begin() + iFile);
          break;
        }
      }
    }
    else {
      configFile >> XSec >> LegendEntry >> ColorStyleEntry;
      if (LegendEntry == ".")
        LegendEntry = currLegend;
      else
        currLegend = LegendEntry;
      
      if (ColorStyleEntry == ".")
        ColorStyleEntry = currColorStyle;
      else if (ColorStyleEntry == "rgb") {
        ++newColors;
        ColorStyleEntry = TString::Format("%i",5000 + newColors);
        currColorStyle = ColorStyleEntry;
        configFile >> red >> green >> blue;
        TColor* setColor = new TColor(ColorStyleEntry.Atoi(),red.Atof()/255,green.Atof()/255,blue.Atof()/255);
      }
      else
        currColorStyle = ColorStyleEntry;

      if (ColorStyleEntry != "" && !LimitTreeName.BeginsWith('#'))
        AddMCFile(LimitTreeName, fileDir + FileName, XSec.Atof(), LegendEntry.ReplaceAll("_"," "), ColorStyleEntry.Atoi());
    }
  }
  configFile.close();
}

#endif
