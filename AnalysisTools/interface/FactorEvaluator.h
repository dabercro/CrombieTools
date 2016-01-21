#ifndef CROMBIETOOLS_SKIMMINGTOOLS_FACTOREVALUATOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_FACTOREVALUATOR_H

#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"

class FactorEvaluator
{
 public:
  FactorEvaluator( TString name );
  virtual ~FactorEvaluator();

  TString               GetName           ()                    { return fName;                          }

  void                  AddInExpression   ( TString expres )    { fInExpressions.push_back(expres);      }
  void                  SetInCut          ( TString cut )       { fInCut = cut;                          }
  void                  SetInTree         ( TTree* tree )       { fInTree = tree; InitializeTree();      } 

  void                  SetCorrectionFile ( TString fileName )  { fCorrectionFile = new TFile(fileName); }

  virtual   Float_t     Evaluate          ()         const      { return 1.0; }

 protected:

  TString               fName;
  
  TTree*                fInTree;
  void                  EvalExpressions   ();
  Bool_t                fCutPass;
  std::vector<Double_t> fExpressionEval;

  TFile*                fCorrectionFile;
  void                  InitializeTree    ();
  virtual    void       LoadCorrectorInfo ()   {}

 private:
  TString                     fInCut;
  TTreeFormula*               fCutFormula;
  std::vector<TString>        fInExpressions;
  std::vector<TTreeFormula*>  fFormulas;
};

//--------------------------------------------------------------------
FactorEvaluator::FactorEvaluator( TString name ) :
  fName(name),
  fInTree(NULL),
  fCutPass(false),
  fCorrectionFile(NULL),
  fInCut('1'),
  fCutFormula(NULL)
{
  fExpressionEval.resize(0);
  fInExpressions.resize(0);
  fFormulas.resize(0);
}
  
//--------------------------------------------------------------------
FactorEvaluator::~FactorEvaluator()
{
  if (fCorrectionFile != NULL) {
    if (fCorrectionFile->IsOpen())
      fCorrectionFile->Close();
  }

  if (fCutFormula != NULL)
    delete fCutFormula;
  
  for (UInt_t iFormula = 0; iFormula != fFormulas.size(); ++iFormula)
    delete fFormulas[iFormula];
}

//--------------------------------------------------------------------
void
FactorEvaluator::EvalExpressions()
{
  fCutPass = fCutFormula->EvalInstance<Bool_t>();
  for (UInt_t iExpr = 0; iExpr != fInExpressions.size(); ++iExpr) {
    fExpressionEval[iExpr] = fFormulas[iExpr]->EvalInstance();
  }
}

//--------------------------------------------------------------------
void
FactorEvaluator::InitializeTree()
{
  fCutFormula = new TTreeFormula(fInCut,fInCut,fInTree);

  TTreeFormula* tempFormula;
  for (UInt_t iExpression = 0; iExpression != fInExpressions.size(); ++iExpression) {
    fExpressionEval.push_back(0.0);
    tempFormula = new TTreeFormula(fInExpressions[iExpression],fInExpressions[iExpression],fInTree);
    fFormulas.push_back(tempFormula);
  }

  LoadCorrectorInfo();
}

#endif
