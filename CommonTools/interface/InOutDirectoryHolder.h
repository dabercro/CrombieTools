/**
  @file   InOutDirectoryHolder.h
  File that defines the InOutDirectoryHolder class
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_INOUTDIRECTORYHOLDER_H
#define CROMBIETOOLS_COMMONTOOLS_INOUTDIRECTORYHOLDER_H

#include "TString.h"
#include "InDirectoryHolder.h"

/**
n   @ingroup commongroup
   @class InOutDirectoryHolder
   This class is used to facilitate writing files to a directory.

   Main purpose is to centralize function of adding output directory to the filename
   when considering said file.
*/

class InOutDirectoryHolder : public InDirectoryHolder
{
 public:
  InOutDirectoryHolder() {};
  virtual ~InOutDirectoryHolder() {};

  /// Sets the output directory where the files will be written, and adds a "/" if needed.
  inline void       SetOutDirectory     ( TString dir )      { fOutDirectory = dir.EndsWith("/") ? dir : dir + "/"; }
  /// @returns the output directory
  inline TString    GetOutDirectory     ()  const            { return fOutDirectory;                                }
  /// Sets whether or not to overwrite files
  inline void       SetOverwriteFiles   ( Bool_t overwrite ) { fOverwriteFiles = overwrite;                         }

 protected:
  /**
     A helper function that prepends the output directory to a filename.
     @param FileName is the name of a file inside
                     the fOutDirectory or an absolute path.
     @returns FileName with the output directory prepended,
              unless absolute or fOutDirectory is empty,
              where it is left alone.
  */
  TString           AddOutDir           ( TString FileName ) const;

 private:
  TString    fOutDirectory = "";        ///< Stores the output directory, and always ends with a '/'
  Bool_t     fOverwriteFiles = true;    ///< Sets whether or not to overwrite output files
};

//--------------------------------------------------------------------

TString
InOutDirectoryHolder::AddOutDir(TString FileName) const {

  TString output = AddDirectory(fOutDirectory, FileName);

  if (!fOverwriteFiles) {

    struct stat buffer;
    int code = stat(output.Data(), &buffer);
    if (code == 0) {
      std::cerr << "File already exists " << output << "!" << std::endl;
      exit(21);
    }

  }

  return output;
}


#endif
