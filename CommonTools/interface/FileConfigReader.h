/**
   @file FileConfigReader.h

   Defines the FileConfigReader class. Since the FileConfigReader class is never used alone,
   the entire class it defined in this header.

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_FILECONFIGREADER_H
#define CROMBIETOOLS_COMMONTOOLS_FILECONFIGREADER_H

#include <fstream>
#include <iostream>
#include <set>
#include <vector>
#include "TColor.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1D.h"

#include "InOutDirectoryHolder.h"
#include "FileInfo.h"
#include "PlotHists.h"

/**
   @ingroup commongroup
   @class FileConfigReader
   This class reads a formatted [MC Config file](@ref formatmc).
*/

class FileConfigReader : public InOutDirectoryHolder, public PlotHists
{
 public:
  FileConfigReader();
  virtual ~FileConfigReader();

  /// Resets the information from the config files being held
  void                 ResetConfig          ();

  /// Differentiates between background, signal MC and data
  enum FileType {
    kBackground = 0,   ///< Specifies the standard model background files
    kSignal,           ///< The signals in the analysis
    kData,             ///< Real data from the detector
  };

  /// Returns a vector of limit tree names that have been read from the configs
  std::set<TString>    ReturnTreeNames      ( FileType type = kBackground);

  /// Determines how to match to return files
  enum SearchBy {
    kLimitName = 0,    ///< Match the LimitTree entry
    kLegendEntry       ///< Match the legend entry
  };
  /// Returns a vector of file names that have been read from the configs
  std::vector<TString> ReturnFileNames      ( FileType type = kBackground, TString matchName = "",
                                              SearchBy search = kLimitName, Bool_t match = true );

  /// Returns a TChain of files that match the FileType and name for the LimitTreeMaker
  TChain*              ReturnTChain         ( TString treeName = "events", FileType type = kBackground,
                                              TString matchName = "", SearchBy search = kLimitName, Bool_t match = true );

  /// Add a data file
  void                 AddDataFile          ( TString fileName );

  /// Reads an MC configuration file
  void                 ReadMCConfig         ( TString config, TString fileDir = "" );

  /// This is the default MC File adder
  void                 AddFile              ( TString treeName, TString fileName, Double_t XSec, 
                                              TString entry = "", Int_t colorstyle = 0 );

  /// The multipliers for Data can be set separately.
  inline    void       SetDataWeight        ( TString weight )                          { fDataWeight = weight;    }
  /// The multipliers for MC can be set separately.
  inline    void       SetMCWeight          ( TString weight )                          { fMCWeight = weight;      }
  
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
  /// Set the tree of the files you are trying to plot
  inline    void       SetTreeName          ( TString name )                            { fTreeName = name;        }

  /// Use this to set a different expression for data from MC
  inline    void       SetDataExpression    ( TString expr )                            { fDataExpression = expr;  }

 protected:
  Double_t   fLuminosity = 2000.0;                        ///< The Luminosity in inverse pb
  TString    fDataTreeName = "data";                      ///< The base name of the data in a limit tree
  TString    fDataEntry = "Data";                         ///< The legend entry for data
  TString    fAllHistName = "htotal";                     ///< The all histogram name used ingenerating cross section weights
  std::vector<FileInfo*>  fDataFileInfo;                  ///< Vector of data FileInfo objects
  std::vector<FileInfo*>  fMCFileInfo;                    ///< Vector of background FileInfo objects
  std::vector<FileInfo*>  fSignalFileInfo;                ///< Vector of signal FileInfo objects

  /// Draws histograms for using the default expression and file configuration
  std::vector<TH1D*>    GetHistList            ( Int_t NumXBins, Double_t *XBins, FileType type, TString matchName = "",
                                                 SearchBy search = kLimitName, Bool_t match = true);

  /// Draw a single histogram using the default expression and file configuration
  TH1D*      GetHist       ( Int_t NumXBins, Double_t *XBins, FileType type, TString matchName = "",
                             SearchBy search = kLimitName, Bool_t match = true);

  void       OpenFiles     ( std::vector<TString> fileNames );  ///< Opens the files in a vector
  void       CloseFiles    ();                                  ///< Closes the files that are open

