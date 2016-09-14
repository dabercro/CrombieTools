/**
  @file   PlotBase.h

  Definition of PlotBase class. Since PlotBase is never initialized directly as a class,
  the entire class definition is contained in this header file.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTBASE_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTBASE_H

#include <vector>
#include <iostream>
#include "TObject.h"
#include "TStyle.h"
#include "TTree.h"
#include "TString.h"
#include "TCut.h"

#include "TLatex.h"
#include "TH1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"
#include "TLine.h"

#include "PlotUtils.h"

/**
   @ingroup plotgroup
   @class PlotBase
   Base class for all plotting classes.
   PlotBase holds much of the parameters used for plotting various kinds of plots.
   It also holds all of the functions that are used to create and save the canvas.
*/

class PlotBase
{
 public:
  PlotBase();
  virtual ~PlotBase();
  
  ///  This function adds a tree pointer, cut, and expression used for generating a line in the plot
  inline    void         AddLine                  ( TTree *tree, TCut cut, TString expr );

  /// Set a tree and weight concurrently for each line. Best used when a default expression is set.
  inline    void         AddTreeWeight            ( TTree *tree, TCut cut );
  /// Set a tree and expression concurrently for each line. Best used when a default weight is set.
  inline    void         AddTreeExpr              ( TTree *tree, TString expr );
  /// Set a weight and expression concurrently for each line. Best used when a default tree is set.
  inline    void         AddWeightExpr            ( TCut cut, TString expr );

