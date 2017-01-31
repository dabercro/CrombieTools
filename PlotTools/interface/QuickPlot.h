/**
  @file   QuickPlot.h
  Definition of QuickPlot class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/


#ifndef CROMBIETOOLS_PLOTTOOLS_QUICKPLOT_H
#define CROMBIETOOLS_PLOTTOOLS_QUICKPLOT_H

#include <vector>

#include "TString.h"

#include "FileConfigReader.h"

/**
   @ingroup plotgroup
   @class QuickPlot
   A less flexible class that quickly compares shapes from the MCConfig.
   This class is accessed through CrombieTools.PlotTools.QuickPlot.
*/

class QuickPlot : public FileConfigReader
{
 public:
  QuickPlot();
  virtual ~QuickPlot();

  /**
     Add a MC contributer to the plots that will be shown.
     @param entry is the legend entry in the MC Config.
     @param color is the color to draw the line
     @param type is the FileType where the FileInfo will be found.
  */
  void         AddBackground            ( TString entry, Color_t color, FileType type = kBackground );

  /// Make canvas using an array
  void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                                          TString XLabel, TString = "", Bool_t logY = false);

  /// Make canvas using regular binning
  void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                          TString XLabel, TString = "", Bool_t logY = false);

  /// Reset the stored backgrounds to make a different plot
  void         Reset                    ()                { ResetLegend(); fBackgroundType.resize(0); }

  /// If this is false, the plots are not normalized at the end, so the shape is not as easily compared
  void         SetCompareShapes         ( bool compare )            { bCompareShapes = compare;       }

 private:
  std::vector<FileType> fBackgroundType;       ///< FileType for each background to plot
  bool bCompareShapes = true;                  ///< Bool to tell whether to compare shapes by normalizing plots

  ClassDef(QuickPlot, 1)
};

#endif
