/**
  @file   TmvaClassifier.h

  This is the header file that defines the TmvaClassifier class.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_ANALYSISTOOLS_TMVACLASSIFIER_H
#define CROMBIETOOLS_ANALYSISTOOLS_TMVACLASSIFIER_H

#include <vector>

#include "TString.h"

/**
   @ingroup analysisgroup
   @class TmvaClassifier
   @brief This class does the classification with BDTs.
*/

class TmvaClassifier
{
 public:
  TmvaClassifier();
  virtual ~TmvaClassifier();
  
  void   SetSignalCut( TString cut )                      { fSignalCut = cut;                                     }
  void   SetBackgroundCut( TString cut )                  { fBackgroundCut = cut;                                 }
  
  void   SetWeight( TString weight )                      { fWeight = weight;                                     }
  
  void   AddSignalTree( TString file, TString tree )      { fSignalFileNames.push_back(file);
                                                            fSignalTreeNames.push_back(tree);                     }
  void   AddBackgroundTree( TString file, TString tree )  { fBackgroundFileNames.push_back(file);
                                                            fBackgroundTreeNames.push_back(tree);                 }
  
  void   SetJobName( TString name )                       { fJobName = name;                                      }
  void   SetMethodName( TString name )                    { fMethodName = name;                                   }
  void   SetBDTDef( TString def )                         { fBDTDef = def;                                        }
  void   SetBDTName( TString name )                       { fBDTName = name;                                      }
  void   SetOutputName( TString name )                    { fOutputName = name;                                   }
  
  TString  GetOutputName()                                { return fOutputName;                                   }
  TString  GetMethodName()                                { return fMethodName;                                   }
  TString  GetUniformVariable()                           { return fUniformVariable;                              }
  TString  GetWeight()                                    { return fWeight;                                       }
  
  void   SetConfigFile( TString name );
  
  void   SetUniformVariable( TString var )                { fUniformVariable = var;                               }
  void   AddVariable( TString var, char type = 'F' )      { fVariables.push_back(var); fVarTypes.push_back(type); }
  void   AddSpectatorVariable( TString var )              { fSpectatorVariables.push_back(var);                   }
  
  void   TmvaClassify();
  
  void   SetApplicationDirectory( TString dir )           { fApplicationDirectory = dir;                          }
  void   SetApplicationTree( TString tree )               { fApplicationTree = tree;                              }
  void   SetApplicationOutput( TString dir )              { fApplicationOutput = dir;                             }
  
  TString  GetConfigFile()                                { return fConfigFile;                                   }
  
  TString  GetApplicationDirectory()                      { return fApplicationDirectory;                         }
  TString  GetApplicationTree()                           { return fApplicationTree;                              }
  TString  GetApplicationOutput()                         { return fApplicationOutput;                            }
  
  TString  GetWeightsFile()        { return TString("weights/")+fJobName+TString("_")+fMethodName+".weights.xml"; }
  
  void   SetReportFrequency( Int_t freq )                 { fReportFrequency = freq;                              }
  
  void   Apply();
  void   Apply( Int_t NumBins, Double_t *VarVals, Int_t NumMapPoints = 501 );
  void   Apply( Int_t NumBins, Double_t VarMin, Double_t VarMax, Int_t NumMapPoints = 501 );
  
 private:
  
  TString              fSignalCut;
  TString              fBackgroundCut;
  
  TString              fWeight;
  
  std::vector<TString> fSignalFileNames;
  std::vector<TString> fSignalTreeNames;
  std::vector<TString> fBackgroundFileNames;
  std::vector<TString> fBackgroundTreeNames;
  
  TString              fJobName;
  TString              fMethodName;
  TString              fBDTDef;
  TString              fBDTName;
  TString              fOutputName;
  
  TString              fConfigFile;
  
  TString              fUniformVariable;
  std::vector<TString> fVariables;
  std::vector<char>    fVarTypes;
  std::vector<TString> fSpectatorVariables;
  
  TString              fApplicationDirectory;
  TString              fApplicationTree;
  TString              fApplicationOutput;
  
  Int_t                fReportFrequency;
  
  ClassDef(TmvaClassifier,1)
};

#endif