  /// Set entry and color for each line. This uses the default line width and style.
  inline    void         AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry );
  /// Set entry, color, width, and style for each line.
  inline    void         AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry, Int_t LineWidth, Int_t LineStyle );

  ///  This function adds a tree pointer, cut, and expression used for generating a line in the plot
  inline    void         AddLine              ( TTree *tree, const char* cut, TString expr )   { AddLine(tree, TCut(cut), expr); }

  /// Set a tree and weight concurrently for each line. Best used when a default expression is set.
  inline    void         AddTreeWeight        ( TTree *tree, const char* cut )                 { AddTreeWeight(tree, TCut(cut)); }
  /// Set a weight and expression concurrently for each line. Best used when a default tree is set.
  inline    void         AddWeightExpr        ( const char* cut, TString expr )                { AddWeightExpr(TCut(cut), expr); }

  /// Sets the name of the canvas created by PlotBase.
  inline    void         SetCanvasName            ( TString name )                                { fCanvasName = name;          }
  /// Sets the size of the output canvas.
  inline    void         SetCanvasSize            ( Int_t width, Int_t height )  { fCanvasWidth = width; fCanvasHeight = height; }
  /// Sets the offset of the Y-axis title as a ratio of the default offset
  inline    void         SetAxisTitleOffset       ( Float_t offset )                              { fTitleOffset = offset;       }
  /// Sets the fontsize of the axis labels
  inline    void         SetFontSize              ( Float_t fontSize )                            { fFontSize = fontSize;        }
  /// Forces the minimum and maximum values of the Y-axis in the plot.
  inline    void         SetAxisMinMax            ( Float_t min, Float_t max )                 { fAxisMin = min; fAxisMax = max; }
  
  /// Set the default tree pointer for each line in the plot
  inline    void         SetDefaultTree           ( TTree *tree )                                 { fDefaultTree = tree;         }
  /// Set the default weight for each line in the plot.
  inline    void         SetDefaultWeight         ( TCut cut )                                    { fDefaultCut = cut;           }
  /// Set the default weight for each line in the plot.
  inline    void         SetDefaultWeight         ( const char* cut )                             { SetDefaultWeight(TCut(cut)); }
  /// Set the default expression to be plotted on the x-axis for each line in the plot
  inline    void         SetDefaultExpr           ( TString expr )                                { fDefaultExpr = expr;         }

  /// Get the default weight.
  inline    TCut         GetDefaultWeight         ()       const                                  { return fDefaultCut;          }

  /// Can store multiple trees at once for plots. Each tree plots its own line.
  inline    void         SetTreeList              ( std::vector<TTree*> treelist )                { fInTrees = treelist;         }
  /// Set a tree for a single line.
  inline    void         AddTree                  ( TTree *tree )                                 { fInTrees.push_back(tree);    }
  /// Set a weight for a single line.
  inline    void         AddWeight                ( TCut cut )                                    { fInCuts.push_back(cut);      }
  /// Set a weight for a single line.
  inline    void         AddWeight                ( const char* cut )                             { AddWeight(TCut(cut));        }
  /// Set an x expression for a single line.
  inline    void         AddExpr                  ( TString expr )                                { fInExpr.push_back(expr);     }

  /// Reset the list of trees used to makes lines
  inline    void         ResetTree                ()                                              { fInTrees.clear();            }
  /// Reset the list of weights used to makes lines
  inline    void         ResetWeight              ()                                              { fInCuts.clear();             }
  /// Reset the list of expressions used to makes lines
  inline    void         ResetExpr                ()                                              { fInExpr.clear();             }
  
  /// Set the default line width.
  inline    void         SetDefaultLineWidth      ( Int_t width )                                 { fDefaultLineWidth = width;   }
  /// Set the default line style.
  inline    void         SetDefaultLineStyle      ( Int_t style )                                 { fDefaultLineStyle = style;   }
  /// Set this to true to feature error bars in the plots.
  inline    void         SetIncludeErrorBars      ( Bool_t include )                              { fIncludeErrorBars = include; }

  /// Used for vertical position of legend.
  enum LegendY {
    kUpper = 0,      ///< Places the plot legend on the upper part of the canvas
    kLower,          ///< Places the plot legend on the lower part of the canvas
  };
  /// Used for horizontal position of legend.
  enum LegendX { 
    kLeft  = 0,      ///< Places the plot legend on the left part of the canvas
    kRight,          ///< Places the plot legend on the right part of the canvas
  };
  /// Set the legend location using LegendY and LegendX enums.
  inline    void         SetLegendLocation        ( LegendY yLoc, LegendX xLoc, Double_t xWidth = 0.3, Double_t yWidth = 0.2 );
  /// Set the legend location manually.
  inline    void         SetLegendLimits          ( Double_t lim1, Double_t lim2, Double_t lim3, Double_t lim4 )
                                                                                   { l1 = lim1; l2 = lim2; l3 = lim3; l4 = lim4; }

  /// Set the legend border size.
  inline    void         SetLegendBorderSize      ( Int_t size )                                  { fLegendBorderSize = size;    }
  /// If true, the legend will be filled with a solid background.
  inline    void         SetLegendFill            ( Bool_t fill )                                 { fLegendFill = fill;          }

  /// Resets the legend entries for each line.
  inline    void         ResetLegend              ()                              { fLegendEntries.clear(); fLineColors.clear(); 
                                                                                    fLineWidths.clear();    fLineStyles.clear(); }

  /// Set one of the lines to be plotted as data.
  inline    void         SetDataIndex             ( Int_t data )                                  { fDataIndex = data;           }
  /// If true, a ratio pad will be drawn underneath.
  inline    void         SetMakeRatio             ( Bool_t ratio )                                { fMakeRatio = ratio;          }
  /// Set which line will be '1' in the ratio plot.
  inline    void         SetRatioIndex            ( Int_t ratio )                                 { fRatioIndex = ratio;         }
  /**
     Add a line to show in the ratio plot.
     The line is specified by its index. If this list is empty, all of the lines appear in the ratio pad.
  */
  inline    void         AddRatioLine             ( Int_t line )                                  { fRatioLines.push_back(line); }
  /// Force the minimum and maximum values of the ratio pad.
  inline    void         SetRatioMinMax           ( Float_t min, Float_t max )               { fRatioMin = min; fRatioMax = max; }
  /// Set the y axis label of the ratio pad.
  inline    void         SetRatioTitle            ( TString title )                               { fRatioTitle = title;         }
  /// Set horizontal dotted lines on the ratio pad.
  inline    void         SetRatioGrid             ( Int_t grid )                                  { fRatioGrid = grid;           }
  /// Sets the divisions of the ratio y axis.
  inline    void         SetRatioDivisions        ( Int_t divisions, Bool_t optimize = true )     { fRatioDivisions = divisions; 
                                                                                                    fOptimDivisions = optimize;  }
  /// Force a line to draw first on the plot, if desired.
  inline    void         SetDrawFirst             ( Int_t first )                                 { fDrawFirst = first;          }

  /// Call this before plotting to only write plots to .pdf files.
  inline    void         OnlyPDF                  ()                                              { bPNG = false; bC = false;    }
  /// Call this before plotting to only write plots to .png files.
  inline    void         OnlyPNG                  ()                                              { bPDF = false; bC = false;    }

  /// Set the luminosity label.
  inline    void         SetLumiLabel             ( TString lumi )                                { fLumiLabel = lumi;           }
  /// Set the luminosity format.
  inline    void         SetLumiLabelFormat       ( TString format )                              { fLumiLabelFormat = format;   }
  /// Set the luminosity lable with a float in fb.
  inline    void         SetLumiLabel             ( Float_t lumi )        { fLumiLabel = TString::Format(fLumiLabelFormat,lumi); }
  /// Enums type of lables for plots
  enum      CMSLabelType {
    kNone = 0,       ///< Does not place a CMS label on the plot
    kPreliminary,    ///< Denotes a preliminary result
    kSimulation,     ///< Denotes a simulation result
  };
  /// Set the type of CMS label for the plot
  inline    void         SetCMSLabelType          ( CMSLabelType type )                           { fCMSLabelType = type;        }
  /// Adds a dotted line in order to show cuts.
  inline    void         AddCutLine               ( Double_t loc )                                { fCutLines.push_back(loc);    }
  /// Resets the number of cut lines to plot
  inline    void         ResetCutLines            ()                                              { fCutLines.resize(0);         }
  /// Sets the style for the cut lines.
  inline    void         SetCutLineStyle   ( Color_t color, Int_t width, Int_t style )   { fCutColor = color; fCutWidth = width; 
                                                                                                              fCutStyle = style; }
  /// Add branches that contain independent systematic uncertainties to show in the plots
  inline    void         AddSystematicBranch      ( TString branch )                 { fSystematicBranches.push_back(branch);    }

 protected:

  UInt_t                     fPlotCounter = 0;           ///< This is used so that making scratch plots does not overlap
  
  TTree*                     fDefaultTree = NULL;        ///< Default Tree if needed
  TCut                       fDefaultCut = "";           ///< Default cut if needed
  TString                    fDefaultExpr = "";          ///< Default resolution expression if needed
  
  Double_t                   l1 = 0.6;                   ///< First X value of legend location
  Double_t                   l2 = 0.7;                   ///< First Y value of legend location
  Double_t                   l3 = 0.9;                   ///< Second X value of legend location
  Double_t                   l4 = 0.9;                   ///< Second Y value of legend location
  Int_t                      fLegendBorderSize = 0;      ///< Border size of legend
  
  std::vector<TTree*>        fInTrees;                   ///< Holds all the trees for each line if needed
  std::vector<TCut>          fInCuts;                    ///< Holds the cuts for the trees if needed
  std::vector<TString>       fInExpr;                    ///< Holds multiple resolution expressions if needed
  std::vector<TString>       fSystematicBranches;        ///< Vector of branches to apply as systematic uncertainties
  
  Bool_t                     fIncludeErrorBars = true;   ///< Option to include error bars

  /**
     Minimum value of the y-axis.
     If fAxisMin is the same value as fAxisMax,
     then both values are ignored and the axis height
     is determined by the first line plotted.
  */
  Float_t                    fAxisMin = 0.0;
  Float_t                    fAxisMax = 0.0;             ///< Maximum value of the y-axis. 

  Int_t                      fDataIndex;                 ///< Index in the plotter of the data line
  Bool_t                     fMakeRatio;                 ///< Bool to make a ratio plot on bottom of image
  Int_t                      fRatioIndex;                ///< Pick which line to set as 1 in ratio plot
  Float_t                    fRatioMin;                  ///< Minimum of the ratio pad
  Float_t                    fRatioMax;                  ///< Maximum of the ratio pad
  TString                    fRatioTitle;                ///< Label of the ratio pad
  Int_t                      fRatioGrid;
  Int_t                      fRatioDivisions;
  Bool_t                     fOptimDivisions;
  std::vector<Int_t>         fRatioLines;                ///< Vector of line indices to show up in the ratio pad

  std::vector<TString>       fLegendEntries;             ///< Number of legend entries should match number of lines

  /// Takes number of bins, min and max, and dumps it into an already allocated array
  inline    void             ConvertToArray       ( Int_t NumXBins, Double_t MinX, Double_t MaxX, Double_t *XBins );

  /// This is the powerhouse of all the plotting tools. Everything happens here.
  template<class T>  void    BaseCanvas           ( TString FileBase, std::vector<T*> theLines, TString XLabel, 
                                                    TString YLabel, Bool_t logY = false, Bool_t logX = false );

  Bool_t                     bPDF = true;                ///< If true, BaseCanvas will create a .pdf file
  Bool_t                     bPNG = true;                ///< If true, BaseCanvas will create a .png file
  Bool_t                     bC = true;                  ///< If true, BaseCanvas will create a .C macro

 private:

  TString                    fCanvasName = "canvas";     ///< The name of the output canvas
  Int_t                      fCanvasWidth = 600;         ///< The width of the output canvas
  Int_t                      fCanvasHeight = 600;        ///< The height of the output canvas
  Float_t                    fTitleOffset = 1.0;         ///< The offset of the Y-axis title to account for large numbers
  Float_t                    fFontSize = 0.04;           ///< The size of the font used in the axis titles
  Int_t                      fDefaultLineWidth = 2;      ///< Line width to make all plots
  Int_t                      fDefaultLineStyle = 1;      ///< Line style to use on all plots
  
  std::vector<Color_t>       fLineColors;                ///< Colors of each of the lines
  std::vector<Int_t>         fLineWidths;                ///< Widths of each of the lines
  std::vector<Int_t>         fLineStyles;                ///< Styles of each of the lines

  Bool_t                     fLegendFill = false;        ///< Gives fill option to legend drawing
  Int_t                      fDrawFirst;                 ///< Can force one of the lines to be drawn first

  TString                    fLumiLabelFormat = "%.1f";  ///< Format used for changing lumi numbers into string
  TString                    fLumiLabel = "";            ///< Label used to show luminosity
  CMSLabelType               fCMSLabelType = kNone;      ///< Enum to determine what type of label to give

  /// Use this to get certain draw options correct (for data, for example)
  template<class T>  void    LineDrawing          ( std::vector<T*> theLines, Int_t index, Bool_t same );
  std::vector<TObject*>      fDeleteThese;               ///< Vector of object pointers to free memory at the end

  /// Options for histograms
  TString                    GetOpts              ( TH1* )         { return "hist";      }
  /// Options for TGraphs
  TString                    GetOpts              ( TGraph* )      { return "";          }

  std::vector<Double_t>      fCutLines;                  ///< Locations for dashed lines for cuts
  Color_t                    fCutColor = kRed;           ///< Color of the cuts lines
  Int_t                      fCutWidth = 2;              ///< Width of the cuts lines
  Int_t                      fCutStyle = 2;              ///< Style of the cuts lines
  Double_t                   fCutYMin = 0.0;             ///< Minimum value of cut line y
  Double_t                   fCutYMax = 0.0;             ///< Maximum value of cut line y

  /// Draws the cuts lines
  void                       DrawCutLines         ();

};

