#ifndef CROMBIETOOLS_SKIMMINGTOOLS_CORRECTORAPPLICATOR_H
#define CROMBIETOOLS_SKIMMINGTOOLS_CORRECTORAPPLICATOR_H

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
  void                 SetReportFrequency   ( Int_t freq )                { fReportFrequency = freq;                }

  void                 ApplyCorrections     ( TString fileName );

 private:
  TString                   fName;
  Bool_t                    fSaveAll;
  TString                   fInputTreeName;
  TString                   fOutputTreeName;
  std::vector<Corrector*>   fCorrectors;
  Int_t                     fReportFrequency;

  ClassDef(CorrectorApplicator,1)
};

#endif
