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
#include <map>
#include <set>
#include <vector>
#include "TColor.h"
#include "TString.h"
#include "TCut.h"
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

  /// Differentiates between background, signal MC and data
  enum FileType {
    kBackground = 0,   ///< Specifies the standard model background files
    kSignal,           ///< The signals in the analysis
    kData,             ///< Real data from the detector
  };

  /// Resets the information from the for one type
  void                 ResetConfig          (FileType type = kBackground);
  /// Resets the information from the config files being held for all types
  void                 ResetAllConfig       ();

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
  inline    void       SetDataWeight        ( TCut weight )                             { fDataWeight = weight;    }
  /// The multipliers for Data can be set separately.
  inline    void       SetDataWeight        ( const char* weight )                  { SetDataWeight(TCut(weight)); }
  /// The multipliers for MC can be set separately.
  inline    void       SetMCWeight          ( TCut weight )                             { fMCWeight = weight;      }
  /// The multipliers for MC can be set separately.
  inline    void       SetMCWeight          ( const char* weight )                    { SetMCWeight(TCut(weight)); }

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
  /// Get the FileType of the current file.
  inline    FileType   GetFileType          ()                                          { return fFileType;        }

  /// Reads an MC configuration while changing the FileType
  inline    void       ReadMCConfig         ( TString config,  FileType type, TString fileDir = "" )
                                                                { SetFileType(type); ReadMCConfig(config,fileDir); }
  /// Set the tree of the files you are trying to plot
  inline    void       SetTreeName          ( TString name )                            { fTreeName = name;        }

  /// Use this to set a different expression for data from MC
  inline    void       SetDataExpression    ( TString expr )                            { fDataExpression = expr;  }

  /**
     Use this function to replace the Legend Entries of the FileInfo with the Limit Tree Names.
     Note that this destroys the known Legend Entries.
     You should also call FileConfigReader::SetLegendColor for each entry that you want to
     ensure consistency between your plots.
  */
  void                 NameTreesAfterLimits ( FileType type = kBackground );

  /// Sets the FileInfo with the matching Legend Names to the new colors
  void                 SetLegendColor       ( TString entry, Int_t color, FileType type = kBackground );

  /// Scales the cross section of the matching MC samples by a factor of 1.0 + scale, destroying the old cross section in the process.
  void                 ScaleBackgrounds     ( TString entry, Double_t scale, SearchBy search = kLimitName, FileType type = kBackground );

  /// Loads the fit result into memory for this stacks to make
  void LoadFitResult      ( const char* fit_result );

  /// Allows reader to avoid skipping when reading in exception configs
  void       SetKeepAllFiles                ( Bool_t keep )                             { fKeepAllFiles = keep;    }

 protected:
  Double_t   fLuminosity = 2000.0;                        ///< The Luminosity in inverse pb
  TString    fDataTreeName = "data";                      ///< The base name of the data in a limit tree
  TString    fDataEntry = "Data";                         ///< The legend entry for data
  TString    fAllHistName = "htotal";                     ///< The all histogram name used ingenerating cross section weights
  std::vector<FileInfo*>  fDataFileInfo;                  ///< Vector of data FileInfo objects
  std::vector<FileInfo*>  fMCFileInfo;                    ///< Vector of background FileInfo objects
  std::vector<FileInfo*>  fSignalFileInfo;                ///< Vector of signal FileInfo objects

  /// Draws histograms for using the default expression and a file list
  std::vector<TH1D*>    GetHistList            ( Int_t NumXBins, Double_t *XBins, std::vector<TString> FileList, FileType type );

  /// Draws histograms for using the default expression and file configuration
  std::vector<TH1D*>    GetHistList            ( Int_t NumXBins, Double_t *XBins, FileType type, TString matchName = "",
                                                 SearchBy search = kLimitName, Bool_t match = true);

  /// Draw a single histogram using the default expression and a file list
  TH1D*      GetHist       ( std::vector<TH1D*> HistList );

  /// Draw a single histogram using the default expression and a file list
  TH1D*      GetHist       ( Int_t NumXBins, Double_t *XBins, std::vector<TString> FileList, FileType type );

  /// Draw a single histogram using the default expression and file configuration
  TH1D*      GetHist       ( Int_t NumXBins, Double_t *XBins, FileType type, TString matchName = "",
                             SearchBy search = kLimitName, Bool_t match = true);

  void       OpenFiles     ( std::vector<TString> fileNames );  ///< Opens the files in a vector
  void       CloseFiles    ();                                  ///< Closes the files that are open

  /// Return a pointer to a proper vector of FileInfo
  std::vector<FileInfo*> *GetFileInfo       ( FileType type );

  /// Allows reader to avoid skipping when reading in exception configs
  void       SetMultiplyLumi                ( Bool_t doMultiply )                    { fMultiplyLumi = doMultiply; }

  TCut       fDataWeight = "";                            ///< Separate Data weights if needed
  TCut       fMCWeight = "";                              ///< Separate MC weights if needed

  TString    fTreeName = "events";                        ///< Stores name of tree from file

 private:
  FileType   fFileType = kBackground;                     ///< Type of files in the next config
  Bool_t     fKeepAllFiles = false;                       ///< Keeps FileInfo stored usually deleted by exception configs
  Bool_t     fMultiplyLumi = true;                        ///< Returns XSecWeight with luminosity multiplied
  std::vector<TObject*> fDeleteThese;                     ///< Vector of object pointers to free memory at the end
  std::vector<TFile*>   fFiles;                           ///< Vector of active files
  std::vector<TH1D*>    fHists;                           ///< Vector of histograms to use
  std::vector<std::vector<TFile*>> fAllFiles;             ///< Vector of all open files

  TString    fDataExpression = "";                        ///< Holds an alternative expression to plot data in

  std::map<TString, double> fFitResult;                   ///< Holds the fit results

};

