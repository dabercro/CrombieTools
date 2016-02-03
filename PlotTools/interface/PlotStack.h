#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H

#include "TreeContainer.h"
#include "PlotHists.h"
#include "HistHolder.h"

class PlotStack : public PlotHists
{
 public:
  PlotStack();
  virtual ~PlotStack();
  
  // Set the tree of the files you are trying to plot and add friends in same file
  void SetTreeName       ( TString name )             { fTreeName = name;                        }
  void AddFriend         ( TString name )             { fFriends.push_back(name);                }
  
  // Set the all histogram and luminosity for normalization
  void SetAllHist        ( TString name )             { fAllHist = name;                         }
  void SetLuminosity     ( Double_t lum )             { fLuminosity = lum;                       }
  
  // Add files as either data or MC
  void AddDataFile       ( TString FileName )         { fDataFiles.push_back(FileName);          }
  void AddMCFile         ( TString FileName, Double_t XSec, 
                           TString LegendEntry, Color_t ColorEntry )
                                         { fMCFiles.push_back(FileName); 
                                           fXSecs.push_back(XSec); 
                                           fStackEntries.push_back(LegendEntry); 
                                           fStackColors.push_back(ColorEntry);                   }

  void AddSignalFile     ( TString FileName, Double_t XSec, 
                           TString LegendEntry, Int_t Style )
                                         { fSignalFiles.push_back(FileName); 
                                           fSignalXSecs.push_back(XSec); 
                                           fSignalEntries.push_back(LegendEntry); 
                                           fSignalStyles.push_back(Style);                       }


  // Alternatively, read a central MC configuration file
  void ReadMCConfig      ( TString config, TString fileDir = "" );

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
  void SetOthersColor    ( Color_t color )            { fOthersColor = color;                    }
  void SetStackLineWidth ( Int_t width )              { fStackLineWidth = width;                 }

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
  TString               fAllHist;                   // Used to get the number of events in MC sample for normalization
  Double_t              fLuminosity;                // Set luminosity in inverse pb
  std::vector<TString>  fDataFiles;                 // List of data files
  std::vector<TString>  fMCFiles;                   // List of MC files
  std::vector<Double_t> fXSecs;                     // List of MC cross sections
  std::vector<TString>  fStackEntries;              // List of legend entries for MC
  std::vector<Color_t>  fStackColors;               // List of legend colors for MC
  std::vector<TString>  fSignalFiles;
  std::vector<Double_t> fSignalXSecs;
  std::vector<TString>  fSignalEntries;
  std::vector<Color_t>  fSignalStyles;
  
  TreeContainer*        fDataContainer;             // A TreeContainer for data
  TreeContainer*        fMCContainer;               // A TreeContainer for MC
  TreeContainer*        fSignalContainer;           // A TreeContainer for Signal MC
  TString               fDataWeights;               // Separate Data weights if needed
  TString               fMCWeights;                 // Separate MC weights if needed
  TString               fForceTop;
  Double_t              fMinLegendFrac;
  Int_t                 fStackLineWidth;
  Color_t               fOthersColor;

  Bool_t                fDebug;                     // Dumps yield tests
  TString               fDumpRootName;              // File where each histogram in stack is dumped

  ClassDef(PlotStack,1)
};

#endif