//--------------------------------------------------------------------
PlotBase::PlotBase() :
  fAxisMin(0.0),
  fAxisMax(0.0),
  fDataIndex(-1),
  fMakeRatio(false),
  fRatioIndex(-1),
  fRatioMin(0.0),
  fRatioMax(0.0),
  fRatioTitle("Ratio"),
  fRatioGrid(0),
  fRatioDivisions(504),
  fOptimDivisions (true),
  fDrawFirst(-1)
{ }

//--------------------------------------------------------------------
PlotBase::~PlotBase()
{ }

//--------------------------------------------------------------------
void PlotBase::AddLine(TTree *tree, TCut cut, TString expr)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree != NULL) {
    std::cerr << "Default tree already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultCut != "") {
    std::cerr << "Default cut already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultExpr != "") {
    std::cerr << "Default resolution expression already set! Check configuration..." << std::endl;
    exit(1);
  }
  fInTrees.push_back(tree);
  fInCuts.push_back(cut);
  fInExpr.push_back(expr);
}

//--------------------------------------------------------------------
void PlotBase::AddTreeWeight(TTree *tree, TCut cut)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree != NULL) {
    std::cerr << "Default tree already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultCut != "") {
    std::cerr << "Default cut already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultExpr == "") {
    std::cerr << "Please set default resolution expression first!" << std::endl;
    exit(1);
  }
  fInTrees.push_back(tree);
  fInCuts.push_back(cut);
}

