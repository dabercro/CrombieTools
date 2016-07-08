/**
   @file FileConfigReader.h

   Defines the FileConfigReader class. Since the FileConfigReader class is never used alone,
   the entire class it defined in this header.

   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_COMMONTOOLS_FILECONFIGREADER_H
#define CROMBIETOOLS_COMMONTOOLS_FILECONFIGREADER_H

#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include "TColor.h"
#include "TString.h"
#include "TChain.h"

#include "InOutDirectoryHolder.h"
#include "FileInfo.h"

/**
   @ingroup commongroup
   @class FileConfigReader
   This class reads a formatted [MC Config file](@ref md_docs_FORMATMC). */

class FileConfigReader : public InOutDirectoryHolder
{
 public:
  FileConfigReader();
  virtual ~FileConfigReader();

  /// Differentiates between background, signal MC and data.
  enum FileType { kBackground = 0, kSignal, kData };

  /// Returns a vector of limit tree names that have been read from the configs
  std::set<TString>    ReturnTreeNames      ( FileType type = kBackground);

  enum SearchBy { kLimitName = 0, kLegendEntry };
  /// Returns a vector of file names that have been read from the configs
  std::vector<TString> ReturnFileNames      ( FileType type = kBackground, TString limitName = "", SearchBy search = kLimitName, Bool_t match = true );

  /// Returns a TChain of files that match the FileType and name for the LimitTreeMaker
  TChain*              ReturnTChain         ( TString treeName = "events", FileType type = kBackground,
                                              TString limitName = "", SearchBy search = kLimitName, Bool_t match = true );

  /// Add a data file
  void                 AddDataFile          ( TString fileName );

  /// Reads an MC configuration file
  void                 ReadMCConfig         ( TString config, TString fileDir = "" );

  /// This is the default MC File adder
  void                 AddFile              ( TString treeName, TString fileName, Double_t XSec, 
                                              TString entry = "", Int_t colorstyle = 0 );

  /// Default File adder with FileType changing
  inline    void       AddFile              ( TString treeName, TString fileName, Double_t XSec, 
                                              TString entry, Int_t colorstyle, FileType type )
                                            { SetFileType(type); AddFile(treeName,fileName,XSec,entry,colorstyle); }
  /// This is for when you don't care about limit trees and are adding by hand
  inline    void       AddFile              ( TString fileName, Double_t XSec, TString entry, Int_t colorstyle )
                                                                     { AddFile("",fileName,XSec,entry,colorstyle); }
  /// For when you don't care about limit trees and are adding by hand with type changing
  inline    void       AddFile              ( TString fileName, Double_t XSec, TString entry, 
                                              Int_t colorstyle, FileType type )
                                                     { SetFileType(type); AddFile(fileName,XSec,entry,colorstyle); }

  /// Set the tree name for data
  inline    void       SetDataTreeName      ( TString name )                            { fDataTreeName = name;    }
  /// Set the legend entry for data
  inline    void       SetDataEntry         ( TString entry )                           { fDataEntry = entry;      }

  /// Set the all histogram and luminosity for normalization.
  inline    void       SetAllHistName       ( TString name )                            { fAllHistName = name;     }
  /// Set the Luminosity in inverse pb.
  inline    void       SetLuminosity        ( Double_t lum )                            { fLuminosity = lum;       }

  /// Set the FileType of the next config file read.
  inline    void       SetFileType          ( FileType type )                           { fFileType = type;        }
    
  /// Reads an MC configuration while changing the FileType
  inline    void       ReadMCConfig         ( TString config,  FileType type, TString fileDir = "" ) 
                                                                { SetFileType(type); ReadMCConfig(config,fileDir); }

 protected:
  Double_t   fLuminosity = 2000.0;                        ///< The Luminosity in inverse pb
  TString    fDataTreeName = "data";                      ///< The base name of the data in a limit tree
  TString    fDataEntry = "Data";                         ///< The legend entry for data
  TString    fAllHistName = "htotal";                     ///< The all histogram name used ingenerating cross section weights
  std::vector<FileInfo*>  fDataFileInfo;                  ///< Vector of data FileInfo objects
  std::vector<FileInfo*>  fMCFileInfo;                    ///< Vector of background FileInfo objects
  std::vector<FileInfo*>  fSignalFileInfo;                ///< Vector of signal FileInfo objects

