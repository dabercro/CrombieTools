#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H

#include "TreeContainer.h"
#include "PlotHists.h"

struct HistHolder
{
  HistHolder         ( TH1D *hist, TString entry, Color_t color )
                                      { fHist = hist; fEntry = entry; fColor = color;
                                        fHist->SetFillColor(fColor); fHist->SetMarkerSize(0);   }
  virtual ~HistHolder()               {}
  TH1D *fHist;
  TString fEntry;
  Color_t fColor;
};

Bool_t
SortHistHolders ( HistHolder *h1, HistHolder *h2 )
{
  return h1->fHist->Integral() > h2->fHist->Integral(); 
}

class PlotStack : public PlotHists
{
 public:
  PlotStack();
  virtual ~PlotStack();
  
  void SetTreeName   ( TString name )             { fTreeName = name;                           }
  void AddFriend     ( TString name )             { fFriends.push_back(name);                   }
  
  void SetAllHist    ( TString name )             { fAllHist = name;                            }
  void SetLuminosity ( Double_t lum )             { fLuminosity = lum;                          }
  
  void AddDataFile   ( TString FileName )         { fDataFiles.push_back(FileName);             }
  void AddMCFile     ( TString FileName, Double_t XSec, TString LegendEntry, Color_t ColorEntry )
                                        { fMCFiles.push_back(FileName); 
                                          fXSecs.push_back(XSec); 
                                          fStackEntries.push_back(LegendEntry); 
                                          fStackColors.push_back(ColorEntry);                   }
  void ReadMCConfig  ( TString config, TString fileDir = "" );
  
  void MakeCanvas    ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                       TString XLabel, TString YLabel, Bool_t logY = false );
  
  void MakeCanvas    ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                       TString XLabel, TString YLabel, Bool_t logY = false );
  
  void SetDebug      ( Bool_t debug )              { fDebug = debug;                            }
  
 private:
  
  std::vector<TH1D*>    GetHistList            ( std::vector<TString> FileList, 
                                                 Int_t NumXBins, Double_t *XBins, Bool_t isMC);

  TString               fTreeName;
  std::vector<TString>  fFriends;
  TString               fAllHist;                   // Used to get the number of events in MC sample for normalization
  Double_t              fLuminosity;
  std::vector<TString>  fDataFiles;
  std::vector<TString>  fMCFiles;
  std::vector<Double_t> fXSecs;
  std::vector<TString>  fStackEntries;
  std::vector<Color_t>  fStackColors;
  
  TreeContainer        *fDataContainer;
  TreeContainer        *fMCContainer;

  Bool_t                fDebug;
  
  ClassDef(PlotStack,1)
};

#endif

