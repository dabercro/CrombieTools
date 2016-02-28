/**
   @file InDirectoryHolder.h
   Header file that defines InDirectoryHolder class.
   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_COMMONTOOLS_INDIRECTORYHOLDER_H
#define CROMBIETOOLS_COMMONTOOLS_INDIRECTORYHOLDER_H

#include "TString.h"

/**
   @class InDirectoryHolder
   This class is used to facilitate running over files in a directory.
   
   Main purpose is to centralize function of adding input directory to the filename
   when considering said file. */

class InDirectoryHolder
{
 public:
  InDirectoryHolder() {};
  virtual ~InDirectoryHolder() {};

  /// Sets the input directory where the MCFiles will be searched for, appending a "/" if missing.
  inline void       SetInDirectory       ( TString dir )    { fInDirectory = dir.EndsWith("/") ? dir : dir + "/"; }
  /// @returns the input directory
  inline TString    GetInDirectory       ()                 { return fInDirectory;                                }

 private:
  TString    fInDirectory = "";                                  ///< Stores the input directory

 protected:
  inline TString    AddInDir             ( TString FileName );   ///< A helper function that prepends the input directory to a filename
};

//--------------------------------------------------------------------

/**
   @param FileName is the name of a file inside 
   the fInDirectory or an absolute path.
   @returns FileName with the input directory prepended,
   unless absolute or fInDirectory is empty, 
   where it is left alone. */

TString InDirectoryHolder::AddInDir(TString FileName)
{
  if (fInDirectory != "" && !FileName.BeginsWith('/'))
    return fInDirectory + FileName;
  else
    return FileName;
}

#endif