  /// Allows reader to avoid skipping when reading in exception configs
  void       SetKeepAllFiles                ( Bool_t keep )                             { fKeepAllFiles = keep;    }
  /// Allows reader to avoid skipping when reading in exception configs
  void       SetMultiplyLumi                ( Bool_t doMultiply )                    { fMultiplyLumi = doMultiply; }

 private:
  /// Return a pointer to a proper vector of FileInfo
  std::vector<FileInfo*> *GetFileInfo       ( FileType type ); 
  FileType     fFileType = kBackground;                   ///< Type of files in the next config
  Bool_t       fKeepAllFiles = false;                     ///< Keeps FileInfo stored usually deleted by exception configs
  Bool_t       fMultiplyLumi = true;                      ///< Returns XSecWeight with luminosity multiplied
  std::vector<TObject*> fDeleteThese;                     ///< Vector of object pointers to free memory at the end
  
};

//--------------------------------------------------------------------
FileConfigReader::FileConfigReader()
{ }

//--------------------------------------------------------------------
FileConfigReader::~FileConfigReader()
{
  for (UInt_t iInfo = 0; iInfo != fMCFileInfo.size(); ++iInfo)
    delete fMCFileInfo[iInfo];

  for (UInt_t iInfo = 0; iInfo != fSignalFileInfo.size(); ++iInfo)
    delete fSignalFileInfo[iInfo];

  for (UInt_t iInfo = 0; iInfo != fDataFileInfo.size(); ++iInfo)
    delete fDataFileInfo[iInfo];

  for (UInt_t iDelete = 0; iDelete != fDeleteThese.size(); ++iDelete)
    delete fDeleteThese[iDelete];
  fDeleteThese.clear();
}

//--------------------------------------------------------------------
std::vector<FileInfo*>*
FileConfigReader::GetFileInfo(FileType type)
{
  std::vector<FileInfo*> *fileInfo;
  switch (type)
    {
    case kBackground:
      fileInfo = &fMCFileInfo;
      break;
    case kSignal:
      fileInfo = &fSignalFileInfo;
      break;
    case kData:
      fileInfo = &fDataFileInfo;
      break;
    default:
      std::cout << "What case is that?" << std::endl;
      exit(1);
    }
  return fileInfo;
} 

//--------------------------------------------------------------------
std::set<TString>
FileConfigReader::ReturnTreeNames(FileType type)
{
  std::set<TString> output;
  std::vector<FileInfo*> *fileInfo = GetFileInfo(type);

  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo)
    output.insert((*iInfo)->fTreeName);
 
  return output;
}

//--------------------------------------------------------------------
std::vector<TString>
FileConfigReader::ReturnFileNames(FileType type, TString limitName, SearchBy search, Bool_t match)
{
  std::vector<TString> output;
  std::vector<FileInfo*> *fileInfo = GetFileInfo(type);

  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo) {
    if (search == kLimitName) {
      if (limitName != "" && (((*iInfo)->fTreeName != limitName && match) || ((*iInfo)->fTreeName == limitName && !match)))
        continue;
    }
    else if (search == kLegendEntry) {
      if (limitName != "" && (((*iInfo)->fEntry != limitName && match) || ((*iInfo)->fEntry == limitName && !match)))
        continue;
    }

    output.push_back((*iInfo)->fFileName);
  }
 
  return output;
}

//--------------------------------------------------------------------
TChain*
FileConfigReader::ReturnTChain(TString treeName, FileType type, TString limitName, SearchBy search, Bool_t match)
{
  TChain *theChain = new TChain(treeName, treeName + "_chain");
  std::vector<TString> fileList = ReturnFileNames(type, limitName, search, match);
  for (std::vector<TString>::iterator iFile = fileList.begin(); iFile != fileList.end(); ++iFile)
    theChain->Add(iFile->Data());

  fDeleteThese.push_back(theChain);
  return theChain;
}

