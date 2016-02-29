/**
   @file MCReader.h

   Defines the MCReader class. Since the MCReader class is never used alone,
   the entire class it defined in this header.

   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_COMMONTOOLS_MCREADER_H
#define CROMBIETOOLS_COMMONTOOLS_MCREADER_H

#include <fstream>
#include <vector>
#include "TColor.h"
#include "TString.h"

#include "InOutDirectoryHolder.h"
#include "MCFileInfo.h"

/**
   @ingroup commongroup
   @class MCReader
   This class reads a formatted [MC Config file](@ref md_docs_FORMATMC). */

class MCReader : public InOutDirectoryHolder
{
 public:
  MCReader();
  virtual ~MCReader();


  /// Differentiates between background and signal MC.
  enum MCType { kBackground = 0, kSignal };

  /// Reads an MC configuration file
  void                 ReadMCConfig         ( TString config, TString fileDir = "" );

  /// This is the default MC File adder
  void                 AddMCFile            ( TString treeName, TString fileName, Double_t XSec, 
                                              TString entry = "", Int_t colorstyle = 0 );

  /// Default File adder with MCType changing
  inline    void       AddMCFile            ( TString treeName, TString fileName, Double_t XSec, 
                                              TString entry, Int_t colorstyle, MCType type )
                                              { SetMCType(type); AddMCFile(treeName,fileName,XSec,entry,colorstyle); }
  /// This is for when you don't care about limit trees and are adding by hand
  inline    void       AddMCFile            ( TString fileName, Double_t XSec, TString entry, Int_t colorstyle )
                                                                     { AddMCFile("",fileName,XSec,entry,colorstyle); }
  /// For when you don't care about limit trees and are adding by hand with type changing
  inline    void       AddMCFile            ( TString fileName, Double_t XSec, TString entry, 
                                              Int_t colorstyle, MCType type )
                                                       { SetMCType(type); AddMCFile(fileName,XSec,entry,colorstyle); }

  /// Set the all histogram and luminosity for normalization.
  inline    void       SetAllHistName       ( TString name )                              { fAllHistName = name;     }
  /// Set the Luminosity in inverse pb.
  inline    void       SetLuminosity        ( Double_t lum )                              { fLuminosity = lum;       }

  /// Set the MCType of the next config file read.
  inline    void       SetMCType            ( MCType type )                               { fMCType = type;          }
    
  /// Reads an MC configuration while changing the MCType
  inline    void       ReadMCConfig         ( TString config,  MCType type, TString fileDir = "" ) 
                                                                    { SetMCType(type); ReadMCConfig(config,fileDir); }

 protected:
  Double_t   fLuminosity = 2245.0;                        ///< The Luminosity in inverse pb
  TString    fAllHistName = "htotal";                     ///< The all histogram name used ingenerating cross section weights
  std::vector<MCFileInfo*>  fMCFileInfo;                  ///< Vector of background MCFileInfo objects
  std::vector<MCFileInfo*>  fSignalFileInfo;              ///< Vector of signal MCFileInfo objects

 private:
  MCType     fMCType = kBackground;                       ///< Type of files in the next config
  
};

//--------------------------------------------------------------------
MCReader::MCReader()
{ }

//--------------------------------------------------------------------
MCReader::~MCReader()
{
  for (UInt_t iInfo = 0; iInfo != fMCFileInfo.size(); ++iInfo)
    delete fMCFileInfo[iInfo];

  for (UInt_t iInfo = 0; iInfo != fSignalFileInfo.size(); ++iInfo)
    delete fSignalFileInfo[iInfo];
}

//--------------------------------------------------------------------
void MCReader::AddMCFile(TString treeName, TString fileName, Double_t XSec, 
                    TString entry, Int_t colorstyle)
{
  MCFileInfo* tempInfo = new MCFileInfo(treeName,AddInDir(fileName),XSec,
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

/**
   Reads in a configuration file assuming it has the format descrbed
   in [Formatting MC Configs](@ref mc_docs_FORMATMC). Contents of 
   this MC file is stored in one of two vectors MCFileInfo pointers. */

void MCReader::ReadMCConfig(TString config, TString fileDir)
{
  if (fileDir != "")
    SetInDirectory(fileDir);
  
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
        if ((*FileInfo)[iFile]->fFileName == AddInDir(FileName)) {
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
        AddMCFile(LimitTreeName, FileName, XSec.Atof(), LegendEntry.ReplaceAll("_"," "), ColorStyleEntry.Atoi());
    }
  }
  configFile.close();
}

#endif
