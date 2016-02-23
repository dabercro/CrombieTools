#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H

#include "TreeContainer.h"
#include "PlotHists.h"
#include "HistHolder.h"
#include "MCReader.h"

class PlotStack : public PlotHists , public MCReader
{
 public:
  PlotStack();
  virtual ~PlotStack();
  
  // Set the tree of the files you are trying to plot and add friends in same file
  void SetTreeName       ( TString name )             { fTreeName = name;                        }
  void AddFriend         ( TString name )             { fFriends.push_back(name);                }
  
  // Add files as data
  void AddDataFile       ( TString FileName )         { fDataFiles.push_back(FileName);          }

  // Alternatively, read a central MC configuration file
  void UseLimitTree      ( TString limitFile, TString region, TString mcConfig, TString signalConfig = "" );

  // The multipliers for Data can be set separately
  void SetDataWeights    ( TString weight )           { fDataWeights = weight;                   }
  // The multipliers for MC can be set separately
  void SetMCWeights      ( TString weight )           { fMCWeights = weight;                     }
  
  // Choose the binning of your plots and make
  void MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                           TString XLabel, TString YLabel, Bool_t logY = false );
  
  void MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                           TString XLabel, TString YLabel, Bool_t logY = false );
  
  void SetForceTop       ( TString force )            { fForceTop = force;                       }
  void SetMinLegendFrac  ( Double_t frac )            { fMinLegendFrac = frac;                   }
  void SetIgnoreInLinear ( Double_t ignore )          { fIgnoreInLinear = ignore;                }
  void SetOthersColor    ( Color_t color )            { fOthersColor = color;                    }
  void SetStackLineWidth ( Int_t width )              { fStackLineWidth = width;                 }

  void SetUsingLumi      ( Bool_t isUsed )            { fUsingLumi = isUsed;                     }

  void SetOutDirectory   ( TString dir )  { fOutDirectory = dir.EndsWith("/") ? dir : dir + "/"; }
  void SetLimitTreeDir   ( TString dir )  { fLimitTreeDir = dir.EndsWith("/") ? dir : dir + "/"; }              
  // This dumps out some raw values for you to check yields
  void SetDebug          ( Bool_t debug )             { fDebug = debug;                          }
  void SetDumpFileName   ( TString dumpName )         { fDumpRootName = dumpName;                }
  
 protected:
  // Draws histograms for the object
  enum HistType { kData = 0, kMC, kSignal };
  std::vector<TH1D*>    GetHistList            ( Int_t NumXBins, Double_t *XBins, HistType type);

 private:
  TString               fTreeName;                  // Stores name of tree from file
  std::vector<TString>  fFriends;                   // Stores list of friends
  std::vector<TString>  fDataFiles;                 // List of data files
  
  TreeContainer        *fDataContainer;             // A TreeContainer for data
  TreeContainer        *fMCContainer;               // A TreeContainer for MC
  TreeContainer        *fSignalContainer;           // A TreeContainer for Signal MC
  TString               fDataWeights;               // Separate Data weights if needed
  TString               fMCWeights;                 // Separate MC weights if needed
  TString               fForceTop;
  Double_t              fMinLegendFrac;
  Double_t              fIgnoreInLinear;
  Int_t                 fStackLineWidth;
  Color_t               fOthersColor;
  Bool_t                fUsingLumi;
  TFile                *fLimitFile;
  TString               fLimitRegion;
  TString               fOutDirectory;
  TString               fLimitTreeDir;

  Bool_t                fDebug;                     // Dumps yield tests
  TString               fDumpRootName;              // File where each histogram in stack is dumped

  ClassDef(PlotStack,1)
};

#endif
