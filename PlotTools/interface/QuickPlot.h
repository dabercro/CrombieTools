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
   A less flexible class that quickly compares shapes from the MCConfig
*/

class QuickPlot : public FileConfigReader
{
 public:
  QuickPlot();
  virtual ~QuickPlot();

  /**
     Add a MC contributer to the plots that will be shown.
     @todo Make the color automatic
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

 private:
  std::vector<FileType> fBackgroundType;       ///< FileType for each background to plot

  ClassDef(QuickPlot, 1)
};

#endif
