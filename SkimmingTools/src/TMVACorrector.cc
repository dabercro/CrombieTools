/**
  @file   TMVACorrector.cc
  Describes the TMVACorrector class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <fstream>

#include "TMVACorrector.h"

ClassImp(TMVACorrector)

TMVACorrector::TMVACorrector(TString name, TString weights_file)
: Corrector(name),
  fWeightsFile{weights_file}
{
  reader->BookMVA(fName, fWeightsFile);
}

TMVACorrector::~TMVACorrector() {
  delete reader;
}

void TMVACorrector::ReadVarConfig(const char* config) {
  std::ifstream configFile {config};
  TString var_name;
  TString expr;

  while (!configFile.eof()) {
    configFile >> var_name;
    if (var_name == "SPEC")
      continue;

    configFile >> expr;
    if (expr != "") {
      fVarNames.push_back(var_name);
      AddInExpression(expr);
      fFormulaResults.push_back({});
      reader->AddVariable(var_name, &fFormulaResults.back());
    }
  }
}

Float_t TMVACorrector::DoEval() {
  for (decltype(GetNumDims()) iDim = 0; iDim < GetNumDims(); ++iDim)
    fFormulaResults[iDim] = GetFormulaResult(iDim, false);

  Float_t output = reader->EvaluateMVA(fName);
  return output;
}

//--------------------------------------------------------------------
TMVACorrector* TMVACorrector::Copy()
{
  TMVACorrector *newTMVACorrector = new TMVACorrector(fName, fWeightsFile);
  auto* reader = newTMVACorrector->reader;
  *newTMVACorrector = *this;
  newTMVACorrector->reader = reader;
  for (decltype(GetNumDims()) iDim = 0; iDim < GetNumDims(); ++iDim)
    newTMVACorrector->reader->AddVariable(fVarNames[iDim], &(newTMVACorrector->fFormulaResults[iDim]));

  newTMVACorrector->fIsCopy = true;
  return newTMVACorrector;
}
