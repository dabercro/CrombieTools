#ifndef MITPLOTS_PLOT_PLOTSTACK_H
#define MITPLOTS_PLOT_PLOTSTACK_H

#include "TreeContainer.h"
#include "PlotHists.h"

struct HistHolder
{
  HistHolder(TH1 *hist, TString entry, Color_t color) 
            { fHist = hist; fEntry = entry; fColor = color; }
  TH1 *fHist;
  TString fEntry;
  Color_t fColor;
}

class PlotStack : private PlotHists
{
 public:
  PlotStack();
  virtual ~PlotStack();
  
  void SetTreeName   ( TString name )             { fTreeName = name;                           }
  void AddFriend     ( TString name )             { fFriends.push_back(name);                   }
  
  void SetJSON       ( TString json )             { fJSON = json;                               }
  
  void SetWeight     ( TString weight )           { SetDefaultWeight(weight);                   }
  void SetAllHist    ( TString name )             { fAllHist = name;                            }
  void SetVariable   ( TString var )              { SetDefaultExpr(var);                        }
  void SetLuminosity ( Double_t lum )             { fLuminosity = lum;                          }
  
  void AddDataFile   ( TString FileName )         { fDataFiles.push_back(FileName);             }
  void AddMCFile     ( TString FileName, Double_t XSec, TString LegendEntry, Color_t ColorEntry )
                                    { fMCFiles.push_back(FileName); fXSecs.push_back(XSec); 
                                      fStackEntries.push_back(LegendEntry); 
				      fStackColors.push_back(ColorEntry);      }
  
  void PlaceLegend   ( Double_t lim1, Double_t lim2, Double_t lim3, Double_t lim4 )
                                                  { SetLegendLimits(lim1, lim2, lim3, lim4);    }
  
  void Plot          ( Int_t NumXBins, Double_t *XBins, TString FileBase,
                       TString XLabel, TString YLabel, Bool_t logY = false );
  
  void Plot          ( Int_t NumXBins, Double_t MinX, Double_t MaxX, TString FileBase,
                       TString XLabel, TString YLabel, Bool_t logY = false );
  
  void SetDebug      ( Bool_t debug )              { fDebug = debug;                            }
  
 private:
  
  std::vector<TH1D*>    GetHistList   (std::vector<TString> FileList, Int_t NumXBins, Double_t *XBins, Bool_t isMC);
  
  TString               fTreeName;
  std::vector<TString>  fFriends;
  TString               fJSON;
  TString               fAllHist;
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