//--------------------------------------------------------------------
void PlotBase::AddTreeExpr(TTree *tree, TString expr)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree != NULL) {
    std::cerr << "Default tree already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultCut == "") {
    std::cerr << "Please set default cut first!" << std::endl;
    exit(1);
  }
  if (fDefaultExpr != "") {
    std::cerr << "Default resolution expression already set! Check configuration..." << std::endl;
    exit(1);
  }
  fInTrees.push_back(tree);
  fInExpr.push_back(expr);
}

//--------------------------------------------------------------------
void PlotBase::AddWeightExpr(TCut cut, TString expr)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree == NULL) {
    std::cerr << "Please set default tree first!" << std::endl;
    exit(1);
  }
  if (fDefaultCut != "") {
    std::cerr << "Default cut already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultExpr != "") {
    std::cerr << "Default resolution expression already set! Check configuration..." << std::endl;
    exit(1);
  }
  fInCuts.push_back(cut);
  fInExpr.push_back(expr);
}

//--------------------------------------------------------------------

/**
   This function is designed to be lazy about putting legends
   There are nice enums, like kUpper, kLower and kLeft, kRight
   The width and height are then fractions of the canvas
*/

void PlotBase::SetLegendLocation(LegendY yLoc, LegendX xLoc, Double_t xWidth, Double_t yWidth)
{
  if (xLoc == kLeft) {
    l1 = 0.15;
    l3 = 0.15 + xWidth;
  }
  else {
    l3 = 0.9;
    l1 = 0.9 - xWidth;
  }

  if (yLoc == kUpper) {
    l4 = 0.9;
    l2 = 0.9 - yWidth;
  }
  else {
    l2 = 0.15;
    l4 = 0.15 + yWidth;
  }
}

