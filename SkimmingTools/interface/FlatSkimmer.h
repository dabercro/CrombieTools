#ifndef CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_FLATSKIMMER_H

#include <vector>

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
  void        SetReportFrequency   ( Int_t freq )               { fReportFreq = freq;           }
  void        AddCopyObject        ( TString name )             { fCopyObjects.push_back(name); }
  void        Slim                 ( TString fileName );
  
 private:
  
  GoodLumiFilter      *fGoodLumiFilter;
  TString              fInDirectory;
  TString              fOutDirectory;
  TString              fCut;
  TString              fTreeName;
  TString              fRunExpr;
  TString              fLumiExpr;
  Int_t                fReportFreq;
  std::vector<TString> fCopyObjects;
  
  ClassDef(FlatSkimmer,1)
};

#endif
