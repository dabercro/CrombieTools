#ifndef CROMBIETOOLS_SKIMMINGTOOLS_CORRECTOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_CORRECTOR_H

#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TH1.h"

class Corrector
{
 public:
  Corrector( TString name );
  virtual ~Corrector();

  TString               GetName           ()                    { return fName;                                    }

  void                  AddInExpression   ( TString expres )    { fInExpressions.push_back(expres); ++fNumDims;    }
  void                  SetInCut          ( TString cut )       { fInCut = cut;                                    }
  void                  SetInTree         ( TTree* tree )       { fInTree = tree; InitializeTree();                } 

  void                  SetCorrectionFile ( TString fileName )  { fCorrectionFile = new TFile(fileName);           }
  virtual   void        SetCorrectionHist ( TString histName )  { fInHist = (TH1*) fCorrectionFile->Get(histName); }

  Float_t               Evaluate          ();

 protected:

  TString               fName;
  
  TTree*                fInTree;

  TFile*                fCorrectionFile;
  void                  InitializeTree    ();

 private:
  Int_t                       fNumDims;
  TH1*                        fInHist;
  TString                     fInCut;
  TTreeFormula*               fCutFormula;
  std::vector<TString>        fInExpressions;
  std::vector<TTreeFormula*>  fFormulas;

  ClassDef(Corrector,1)
};

#endif