//--------------------------------------------------------------------
void PlotBase::AddLegendEntry(TString LegendEntry, Color_t ColorEntry )
{
  fLegendEntries.push_back(LegendEntry);
  fLineColors.push_back(ColorEntry);
  fLineWidths.push_back(fDefaultLineWidth);
  fLineStyles.push_back(fDefaultLineStyle);
}

//--------------------------------------------------------------------
void
PlotBase::AddLegendEntry(TString LegendEntry, Color_t ColorEntry, Int_t LineWidth, Int_t LineStyle)
{
  fLegendEntries.push_back(LegendEntry);
  fLineColors.push_back(ColorEntry);
  fLineWidths.push_back(LineWidth);
  fLineStyles.push_back(LineStyle);
}

//--------------------------------------------------------------------
void
PlotBase::ConvertToArray(Int_t NumXBins, Double_t MinX, Double_t MaxX, Double_t *XBins)
{
  Double_t binWidth = (MaxX - MinX)/NumXBins;
  for (Int_t i0 = 0; i0 < NumXBins + 1; i0++)
    XBins[i0] = MinX + i0 * binWidth;
}

//--------------------------------------------------------------------
void
PlotBase::DrawCutLines() {
  // Draw the cuts lines
  for (UInt_t iCut = 0; iCut != fCutLines.size(); ++iCut) {
    TLine *aCutLine = new TLine(fCutLines[iCut], fCutYMin, fCutLines[iCut], fCutYMax);
    aCutLine->SetLineColor(fCutColor);
    aCutLine->SetLineWidth(fCutWidth);
    aCutLine->SetLineStyle(fCutStyle);
    fDeleteThese.push_back(aCutLine);
    aCutLine->Draw("same");
  }
}

