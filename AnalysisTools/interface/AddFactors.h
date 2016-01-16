#ifndef CROMBIETOOLS_SKIMMINGTOOLS_ADDFACTORS_H
#define CROMBIETOOLS_SKIMMINGTOOLS_ADDFACTORS_H

#include <vector>
#include "TString.h"

enum ReweightType { kHist = 0, kMatchSpectrum }

struct CorrectionInfo
{
  CorrectionInfo ( ReweightType type, TString filename, TString objectname, TString weight, 
                   std::vector<TString> varnames )
   { fType = type; fFileName = filename; fObjectName = objectname; fWeight = weight; fVarNames = varnames; }
  virtual ~CorrectionInfo  ()    {}

  ReweightType         fType;
  TString              fFileName;
  TString              fObjectName;
  TString              fWeight;
  std::vector<TString> fVarNames;
}

class AddFactors
{
 public:
  AddFactors();
  virtual ~AddFactors()  {}
  
  void      SetInTreeName             ( TString name )       { fInTreeName = name;                         }
  void      SetOutTreeName            ( TString name )       { fOutTreeName = name;                        }

  void      SetReweightType           ( ReweightType type )  { fReweightType = type;                       }
  void      AddCorrectionFileName     ( TString name )       { fCorrectionFileName = name;                 }
  void      SetCorrectionObjectName   ( TString name )       { fCorrectionObjectName = name;               }
  void      SetCorrectionWeight       ( TString weight )     { fCorrectionWeight = weight;                 }
  void      AddVariableName           ( TString var )        { fVariableNames.push_back(var);              }
  void      PushBackCorrection        ( TString outname );

  void      SetOverallCorrection      ( TString name, Bool_t only = false ) 
                                                     { fOverallCorrectionName = name; fOnlyOverall = only; }
  
  void      AddFactorToFile           ( TString file );

 private:

  TString                      fInTreeName;
  TString                      fOutTreeName;

  ReweightType                 fReweightType;
  TString                      fCorrectionFileName;
  TString                      fCorrectionObjectName;
  TString                      fCorrectionWeight;
  std::vector<TString>         fVariableNames;

  std::vector<TString>         fCorrectionNames;
  std::vector<CorrectionInfo>  fInfos;

  TString                      fOverallCorrectionName;
  Bool_t                       fOnlyOverall;

  ClassDef(AddFactors,1)
};

#endif
