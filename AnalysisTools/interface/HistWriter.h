#ifndef CROMBIETOOLS_ANALYSISTOOLS_HISTWRITER_H
#define CROMBIETOOLS_ANALYSISTOOLS_HISTWRITER_H

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

 private:
  TString               fFileName;
  TString               fHistName;

  ClassDef(HistWriter,1)
};

#endif
