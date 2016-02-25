#ifndef CROMBIETOOLS_SKIMMINGTOOLS_HISTWRITER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_HISTWRITER_H

#include <vector>
#include "TString.h"

class HistWriter
{
 public:
  HistWriter()                                                   { HistWriter("output.root", "corrections");  }
  HistWriter( TString fileName, TString histName );
  virtual ~HistWriter();

  void                  SetFileName       ( TString fileName )   { fFileName = fileName;                      }
  void                  SetHistName       ( TString histName )   { fHistName = histName;                      }

  void                  MakeHist          ( TString configName );
  void                  MakeHist          ( TString fileName, TString histName, TString configName )
                                        { SetFileName(fileName); SetHistName(histName); MakeHist(configName); }

 private:
  TString               fFileName;
  TString               fHistName;

  ClassDef(HistWriter,1)
};

#endif