  /// Allows reader to avoid skipping when reading in exception configs
  void       SetKeepAllFiles                ( Bool_t keep )                             { fKeepAllFiles = keep;    }
  /// Allows reader to avoid skipping when reading in exception configs
  void       SetMultiplyLumi                ( Bool_t doMultiply )                    { fMultiplyLumi = doMultiply; }

 private:
  TString    fTreeName = "events";                        ///< Stores name of tree from file

  /// Return a pointer to a proper vector of FileInfo
  std::vector<FileInfo*> *GetFileInfo       ( FileType type ); 

  /// Fill fHists with histograms that would be generated with GetHistList().
  void       GenerateHistograms           ( Int_t NumXBins, Double_t *XBins, FileType type );

  FileType   fFileType = kBackground;                     ///< Type of files in the next config
  Bool_t     fKeepAllFiles = false;                       ///< Keeps FileInfo stored usually deleted by exception configs
  Bool_t     fMultiplyLumi = true;                        ///< Returns XSecWeight with luminosity multiplied
  std::vector<TObject*> fDeleteThese;                     ///< Vector of object pointers to free memory at the end
  std::vector<TFile*>   fFiles;                           ///< Vector of active files
  std::vector<TTree*>   fTrees;                           ///< Vector of active files' trees
  std::vector<TH1D*>    fHists;                           ///< Vector of histograms to use
  std::vector<std::vector<TFile*>> fAllFiles;             ///< Vector of all open files

  TString    fDataWeight = "";                            ///< Separate Data weights if needed
  TString    fMCWeight = "";                              ///< Separate MC weights if needed
  
  TString    fDataExpression = "";                        ///< Holds an alternative expression to plot data in
  
};

//--------------------------------------------------------------------
FileConfigReader::FileConfigReader()
{ }

//--------------------------------------------------------------------
FileConfigReader::~FileConfigReader()
{
  CloseFiles();
  ResetConfig();
}

//--------------------------------------------------------------------
void
FileConfigReader::ResetConfig()
{
  for (UInt_t iInfo = 0; iInfo != fMCFileInfo.size(); ++iInfo)
    delete fMCFileInfo[iInfo];
  fMCFileInfo.clear();

  for (UInt_t iInfo = 0; iInfo != fSignalFileInfo.size(); ++iInfo)
    delete fSignalFileInfo[iInfo];
  fSignalFileInfo.clear();

  for (UInt_t iInfo = 0; iInfo != fDataFileInfo.size(); ++iInfo)
    delete fDataFileInfo[iInfo];
  fDataFileInfo.clear();

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
      std::cerr << "What case is that?" << std::endl;
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
FileConfigReader::ReturnFileNames(FileType type, TString matchName, SearchBy search, Bool_t match)
{
  std::vector<TString> output;
  std::vector<FileInfo*> *fileInfo = GetFileInfo(type);

  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo) {
    if (search == kLimitName) {
      if (matchName != "" && (((*iInfo)->fTreeName != matchName && match) || ((*iInfo)->fTreeName == matchName && !match)))
        continue;
    }
    else if (search == kLegendEntry) {
      if (matchName != "" && (((*iInfo)->fEntry != matchName && match) || ((*iInfo)->fEntry == matchName && !match)))
        continue;
    }

    output.push_back((*iInfo)->fFileName);
  }
 
  return output;
}

