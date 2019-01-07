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
{}

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
  method = reader->BookMVA(fName, fWeightsFile);
}

Float_t TMVACorrector::DoEval() {
  for (decltype(GetNumDims()) iDim = 0; iDim < GetNumDims(); ++iDim)
    fFormulaResults[iDim] = GetFormulaResult(iDim, false);

  Float_t output = method->GetMvaValue();
  return output;
}

//--------------------------------------------------------------------
TMVACorrector* TMVACorrector::Copy()
{
  TMVACorrector *newTMVACorrector = new TMVACorrector(fName, fWeightsFile);
  auto* newreader = newTMVACorrector->reader;
  *newTMVACorrector = *this;
  newTMVACorrector->reader = newreader;
  for (decltype(GetNumDims()) iDim = 0; iDim < GetNumDims(); ++iDim)
    newTMVACorrector->reader->AddVariable(fVarNames[iDim], &(newTMVACorrector->fFormulaResults[iDim]));
  newTMVACorrector->method = newTMVACorrector->reader->BookMVA(fName, fWeightsFile);

  newTMVACorrector->fIsCopy = true;
  return newTMVACorrector;
}