// Dump this into global namespace because fuck typing
using FileType = FileConfigReader::FileType;
const std::vector<FileType> gFileTypes {FileType::kBackground, FileType::kSignal, FileType::kData};

//--------------------------------------------------------------------
FileConfigReader::FileConfigReader()
{ }

//--------------------------------------------------------------------
FileConfigReader::~FileConfigReader()
{
  CloseFiles();
  ResetAllConfig();
}

//--------------------------------------------------------------------
void
FileConfigReader::ResetConfig(FileType type)
{
  std::vector<FileInfo*>* fileInfo = GetFileInfo(type);

  for (UInt_t iInfo = 0; iInfo != fileInfo->size(); ++iInfo)
    delete (*fileInfo)[iInfo];
  fileInfo->clear();
}

//--------------------------------------------------------------------
void
FileConfigReader::ResetAllConfig()
{
  ResetConfig(kBackground);
  ResetConfig(kSignal);
  ResetConfig(kData);

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
      Message(eError, "What case is that?");
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
  DisplayFunc(__func__);

  Message(eDebug, "Searching for files that match %s", matchName.Data());
  std::vector<TString> output;
  std::vector<FileInfo*> *fileInfo = GetFileInfo(type);

  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo) {

    if (search == kLimitName) {
      Message(eDebug, "Comparing to Limit Tree %s", (*iInfo)->fTreeName.Data());
      if (matchName != "" && (((*iInfo)->fTreeName != matchName && match) || ((*iInfo)->fTreeName == matchName && !match)))
        continue;

    } else if (search == kLegendEntry) {
      Message(eDebug, "Comparing to Legend Entry %s", (*iInfo)->fEntry.Data());
      if (matchName != "" && (((*iInfo)->fEntry != matchName && match) || ((*iInfo)->fEntry == matchName && !match)))
        continue;

    }

    Message(eDebug, "Found a match! Adding file %s", (*iInfo)->fFileName.Data());

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

  DisplayFunc(__func__);
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

    Message(eError, "Don't have a correct MC Type. Not saving fileInfo.");
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
  DisplayFunc(__func__);

  Message(eInfo, "Reading MC Config file %s", config.Data());

  if (fileDir != "")
    SetInDirectory(fileDir);

  Message(eInfo, "Input directory is %s", GetInDirectory().Data());

  std::ifstream configFile {config.Data()};
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

  std::vector<FileInfo*> *FileInfo = (fFileType == kSignal) ? &fSignalFileInfo : &fMCFileInfo;

  std::vector<UInt_t> SplitSamples;

  while (!configFile.eof()) {

    Message(eDebug, "About to read line.");
    Message(eDebug, "currTreeName:    %s", currTreeName.Data());
    Message(eDebug, "currLegend:      %s", currLegend.Data());
    Message(eDebug, "currColorStyle:  %s", currColorStyle.Data());
    Message(eDebug, "Now reading...");

    configFile >> LimitTreeName;

    Message(eDebug, "LimitTreeName:   %s", LimitTreeName.Data());

    if (LimitTreeName == ".")
      LimitTreeName = currTreeName;

    else if (!(LimitTreeName == "#." || LimitTreeName == "INGROUP" || LimitTreeName == "ENDGROUP")) {

      if (LimitTreeName.BeginsWith('#'))
        currTreeName = TString(LimitTreeName.Strip(TString::kLeading, '#'));

      else
        currTreeName = LimitTreeName;

    }

    Message(eDebug, "Processed tree name, now: %s", LimitTreeName.Data());

    // Do the checking here for merging groups
    if (LimitTreeName == "INGROUP") {

      SplitSamples.push_back((*FileInfo).size());
      Message(eDebug, "Starting merged group. Split at: %i, Size: %i",
              (*FileInfo).size(), SplitSamples.size());

    } else if (LimitTreeName == "ENDGROUP") {

      Message(eDebug, "Group ended.");

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

    } else {

      configFile >> FileName;
      Message(eDebug, "FileName:        %s", FileName.Data());

      if (LimitTreeName == "skip") {

        Message(eInfo, "Skipping files!");

        if (!fKeepAllFiles) {

          for (UInt_t iFile = 0; iFile != (*FileInfo).size(); ++iFile) {

            Message(eDebug, "File: %i/%i comparing %s to %s", iFile, (*FileInfo).size(),
                    ((*FileInfo)[iFile]->fFileName).Data(), (AddInDir(FileName)).Data());

            if ((*FileInfo)[iFile]->fFileName == AddInDir(FileName)) {

              Message(eInfo, "Removing file %s", (*FileInfo)[iFile]->fFileName.Data());
              delete (*FileInfo)[iFile];
              (*FileInfo).erase((*FileInfo).begin() + iFile);
              break;

            }

          }

        }

      } else {

        configFile >> XSec >> LegendEntry >> ColorStyleEntry;

        if (LegendEntry == ".")
          LegendEntry = currLegend;

        else
          currLegend = LegendEntry;

        if (ColorStyleEntry == ".")
          ColorStyleEntry = currColorStyle;

        else if (ColorStyleEntry == "rgb") {

          ++newColors;
          ColorStyleEntry = TString::Format("%i", 5000 + newColors);
          currColorStyle = ColorStyleEntry;
          configFile >> red >> green >> blue;
          TColor* setColor = new TColor(ColorStyleEntry.Atoi(),red.Atof()/255,green.Atof()/255,blue.Atof()/255);

        } else

          currColorStyle = ColorStyleEntry;

        Message(eDebug, "XSec:            %s", XSec.Data());
        Message(eDebug, "LegendEntry:     %s", LegendEntry.Data());
        Message(eDebug, "ColorStyleEntry: %s", ColorStyleEntry.Data());

        if (ColorStyleEntry != "" && (fKeepAllFiles || !LimitTreeName.BeginsWith('#')))
          // Replace _ with spaces, but put the escaped ones back
          AddFile(LimitTreeName, FileName, XSec.Atof(),
                  LegendEntry.ReplaceAll("_", " ").ReplaceAll("\\ ", "_"),
                  ColorStyleEntry.Atoi());

      }

    }

  }
}