//--------------------------------------------------------------------
TChain*
FileConfigReader::ReturnTChain(TString treeName, FileType type, TString matchName, SearchBy search, Bool_t match)
{
  TChain *theChain = new TChain(treeName, treeName + "_chain");
  std::vector<TString> fileList = ReturnFileNames(type, matchName, search, match);
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
   in [Formatting MC Configs](@ref formatmc). Contents of 
   this MC file is stored in one of two vectors FileInfo pointers.
*/

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
  TString currTreeName;
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

    if (LimitTreeName == ".")
      LimitTreeName = currTreeName;
    else if (LimitTreeName != "#.") {
      if (LimitTreeName.BeginsWith('#'))
        currTreeName = TString(LimitTreeName.Strip(TString::kLeading, '#'));
      else
        currTreeName = LimitTreeName;
    }

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

//--------------------------------------------------------------------
std::vector<TH1D*>
FileConfigReader::GetHistList(Int_t NumXBins, Double_t *XBins, FileType type, 
                              TString matchName, SearchBy search, Bool_t match)
{

  std::vector<TString> theFileNames = ReturnFileNames(type, matchName, search, match);
  OpenFiles(theFileNames);

  std::vector<FileInfo*> *theFileInfo = &fMCFileInfo;
  TString tempCutHolder = "";
  TString tempExprHolder = "";

  if (type == kSignal)
    theFileInfo = &fSignalFileInfo;

  SetTreeList(fTrees);

  if (type == kData && fDataWeight != "") {

    tempCutHolder = fDefaultCut;
    SetDefaultWeight(TString("(") + tempCutHolder + TString(") && (") + fDataWeight + TString(")"));

  }
  else if (type != kData && fMCWeight != "") {

    tempCutHolder = fDefaultCut;
    SetDefaultWeight(TString("(") + tempCutHolder + TString(")*(") + fMCWeight + TString(")"));

  }

  if (type == kData && fDataExpression != "") {

    tempExprHolder = fDefaultExpr;
    SetDefaultExpr(fDataExpression);

  }

  SetUncertaintySquared("");

  if (type == kBackground && fSystematicBranches.size() > 0) {

    // Create the systematics and apply them to each histogram
    TString sys_expr = fSystematicBranches[0] + "*" + fSystematicBranches[0];
    for (UInt_t iBranch = 1; iBranch != fSystematicBranches.size(); ++iBranch)
      sys_expr += "+" + fSystematicBranches[iBranch] + "*" + fSystematicBranches[iBranch];

    SetUncertaintySquared(sys_expr);
  }

  std::vector<TH1D*> theHists = MakeHists(NumXBins, XBins);

  if (type == kData && fDataExpression != "")
    SetDefaultExpr(tempExprHolder);

  if (tempCutHolder != "")
    SetDefaultWeight(tempCutHolder);

  if (type != kData) {

    UInt_t iFileName = 0;

    for (UInt_t iFile = 0; iFile < theFileInfo->size(); iFile++) {

      if ((*theFileInfo)[iFile]->fFileName != theFileNames[iFileName])
        continue;

      theHists[iFileName]->Scale((*theFileInfo)[iFile]->fXSecWeight);

      ++iFileName;
    }

  }

  return theHists;
}

//--------------------------------------------------------------------
TH1D*
FileConfigReader::GetHist(Int_t NumXBins, Double_t *XBins, FileType type, 
                          TString matchName, SearchBy search, Bool_t match)
{

  std::vector<TH1D*> theHists = GetHistList(NumXBins, XBins, type, matchName, search, match);

  if (theHists.size() == 0) {
    std::cerr << "The list of histograms called is empty." << std::endl;
    exit(30);
  }

  TH1D *output = (TH1D*) theHists[0]->Clone();
  output->Reset("M");

  for (std::vector<TH1D*>::iterator iHist = theHists.begin(); iHist != theHists.end(); ++iHist)
    output->Add(*iHist);

  return output;

}

//--------------------------------------------------------------------
void
FileConfigReader::OpenFiles(std::vector<TString> fileNames)
{

  fFiles.resize(0);
  fTrees.resize(0);

  for (std::vector<TString>::iterator iFile = fileNames.begin(); iFile != fileNames.end(); ++iFile) {

    TFile *tempFile = TFile::Open(iFile->Data());
    TTree *tempTree;
    if (fTreeName.Contains("/"))
      tempTree = (TTree*) tempFile->Get(fTreeName);
    else
      tempTree = (TTree*) tempFile->FindObjectAny(fTreeName);

    if (!tempTree) {
      std::cerr << "Tree not found in file!" << std::endl;
      std::cerr << "File: " << tempFile << std::endl;
      std::cerr << iFile->Data() << std::endl;
      std::cerr << "Tree: " << tempTree << std::endl;
      std::cerr << fTreeName << std::endl;
      exit(1010);
    }

    fFiles.push_back(tempFile);
    fTrees.push_back(tempTree);

  }

  fAllFiles.push_back(fFiles);

}

//--------------------------------------------------------------------
void
FileConfigReader::CloseFiles()
{

  for (UInt_t iFiles = 0; iFiles != fAllFiles.size(); ++iFiles) {
    for (UInt_t iFile = 0; iFile != fAllFiles[iFiles].size(); ++iFile)
      fAllFiles[iFiles][iFile]->Close();
    fAllFiles[iFiles].clear();
  }
  
  fFiles.clear();
  fTrees.clear();
  fAllFiles.clear();

}

#endif
