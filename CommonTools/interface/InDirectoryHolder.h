/**
   @file InDirectoryHolder.h
   Header file that defines InDirectoryHolder class.
   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_INDIRECTORYHOLDER_H
#define CROMBIETOOLS_COMMONTOOLS_INDIRECTORYHOLDER_H

#include <sys/stat.h>
#include <iostream>

#include "TString.h"

//--------------------------------------------------------------------

/**
   @ingroup commongroup
   Helper function that prepends a directory name to a file.
*/

TString AddDirectory(TString dir, TString FileName)
{
  if (dir == "" || FileName.BeginsWith("/"))
    return FileName;
  else
    return dir + FileName;
}

/**
   @ingroup commongroup
   @class InDirectoryHolder
   This class is used to facilitate running over files in a directory.
   
   Main purpose is to centralize function of adding input directory to the filename
   when considering said file.
*/

class InDirectoryHolder
{
 public:
  InDirectoryHolder() {};
  virtual ~InDirectoryHolder() {};

  /// Sets the input directory where the MCFiles will be searched for, and adds a "/" if needed.
  inline void       SetInDirectory     ( TString dir )      { fInDirectory = dir.EndsWith("/") ? dir : dir + "/"; }
  /// @returns the input directory
  inline TString    GetInDirectory     ()  const            { return fInDirectory;                                }

 protected:
  /**
     A helper function that prepends the input directory to a filename.
     @param FileName is the name of a file inside 
                     the fInDirectory or an absolute path.
     @returns FileName with the input directory prepended,
              unless absolute or fInDirectory is empty, 
              where it is left alone. 
  */
  TString    AddInDir                  ( TString FileName ) const;

 private:
  TString    fInDirectory = "";        ///< Stores the input directory
};

//--------------------------------------------------------------------
TString
InDirectoryHolder::AddInDir(TString FileName) const {

  struct stat buffer;
  TString output = AddDirectory(fInDirectory, FileName);

  int code = stat(output.Data(), &buffer);
  if (code != 0) {
    std::cerr << "Cannot find file " << output << "!" << std::endl;
    std::cerr << "Status code: " << code << std::endl;
    exit(20);
  }

  return output;
}

#endif
