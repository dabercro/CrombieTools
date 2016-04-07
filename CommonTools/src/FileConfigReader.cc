/**
   @file FileConfigReader.cc
   Defines the function members for the FileConfigReader class.
   @author Daniel Abercrombie <dabercro@mit.edu> */

#include <fstream>
#include <iostream>

#include "FileConfigReader.h"

ClassImp(FileConfigReader)

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
}

//--------------------------------------------------------------------
std::vector<TString>
FileConfigReader::ReturnFileNames(FileType type)
{
  std::vector<TString> output;
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

  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo)
    output.push_back((*iInfo)->fFileName);
 
  return output;
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

  while (!configFile.eof()) {
    configFile >> LimitTreeName >> FileName;
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
  configFile.close();
}