//--------------------------------------------------------------------
std::vector<TH1D*>
FileConfigReader::GetHistList(Int_t NumXBins, Double_t *XBins, std::vector<TString> FileList, FileType type)
{

  DisplayFunc(__func__);

  Message(eDebug, "Number of Files to plot: %i", FileList.size());

  std::vector<TString> theFileNames = FileList;
  OpenFiles(theFileNames);

  std::vector<FileInfo*> *theFileInfo = &fMCFileInfo;
  TCut tempCutHolder;
  TString tempExprHolder;

  if (type == kSignal)
    theFileInfo = &fSignalFileInfo;

  if (type == kData && fDataWeight != "") {

    tempCutHolder = fDefaultCut;
    SetDefaultWeight(tempCutHolder + fDataWeight);

  } else if (type != kData && fMCWeight != "") {

    tempCutHolder = fDefaultCut;
    SetDefaultWeight(tempCutHolder * fMCWeight);

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

      auto scale = fFitResult.find((*theFileInfo)[iFile]->fTreeName);
      if (scale != fFitResult.end()) {
        Message(eDebug, "Scaling %s by %f", scale->first.Data(), scale->second);
        theHists[iFileName]->Scale(scale->second);
      }

      ++iFileName;

    }
  }

  return theHists;
}

//--------------------------------------------------------------------
std::vector<TH1D*>
FileConfigReader::GetHistList(Int_t NumXBins, Double_t *XBins, FileType type,
                              TString matchName, SearchBy search, Bool_t match)
{

  DisplayFunc(__func__);
  std::vector<TString> theFileNames = ReturnFileNames(type, matchName, search, match);
  return GetHistList(NumXBins, XBins, theFileNames, type);

}

//--------------------------------------------------------------------
TH1D*
FileConfigReader::GetHist(std::vector<TH1D*> HistList)
{

  DisplayFunc(__func__);
  std::vector<TH1D*> theHists = HistList;

  Message(eDebug, "Number of histograms to merge: %i", theHists.size());

  if (theHists.size() == 0) {
    Message(eError, "The list of histograms called is empty.");
    exit(30);
  }

  TH1D *output = (TH1D*) theHists[0]->Clone();
  output->Reset("M");

  for (std::vector<TH1D*>::iterator iHist = theHists.begin(); iHist != theHists.end(); ++iHist)
    output->Add(*iHist);

  return output;

}

