#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "AddFactors.h"

ClassImp(AddFactors)

//--------------------------------------------------------------------
AddFactors::AddFactors() :
  fInTreeName("events"),
  fOutTreeName(""),
  fReweightType(kHist),
  fCorrectionFileName(""),
  fCorrectionObjectName(""),
  fCorrectionWeight(""),
  fOverallCorrectionName(""),
  fOnlyOverall(false)
{
  fVariableNames.resize(0);
  fCorrectionNames.resize(0);
  fInfos.resize(0);
}

//--------------------------------------------------------------------
AddFactors::PushBackCorrection(TString outname)
{
  fCorrectionNames.push_back(outname);
  fInfos.push_back(CorrectionInfo(fReweightType,
                                  fCorrectionFileName,
                                  fCorrectionObjectName,
                                  fCorrectionWeight,
                                  fVariableNames));
  fReweightType = kHist;
  fCorrectionFilename   = "";
  fCorrectionObjectName = "";
  fCorrectinoWeight     = "";
  fVariableNames.resize(0);
}

//--------------------------------------------------------------------
void
AddFactors::AddFactorToFile(TString file)
{
  // I still need to write this whole part...
  // I'll do that when I actually need it soon.
}
