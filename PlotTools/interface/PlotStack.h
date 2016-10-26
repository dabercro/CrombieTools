/**
  @file   PlotStack.h

  Header of PlotStack class.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTSTACK_H

#include "FileConfigReader.h"

/**
   @ingroup plotgroup
   @class PlotStack
   Class used for making stack plots.
   Most easy way to use this class is through CrombieTools.PlotTools.PlotStack
   convenience functions and using an [MC Configuration](@ref formatmc).
   Many appearance flags must be set through this class's methods though.
*/

class PlotStack : public FileConfigReader
{
 public:
  PlotStack();
  virtual ~PlotStack();

  /// Copy this PlotStack for parallelization
  PlotStack* Copy        ();

  /// Choose the binning of your plots and make then.
  void MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                           TString XLabel, TString YLabel, Bool_t logY = false );

  /// Binning with fixed width
  void MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                           TString XLabel, TString YLabel, Bool_t logY = false );

  /**
     Sets the legend entry that will be on the top of the stack.
     @param force should match a legend entry. It should have spaces instead of underscores
                  if matching an entry in an [MC Configuration](formatmc).
  */
  void SetForceTop       ( TString force )            { fForceTop = force;                        }
  /// Set the minimum fraction of background that shows up on the plot's legend separately
  void SetMinLegendFrac  ( Double_t frac )            { fMinLegendFrac = frac;                    }
  /// Set the minimum fraction of backgrounds that are not plotted or mentioned in linear scale
  void SetIgnoreInLinear ( Double_t ignore )          { fIgnoreInLinear = ignore;                 }
  /// Set the color of the "Others" legend entry
  void SetOthersColor    ( Color_t color )            { fOthersColor = color;                     }
  /// Set the line width of the stack plots
  void SetStackLineWidth ( Int_t width )              { fStackLineWidth = width;                  }

  /// This dumps out some raw values for you to check yields.
  void SetDebug          ( Bool_t debug )             { fDebug = debug;                           }
  /// Set the name of a .root file to dump the separate histograms used in the plots.
  void SetDumpFileName   ( TString dumpName )         { fDumpRootName = dumpName;                 }

  /**
     Insert a histogram from file into the stack plot.
     Most histograms in the stack plot are created from TTree::Draw() functions acting on trees.
     It is also possible to just insert a histogram generated by someone else into the stack plot.
     @param fileName is the name of the file containing the histogram.
     @param histName is the name of the TH1 inside the file.
     @param LegendEntry is the entry in the legend. It does not require underscores for spaces
                        like in the [MC Configurations](@ref formatmc).
  */
  void InsertTemplate    ( TString fileName, TString histName, TString LegendEntry )
                                                      { fTemplateFiles.push_back(fileName);
                                                        fTemplateHists.push_back(histName);
                                                        fTemplateEntries.push_back(LegendEntry);  }

  /// Use this to change the automatic sorting of backgrounds based on yields
  void SetSortBackground ( Bool_t doSort )            { fSortBackground = doSort;                 }

 private:
  TString               fForceTop = "";             ///< A legend entry that is on the top of backgrounds
  Double_t              fMinLegendFrac = 0.0;       ///< If a background contributes less than this, it's grouped in others
  Double_t              fIgnoreInLinear = 0.0;      ///< If background less than this, do not draw in linear-scale plot
  Int_t                 fStackLineWidth = 1;        ///< Line width of the histograms
  Color_t               fOthersColor = 0;           ///< Color of the "Others" legend entry

  std::vector<TString>  fTemplateFiles;             ///< Holds file name of the templates
  std::vector<TString>  fTemplateHists;             ///< Holds histogram name of the templates
  std::vector<TString>  fTemplateEntries;           ///< Holds legend entries of the templates

  Bool_t                fDebug = false;             ///< Dumps yield tests
  TString               fDumpRootName = "";         ///< File where each histogram in stack is dumped
  Bool_t                fSortBackground = true;     ///< Bool to sort the backgrounds

  ClassDef(PlotStack, 1)
};

#endif
