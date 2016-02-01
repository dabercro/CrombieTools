#ifndef CROMBIETOOLS_ANALYSISTOOLS_CORRECTORAPPLICATOR_H
#define CROMBIETOOLS_ANALYSISTOOLS_CORRECTORAPPLICATOR_H

#include <vector>
#include "TString.h"
#include "Corrector.h"

class CorrectorApplicator
{
 public:
  CorrectorApplicator()                                                   { CorrectorApplicator("", true);          }
  CorrectorApplicator( TString name, Bool_t saveAll );
  virtual ~CorrectorApplicator();

  void                 SetInputTreeName     ( TString tree )              { fInputTreeName = tree;                  }
  void                 SetOutputTreeName    ( TString tree )              { fOutputTreeName = tree;                 }
  void                 AddCorrector         ( Corrector* corrector )      { fCorrectors.push_back(corrector);       }
  void                 AddFactorToMerge     ( TString factor )            { fMergeFactors.push_back(factor);        }
  void                 SetReportFrequency   ( Int_t freq )                { fReportFrequency = freq;                }

  void                 ApplyCorrections     ( TString fileName );

 private:
  TString                   fName;
  Bool_t                    fSaveAll;
  TString                   fInputTreeName;
  TString                   fOutputTreeName;
  std::vector<Corrector*>   fCorrectors;
  std::vector<TString>      fMergeFactors;
  Int_t                     fReportFrequency;

  ClassDef(CorrectorApplicator,1)
};

#endif
