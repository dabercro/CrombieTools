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
  Corrector()                                                   { Corrector("correction");                                 }
  Corrector( TString name );
  virtual ~Corrector();

  TString               GetName           ()                    { return fName;                                            }

  void                  AddInExpression   ( TString expres )    { fInExpressions.push_back(expres); ++fNumDims;            }
  void                  SetInCut          ( TString cut )       { fInCut = cut;                                            }
  void                  SetInTree         ( TTree* tree )       { fInTree = tree; InitializeTree();                        } 

  virtual   void        SetCorrectionFile ( TString fileName )  { fCorrectionFile = new TFile(fileName);                   }
  void                  SetCorrectionHist ( TString histName )  { fCorrectionHist = (TH1*) fCorrectionFile->Get(histName); }
  void                  SetCorrectionHist ( TString hist1, TString hist2 );

  Float_t               Evaluate          ();

 protected:

  TString               fName;
  
  TTree*                fInTree;
  void                  InitializeTree    ();

  TFile*                fCorrectionFile;
  TH1*                  fCorrectionHist;

 private:
  Int_t                       fNumDims;
  TString                     fInCut;
  TTreeFormula*               fCutFormula;
  std::vector<TString>        fInExpressions;
  std::vector<TTreeFormula*>  fFormulas;

  ClassDef(Corrector,1)
};

#endif
