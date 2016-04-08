/**
   @file FileConfigReader.h

   Defines the FileConfigReader class. Since the FileConfigReader class is never used alone,
   the entire class it defined in this header.

   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_COMMONTOOLS_FILECONFIGREADER_H
#define CROMBIETOOLS_COMMONTOOLS_FILECONFIGREADER_H

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
  /// @todo add some sort of DataFile addition
  enum FileType { kBackground = 0, kSignal, kData };

  /// Returns a vector of file names that have been read from the configs
  std::vector<TString> ReturnFileNames      ( FileType type = kBackground, TString limitName = "" );

  /// Returns a TChain of files that match the FileType and name for the LimitTreeMaker
  TChain*              ReturnTChain         ( TString treeName = "events", FileType type = kBackground, TString limitName = "" );

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
  FileType     fFileType = kBackground;                   ///< Type of files in the next config
  Bool_t       fKeepAllFiles = false;                     ///< Keeps FileInfo stored usually deleted by exception configs
  Bool_t       fMultiplyLumi = true;                      ///< Returns XSecWeight with luminosity multiplied
  std::vector<TObject*> fDeleteThese;                     ///< Vector of object pointers to free memory at the end
  
};

#endif