//--------------------------------------------------------------------

/**
  This is used instead of Draw for lines so that the draw options are set in one place
*/

template<class T>
void PlotBase::LineDrawing(std::vector<T*> theLines, Int_t index, Bool_t same)
{
  if (!same) {
    fCutYMin = theLines[index]->GetMinimum();
    fCutYMax = theLines[index]->GetMaximum();
  }
  
  TString options = GetOpts(theLines[0]);
  if (index == fDataIndex)
    options = "PE";
  if (options != "")
    options += ",";
  options += "same";

  if (fRatioIndex != -1 && index != fDataIndex) {
    T* tempLine = (T*) theLines[fRatioIndex]->Clone();
    tempLine->SetFillColor(kGray);
    tempLine->SetFillStyle(3001);
    if (!same && index == fRatioIndex)
      tempLine->Draw("e2");
    theLines[index]->Draw(options);
    tempLine->Draw("e2,same");
    fDeleteThese.push_back(tempLine);
  }
  else
    theLines[index]->Draw(options);
}

//--------------------------------------------------------------------

/**
   Main macro of the plotters.
*/

template<class T>
void PlotBase::BaseCanvas(TString FileBase, std::vector<T*> theLines,
                          TString XLabel, TString YLabel, Bool_t logY, Bool_t logX)
{

  gStyle->SetOptStat(0);

  // Font size and size of ratio plots are set here
  // Can make this configurable, but don't think I need to
  Float_t ratioFrac = 0.7;

  UInt_t NumPlots = theLines.size();
  // Initialize the canvas and legend
  TCanvas *theCanvas = new TCanvas(fCanvasName, fCanvasName, fCanvasWidth, fCanvasHeight);
  theCanvas->SetTitle(";"+XLabel+";"+YLabel);
  TLegend *theLegend = new TLegend(l1,l2,l3,l4);
  theLegend->SetBorderSize(fLegendBorderSize);
  theLegend->SetFillStyle(0);

  // We will check for the largest line to plot first
  float maxValue = 0;
  UInt_t plotFirst = 0;

  // Check that legend entries were added correctly
  // No legend is needed if there is only a single line
  if (theLines.size() != 1 && theLines.size() != fLegendEntries.size()) {

    std::cerr << "[ERROR] Number of lines and number of legend entries do not match!" << std::endl;
    exit(1007);

  }

  // Loop through the lines
  for (UInt_t iLine = 0; iLine != NumPlots; ++iLine) {

    // Set title of lines and format
    theLines[iLine]->SetTitle(";"+XLabel+";"+YLabel);
    theLines[iLine]->GetYaxis()->SetTitleOffset(fTitleOffset);

    // If there's an axis requirement, set that here
    if (fAxisMin != fAxisMax) {

      theLines[iLine]->SetMinimum(fAxisMin);
      theLines[iLine]->SetMaximum(fAxisMax);        

    }

    if (int(iLine) != fDataIndex) {

      if (theLines.size() == 1 && fLineColors.size() == 0) {

        theLines[iLine]->SetLineWidth(fDefaultLineWidth);
        theLines[iLine]->SetLineStyle(fDefaultLineStyle);

      }
      else {

        theLines[iLine]->SetLineWidth(fLineWidths[iLine]);
        theLines[iLine]->SetLineStyle(fLineStyles[iLine]);
        theLines[iLine]->SetLineColor(fLineColors[iLine]);

      }

    }
    else
      theLines[iLine]->SetMarkerStyle(8);

    if (theLines.size() != 1) {

      // Add a formated legend entry
      if (fLegendFill && int(iLine) < fDataIndex)
        theLegend->AddEntry(theLines[iLine], fLegendEntries[iLine], "f");
      else
        theLegend->AddEntry(theLines[iLine], fLegendEntries[iLine], "lp");

    }
      
    // If the first draw is not set by user, check if maximum to draw first
    if (fDrawFirst == -1) {
      Double_t checkMax = theLines[iLine]->GetMaximum();
      
      if (checkMax > maxValue) {
        maxValue = checkMax;
        plotFirst = iLine;
      }
    }
  }

  // If there will be a ratio plot, the make an upper pad
  if (fMakeRatio) {

    TPad *pad1 = new TPad("pad1", "pad1", 0, 1.0 - ratioFrac, 1, 1.0);
    pad1->SetBottomMargin(0.025);
    pad1->Draw();
    pad1->cd();

    // Change the size of the font accordingly
    for (UInt_t iLine = 0; iLine != NumPlots; ++iLine) {

      theLines[iLine]->GetYaxis()->SetTitleSize(fFontSize/ratioFrac);
      theLines[iLine]->GetYaxis()->SetLabelSize(fFontSize/ratioFrac);
      theLines[iLine]->GetYaxis()->SetTitleOffset(fTitleOffset);
      theLines[iLine]->GetXaxis()->SetTitleSize(0);
      theLines[iLine]->GetXaxis()->SetLabelSize(0);

    }

    if (logX)
      pad1->SetLogx();

    // Assume that log Y is only in the case of the top plot of ratio
    if (logY)
      pad1->SetLogy();

  }

  // If the first draw was not specified by the user, use the maximum found before
  if (fDrawFirst == -1)
    LineDrawing(theLines, plotFirst, false);
  else
    LineDrawing(theLines, fDrawFirst, false);

  // Then loop through all the other lines to draw
  for (UInt_t iLine = 0; iLine != NumPlots; ++iLine)
    LineDrawing(theLines, iLine, true);

  // Draw the data again at the end to ensure it's on top
  if (fDataIndex != -1)
    LineDrawing(theLines, fDataIndex, true);

  if (theLines.size() != 1)
    theLegend->Draw();

  gPad->RedrawAxis();
  DrawCutLines();

  // If not a ratio plot, set the canvas to log
  if (!fMakeRatio) {

    if (logX)
      theCanvas->SetLogx();
    if (logY)
      theCanvas->SetLogy();

  }
  // Otherwise, go on the make the second pad and ratio plots
  else {

    theCanvas->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 1 - ratioFrac);
    pad2->SetTopMargin(0.05);
    pad2->SetBottomMargin(0.4);

    if (fRatioGrid > 0)
      pad2->SetGridy(fRatioGrid);

    pad2->Draw();
    pad2->cd();

    // We take the line equal to '1' and copy it
    T *ratioLine = (T*) theLines[fRatioIndex]->Clone("ValueHolder");
    SetZeroError(ratioLine);

    // Then we make a set of lines that are divided by the ratio line
    std::vector<T*> newLines = GetRatioToLine(theLines, ratioLine);

    // Now we do formatting for all of the lines
    for (UInt_t iLine = 0; iLine != NumPlots; ++iLine) {

      newLines[iLine]->GetXaxis()->SetTitleSize(fFontSize/(1 - ratioFrac));
      newLines[iLine]->GetYaxis()->SetTitleSize(fFontSize/(1 - ratioFrac));
      newLines[iLine]->GetXaxis()->SetLabelSize(fFontSize/(1 - ratioFrac));
      newLines[iLine]->GetYaxis()->SetLabelSize(fFontSize/(1 - ratioFrac));
      newLines[iLine]->GetYaxis()->SetTitleOffset((1 - ratioFrac)/ratioFrac * fTitleOffset);
      newLines[iLine]->GetYaxis()->SetNdivisions(fRatioDivisions, fOptimDivisions);
      newLines[iLine]->GetYaxis()->SetTitle(fRatioTitle);
      newLines[iLine]->GetYaxis()->CenterTitle();

      if (fRatioMin != fRatioMax) {

        newLines[iLine]->SetMinimum(fRatioMin);
        newLines[iLine]->SetMaximum(fRatioMax);

      }

      newLines[iLine]->SetFillColor(0);

    }

    // If we only want some lines (like top of stack and data) just draw those
    // I know I looped over all the lines, but the performance hit is negligible and it makes for less buggy code
    if (fRatioLines.size() != 0) {

      // Make the ratio line much less distinguished
      newLines[fRatioIndex]->SetLineColor(1);
      LineDrawing(newLines, fRatioIndex, false);

      for (UInt_t iLine = 0; iLine != fRatioLines.size(); ++iLine)
        LineDrawing(newLines, fRatioLines[iLine], true);

    }
    // Otherwise draw everything
    else {

      LineDrawing(newLines, plotFirst, false);

      for (UInt_t iLine = 0; iLine < NumPlots; iLine++)
        LineDrawing(newLines, iLine, true);

    }

    // There is no log Y on the ratio plot
    if (logX)
      pad2->SetLogx();

    gPad->RedrawAxis();
    DrawCutLines();

  }

  theCanvas->cd();

  if (fLumiLabel != "") {

    TLatex* latex2 = new TLatex();
    latex2->SetNDC();
    latex2->SetTextSize(0.035);
    latex2->SetTextAlign(31);
    latex2->DrawLatex(0.90, 0.96, fLumiLabel + " fb^{-1} (13 TeV)");
    fDeleteThese.push_back(latex2);

  }

  if (fCMSLabelType != kNone) {

    TLatex* latex3 = new TLatex();
    latex3->SetNDC();
    latex3->SetTextSize(0.035);
    latex3->SetTextFont(62);
    latex3->SetTextAlign(11);
    latex3->DrawLatex(0.12, 0.96, "CMS");
    latex3->SetTextSize(0.030);
    latex3->SetTextFont(52);
    latex3->SetTextAlign(11);

    if (fCMSLabelType == kPreliminary)
      latex3->DrawLatex(0.20, 0.96, "Preliminary");
    else if (fCMSLabelType == kSimulation)
      latex3->DrawLatex(0.20, 0.96, "Simulation");

    fDeleteThese.push_back(latex3);

  }

  // Now save the picture we just finished making
  if (bC)
    theCanvas->SaveAs(FileBase+".C");
  if (bPNG)
    theCanvas->SaveAs(FileBase+".png");
  if (bPDF)
    theCanvas->SaveAs(FileBase+".pdf");

  // Cleanup
  delete theLegend;
  delete theCanvas;

  for (UInt_t iDelete = 0; iDelete != fDeleteThese.size(); ++iDelete)
    delete fDeleteThese[iDelete];

  fDeleteThese.clear();

}

#endif
