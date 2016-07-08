#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H

#include "TreeContainer.h"
#include "PlotHists.h"
#include "FileConfigReader.h"

class PlotStack : public PlotHists , public FileConfigReader
{
 public:
  PlotStack();
  virtual ~PlotStack();

  /// Copy this PlotStack for parallelization
  PlotStack* Copy        ();

  /// Use the LimitTree to plot stacks directly instead of reading the files directly, bad for N-1
  void UseLimitTree      ( TString limitFile, TString region, TString mcConfig, TString signalConfig = "" );

  /// Choose the binning of your plots and make then.
  void MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                           TString XLabel, TString YLabel, Bool_t logY = false );
  
  /// Binning with fixed width
  void MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                           TString XLabel, TString YLabel, Bool_t logY = false );
  
  /// Set the tree of the files you are trying to plot and add friends in same file.
  void SetTreeName       ( TString name )             { fTreeName = name;                         }
  void AddFriend         ( TString name )             { fFriends.push_back(name);                 }
  
  /// The multipliers for Data can be set separately.
  void SetDataWeights    ( TString weight )           { fDataWeights = weight;                    }
  /// The multipliers for MC can be set separately.
  void SetMCWeights      ( TString weight )           { fMCWeights = weight;                      }
  
  void SetForceTop       ( TString force )            { fForceTop = force;                        }
  void SetMinLegendFrac  ( Double_t frac )            { fMinLegendFrac = frac;                    }
  void SetIgnoreInLinear ( Double_t ignore )          { fIgnoreInLinear = ignore;                 }
  void SetOthersColor    ( Color_t color )            { fOthersColor = color;                     }
  void SetStackLineWidth ( Int_t width )              { fStackLineWidth = width;                  }

  void SetUsingLumi      ( Bool_t isUsed )            { fUsingLumi = isUsed;                      }

  void SetLimitTreeDir   ( TString dir )   { fLimitTreeDir = dir.EndsWith("/") ? dir : dir + "/"; }              

  /// This dumps out some raw values for you to check yields.
  void SetDebug          ( Bool_t debug )             { fDebug = debug;                           }
  void SetDumpFileName   ( TString dumpName )         { fDumpRootName = dumpName;                 }

  /// Insert a histogram from file to use in place of the files, labelled by legend entry.
  void InsertTemplate    ( TString fileName, TString histName, TString LegendEntry )
                                                      { fTemplateFiles.push_back(fileName); 
                                                        fTemplateHists.push_back(histName); 
                                                        fTemplateEntries.push_back(LegendEntry);  }

  /// Use to suppress ratio pad if wanted
  void SetMakeRatio      ( Bool_t make )              { fMakeRatio = make;                        }

  /// Use this to change the automatic sorting of backgrounds based on yields
  void SetSortBackground ( Bool_t doSort )            { fSortBackground = doSort;                 }

 protected:
  /// Determines type of histogram being drawn.
  enum HistType { kData = 0, kMC, kSignal };
  /// Draws histograms for the object.
  std::vector<TH1D*>    GetHistList            ( Int_t NumXBins, Double_t *XBins, HistType type);

 private:
  TString               fTreeName;                  ///< Stores name of tree from file
  std::vector<TString>  fFriends;                   ///< Stores list of friends
  
  TreeContainer        *fDataContainer;             ///< A TreeContainer for data
  TreeContainer        *fMCContainer;               ///< A TreeContainer for MC
  TreeContainer        *fSignalContainer;           ///< A TreeContainer for Signal MC
  TString               fDataWeights;               ///< Separate Data weights if needed
  TString               fMCWeights;                 ///< Separate MC weights if needed
  TString               fForceTop;                  ///< A legend entry that is on the top of backgrounds
  Double_t              fMinLegendFrac;             ///< If a background contributes less than this, it's grouped in others
  Double_t              fIgnoreInLinear;            ///< If background less than this, do not draw in linear-scale plot
  Int_t                 fStackLineWidth;            ///< Line width of the histograms
  Color_t               fOthersColor;               ///< Color of the "Others" legend entry
  Bool_t                fUsingLumi;                 ///< Bool to determine if scaling should be done
  TFile                *fLimitFile;                 ///< File containing limit trees
  TString               fLimitRegion;               ///< Region to read from limit tree
  TString               fLimitTreeDir;              ///< Location of limit tree file

  std::vector<TString>  fTemplateFiles;             ///< Holds file name of the templates
  std::vector<TString>  fTemplateHists;             ///< Holds histogram name of the templates
  std::vector<TString>  fTemplateEntries;           ///< Holds legend entries of the templates

  Bool_t                fDebug;                     ///< Dumps yield tests
  TString               fDumpRootName;              ///< File where each histogram in stack is dumped
  Bool_t                fMakeRatio = true;          ///< Bool to make ratio pad
  Bool_t                fSortBackground = true;     ///< Bool to sort the backgrounds

  ClassDef(PlotStack,1)
};

#endif