//--------------------------------------------------------------------
void FileConfigReader::AddDataFile(TString fileName)
{
  FileType tempType = fFileType;
  SetFileType(kData);
  AddFile(fDataTreeName, fileName, -1, fDataEntry);
  SetFileType(tempType);
}

//--------------------------------------------------------------------
void FileConfigReader::AddFile(TString treeName, TString fileName, Double_t XSec, 
                               TString entry, Int_t colorstyle)
{
  FileInfo* tempInfo = new FileInfo(treeName,AddInDir(fileName),XSec,
                                    entry,colorstyle,fAllHistName);
  if (fMultiplyLumi)
    tempInfo->fXSecWeight *= fLuminosity;
  if (fFileType == kBackground)
    fMCFileInfo.push_back(tempInfo);
  else if (fFileType == kSignal)
    fSignalFileInfo.push_back(tempInfo);
  else if (fFileType == kData)
    fDataFileInfo.push_back(tempInfo);
  else {
    std::cout << "Don't have a correct MC Type. Not saving fileInfo." << std::endl;
    delete tempInfo;
  }
}

//--------------------------------------------------------------------

/**
   Reads in a configuration file assuming it has the format descrbed
   in [Formatting MC Configs](@ref md_docs_FORMATMC). Contents of 
   this MC file is stored in one of two vectors FileInfo pointers. */

void FileConfigReader::ReadMCConfig(TString config, TString fileDir)
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
  std::vector<FileInfo*> *FileInfo = &fMCFileInfo;
  if (fFileType == kSignal)
    FileInfo = &fSignalFileInfo;

  std::vector<UInt_t> SplitSamples;

  while (!configFile.eof()) {
    configFile >> LimitTreeName;

    // Do the checking here for merging groups
    if (LimitTreeName == "INGROUP") 
      SplitSamples.push_back((*FileInfo).size());
    else if (LimitTreeName == "ENDGROUP") {

      // Get the uncertainties for each sample
      std::vector<Double_t> Uncerts;
      SplitSamples.push_back((*FileInfo).size());
      for (UInt_t iSample = 0; iSample != SplitSamples.size() - 1; ++iSample) {
        Double_t Uncert = 0.0;
        for (UInt_t iFile = SplitSamples[iSample]; iFile != SplitSamples[iSample + 1]; ++iFile)
          Uncert += (*FileInfo)[iFile]->fXSec * (*FileInfo)[iFile]->fXSecWeight;
        Uncerts.push_back(Uncert);
      }

      // Get the weights from the uncertainties
      std::vector<Double_t> Weights;
      Double_t SumOfWeights = 0.0;
      for (UInt_t iSample = 0; iSample != SplitSamples.size() - 1; ++iSample) {
        Double_t Weight = 1.0;
        for (UInt_t iUncert = 0; iUncert != Uncerts.size(); ++iUncert) {
          if (iSample != iUncert)
            Weight *= Uncerts[iUncert];
        }
        Weights.push_back(Weight);
        SumOfWeights += Weight;
      }

      // Apply the weights
      for (UInt_t iSample = 0; iSample != SplitSamples.size() - 1; ++iSample) {
        for (UInt_t iFile = SplitSamples[iSample]; iFile != SplitSamples[iSample + 1]; ++iFile)
          (*FileInfo)[iFile]->fXSecWeight *= Weights[iSample]/SumOfWeights;
      }

      SplitSamples.resize(0);
    }
    else {
      configFile >> FileName;
      if (LimitTreeName == "skip") {
        if (!fKeepAllFiles) {
          for (UInt_t iFile = 0; iFile != (*FileInfo).size(); ++iFile) {
            if ((*FileInfo)[iFile]->fFileName == AddInDir(FileName)) {
              delete (*FileInfo)[iFile];
              (*FileInfo).erase((*FileInfo).begin() + iFile);
              break;
            }
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
          AddFile(LimitTreeName, FileName, XSec.Atof(), LegendEntry.ReplaceAll("_"," "), ColorStyleEntry.Atoi());
      }
    }
  }
  configFile.close();
}

#endif
