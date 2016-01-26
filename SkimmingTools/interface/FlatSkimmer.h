#ifndef CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H

#include <vector>
#include <set>

#include "TString.h"

#include "GoodLumiFilter.h"

class FlatSkimmer
{
 public:
  FlatSkimmer();
  virtual ~FlatSkimmer()  {}
  
  void        SetGoodLumiFilter    ( GoodLumiFilter *filter )   { fGoodLumiFilter = filter;     }
  void        SetInDirectory       ( TString dir )              { fInDirectory = dir;           }
  void        SetOutDirectory      ( TString dir )              { fOutDirectory = dir;          }
  void        SetCut               ( TString cut )              { fCut = cut;                   }
  void        SetTreeName          ( TString name )             { fTreeName = name;             }
  void        SetRunExpr           ( TString expr )             { fRunExpr = expr;              }
  void        SetLumiExpr          ( TString expr )             { fLumiExpr = expr;             }
  void        SetEventExpr         ( TString expr )             { fEventExpr = expr;            }
  void        SetReportFrequency   ( Int_t freq )               { fReportFreq = freq;           }
  void        SetCheckDuplicates   ( Bool_t check )             { fCheckDuplicates = check;     }
  void        AddCopyObject        ( TString name )             { fCopyObjects.push_back(name); }
  void        Slim                 ( TString fileName );
  void        AddEventFilter       ( TString filterName );
  
 private:
  std::set<TString>    fEventFilter;
  GoodLumiFilter      *fGoodLumiFilter;
  TString              fInDirectory;
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