//--------------------------------------------------------------------
TH1D*
FileConfigReader::GetHist(Int_t NumXBins, Double_t *XBins, std::vector<TString> FileList, FileType type)
{
  DisplayFunc(__func__);
  return GetHist(GetHistList(NumXBins, XBins, FileList, type));
}

//--------------------------------------------------------------------
TH1D*
FileConfigReader::GetHist(Int_t NumXBins, Double_t *XBins, FileType type,
                          TString matchName, SearchBy search, Bool_t match)
{
  DisplayFunc(__func__);
  return GetHist(GetHistList(NumXBins, XBins, type, matchName, search, match));
}

//--------------------------------------------------------------------
void
FileConfigReader::OpenFiles(std::vector<TString> fileNames)
{
  DisplayFunc(__func__);
  fFiles.resize(0);
  fInTrees.resize(0);

  Message(eDebug, "Looking for tree named: %s", fTreeName.Data());

  for (std::vector<TString>::iterator iFile = fileNames.begin(); iFile != fileNames.end(); ++iFile) {

    TFile *tempFile = TFile::Open(iFile->Data());
    TTree *tempTree;
    if (fTreeName.Contains("/"))
      tempTree = (TTree*) tempFile->Get(fTreeName);
    else
      tempTree = (TTree*) tempFile->FindObjectAny(fTreeName);

    Message(eDebug, "File: %s, located at %p, has tree at %p", iFile->Data(), tempFile, tempTree);

    if (!tempTree) {
      Message(eError, "Tree not found in file!");
      Message(eError, "File: %s at %p", iFile->Data(), tempFile);
      Message(eError, "Tree: %s at %p", fTreeName.Data(), tempTree);
      exit(1010);
    }

    fFiles.push_back(tempFile);
    fInTrees.push_back(tempTree);

    Message(eDebug, "Number of files used: %i, Number of trees: %i", fFiles.size(), fInTrees.size());

  }

  fAllFiles.push_back(fFiles);
  Message(eDebug, "Total number of times files opened so far: %i", fAllFiles.size());
}

//--------------------------------------------------------------------
void
FileConfigReader::CloseFiles()
{
  DisplayFunc(__func__);
  for (UInt_t iFiles = 0; iFiles != fAllFiles.size(); ++iFiles) {
    for (UInt_t iFile = 0; iFile != fAllFiles[iFiles].size(); ++iFile) {
      Message(eDebug, "About to close: %s", fAllFiles[iFiles][iFile]->GetName());
      fAllFiles[iFiles][iFile]->Close();
    }
    fAllFiles[iFiles].clear();
  }

  fFiles.clear();
  fInTrees.clear();
  fAllFiles.clear();

}

//--------------------------------------------------------------------
void
FileConfigReader::NameTreesAfterLimits (FileType type)
{
  std::vector<FileInfo*>* fileInfo = GetFileInfo(type);
  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo)
    (*iInfo)->fEntry = (*iInfo)->fTreeName;
}

//--------------------------------------------------------------------
void
FileConfigReader::SetLegendColor(TString entry, Int_t color, FileType type)
{
  std::vector<FileInfo*>* fileInfo = GetFileInfo(type);
  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo) {
    if (entry == (*iInfo)->fEntry)
      (*iInfo)->fColorStyle = color;
  }
}

//--------------------------------------------------------------------
void
FileConfigReader::ScaleBackgrounds(TString entry, Double_t scale, SearchBy search, FileType type)
{

  DisplayFunc(__func__);

  Message(eDebug, "Scaling %s by %f", entry.Data(), scale);

  std::vector<FileInfo*>* fileInfo = GetFileInfo(type);
  for (std::vector<FileInfo*>::iterator iInfo = fileInfo->begin(); iInfo != fileInfo->end(); ++iInfo) {

    TString match = (search == kLimitName) ? (*iInfo)->fTreeName : (*iInfo)->fEntry;
    if (match == entry) {
      Message(eDebug, "File %s XSec before %f", (*iInfo)->fFileName.Data(), (*iInfo)->fXSecWeight);
      (*iInfo)->fXSecWeight *= (1.0 + scale);
      Message(eDebug, "File %s XSec after  %f", (*iInfo)->fFileName.Data(), (*iInfo)->fXSecWeight);
    }

  }

}

//--------------------------------------------------------------------
void
FileConfigReader::LoadFitResult(const char* fit_result) {

  DisplayFunc(__func__);

  // Clear out previous loaded results
  fFitResult.clear();

  // Open file
  std::ifstream fitFile {fit_result};

  // Read it in
  TString key;
  double value;

  while (!fitFile.eof()) {
    fitFile >> key >> value;

    if (key != "") {
      Message(eDebug, "Key: %s, Value: %f", key.Data(), value);
      fFitResult[key] = value;
    }
  }
}

#endif
