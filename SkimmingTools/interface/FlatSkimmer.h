#ifndef CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H

#include <vector>
#include <set>

#include "TString.h"

#include "InDirectoryHolder.h"
#include "GoodLumiFilter.h"

class FlatSkimmer : public InDirectoryHolder
{
 public:
  FlatSkimmer();
  virtual ~FlatSkimmer() {};
  
  void         SetGoodLumiFilter    ( GoodLumiFilter *filter )   { fGoodLumiFilter = *filter;    }
  void         SetOutDirectory      ( TString dir )              { fOutDirectory = dir;          }
  void         SetCut               ( TString cut )              { fCut = cut;                   }
  void         SetTreeName          ( TString name )             { fTreeName = name;             }
  void         SetRunExpr           ( TString expr )             { fRunExpr = expr;              }
  void         SetLumiExpr          ( TString expr )             { fLumiExpr = expr;             }
  void         SetEventExpr         ( TString expr )             { fEventExpr = expr;            }
  void         SetReportFrequency   ( Int_t freq )               { fReportFreq = freq;           }
  void         SetCheckDuplicates   ( Bool_t check )             { fCheckDuplicates = check;     }
  void         AddCopyObject        ( TString name )             { fCopyObjects.push_back(name); }
  void         Slim                 ( TString fileName );
  void         AddEventFilter       ( TString filterName );

  TString      GetOutDirectory      ()                           { return fOutDirectory;         }

  void         RunOnFile            ( TString name )             { Slim(name);                   }
  
 private:
  std::set<TString>    fEventFilter;      ///< Events to skim out.
  GoodLumiFilter       fGoodLumiFilter;   ///< GoodLumiFilter for this FlatSkimmer.
  TString              fOutDirectory;
  TString              fCut;
  TString              fTreeName;
  TString              fRunExpr;
  TString              fLumiExpr;
  TString              fEventExpr;
  Int_t                fReportFreq;
  Int_t                fCheckDuplicates;
  std::vector<TString> fCopyObjects;
  
  ClassDef(FlatSkimmer,1)
};

#endif
