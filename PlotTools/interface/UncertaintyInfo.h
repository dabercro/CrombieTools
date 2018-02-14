/**
   @file UncertaintyInfo.h

   Defines the UncertaintyInfo class to apply to plots

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_UNCERTAINTYINFO_H
#define CROMBIETOOLS_COMMONTOOLS_UNCERTAINTYINFO_H

#include "TString.h"

#include "UncertaintyInfo.h"

/**
   @ingroup commongroup
   @struct UncertaintyInfo
   Structure holding information to apply uncertainties in certain plots.
   The Histogram pointed to is assumed to have values greater than 1.
   When applied via the ApplyUncertainy() function,
   1 is subtracted from the bin content to get the uncertainty.
   This is different than the Corrector approach to uncertainties.
*/

struct UncertaintyInfo
{
  /// The constructor fills all of the entries
  UncertaintyInfo ( TString UncVariable = "", TString FileName = "", TString HistName = "",
                    Int_t StartBin = 1, Int_t EndBin = 0 )
  : fUncVariable{UncVariable},
    fFileName{FileName},
    fHistName{HistName},
    fStartBin{StartBin},
    fEndBin{EndBin}
  {}
  virtual ~UncertaintyInfo()  {}

  TString fUncVariable;  ///< The variable that the uncertainty is a function of
  TString fFileName;     ///< The file that containes the uncertainty histogram
  TString fHistName;     ///< The uncertainty histogram name
  Int_t fStartBin;       ///< The first bin to read from the histogram
  Int_t fEndBin;         ///< The last bin to read from the histogram
};

#endif
