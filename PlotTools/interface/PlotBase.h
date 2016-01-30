#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTBASE_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTBASE_H

#include <vector>
#include <iostream>
#include "TObject.h"
#include "TStyle.h"
#include "TTree.h"
#include "TString.h"

#include "TLatex.h"
#include "TH1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "PlotUtils.h"

class PlotBase
{
 public:
  PlotBase();
  virtual ~PlotBase();
  
  void                   SetCanvasName            ( TString name )                                { fCanvasName = name;          }
  
  void                   AddLine                  ( TTree *tree, TString cut, TString expr );  // Each line has a potentially different
                                                                                               //   tree, weight, and expression.
  // Can make a default for one of these instead
  void                   SetDefaultTree           ( TTree *tree )                                 { fDefaultTree = tree;         }
  void                   SetDefaultWeight         ( TString cut )                                 { fDefaultCut = cut;           }
  void                   SetDefaultExpr           ( TString expr )                                { fDefaultExpr = expr;         }

  // Can set all of the trees at once
  void                   SetTreeList              ( std::vector<TTree*> treelist )                { fInTrees = treelist;         }
  // Otherwise, add things to lines one at a time
  void                   AddTree                  ( TTree *tree )                                 { fInTrees.push_back(tree);    }
  void                   AddWeight                ( TString cut )                                 { fInCuts.push_back(cut);      }
  void                   AddExpr                  ( TString expr )                                { fInExpr.push_back(expr);     }
  // We can reset anything to make more plots with same object
  void                   ResetTree                ()                                              { fInTrees.resize(0);          }
  void                   ResetWeight              ()                                              { fInCuts.resize(0);           }
  void                   ResetExpr                ()                                              { fInExpr.resize(0);           }

  // Two parts at once. Only use if the other part of a line already has a default set.
  void                   AddTreeWeight            ( TTree *tree, TString cut );           // These are used to set multiple values at
  void                   AddTreeExpr              ( TTree *tree, TString expr );          //  the same time. It may be simpler to
  void                   AddWeightExpr            ( TString cut, TString expr );          //  just set single variables for most users.
  
  // Some formatting options are available
  void                   SetDefaultLineWidth      ( Int_t width )                                 { fDefaultLineWidth = width;   }
  void                   SetDefaultLineStyle      ( Int_t style )                                 { fDefaultLineStyle = style;   }
  void                   SetIncludeErrorBars      ( Bool_t include )                              { fIncludeErrorBars = include; }

  // We can also set location and format of legend
  enum LegendY { kUpper = 0, kLower };
  enum LegendX { kLeft  = 0, kRight };
  void                   SetLegendLocation        ( LegendY yLoc, LegendX xLoc, Double_t xWidth = 0.3, Double_t yWidth = 0.2 );
  void                   SetLegendLimits          ( Double_t lim1, Double_t lim2, Double_t lim3, Double_t lim4 )
                                                                                   { l1 = lim1; l2 = lim2; l3 = lim3; l4 = lim4; }

  void                   SetLegendBorderSize      ( Int_t size )                                  { fLegendBorderSize = size;    }
  void                   SetLegendFill            ( Bool_t fill )                                 { fLegendFill = fill;          }

  // We can name the lines in the legend as well as set alternative line widths or styles
  void                   AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry );    // Uses default line width and style
  void                   AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry, Int_t LineWidth, Int_t LineStyle );
  void                   ResetLegend              ()                          { fLegendEntries.resize(0); fLineColors.resize(0); 
                                                                                fLineWidths.resize(0);    fLineStyles.resize(0); }

  // Set one of the lines to be plotted like data
  void                   SetDataIndex             ( Int_t data )                                  { fDataIndex = data;           }
  // Set bool to make a smaller pad for ratio plots
  void                   SetMakeRatio             ( Bool_t ratio )                                { fMakeRatio = ratio;          }
  void                   SetRatioIndex            ( Int_t ratio )                                 { fRatioIndex = ratio;         }
  void                   SetOnlyRatioWithData     ( Bool_t only )                                 { fOnlyRatioWithData = only;   }
  void                   SetRatioMinMax           ( Float_t min, Float_t max )               { fRatioMin = min; fRatioMax = max; }
  // Pick a line to draw first on the plot, if desired
  void                   SetDrawFirst             ( Int_t first )                                 { fDrawFirst = first;          }

  // Options to not spit out .pdf .png and .C files
  void                   OnlyPDF                  ()                                              { bPNG = false; bC = false;    }
  void                   OnlyPNG                  ()                                              { bPDF = false; bC = false;    }

  void                   SetLumiLabel             ( TString lumi )                                { fLumiLabel = lumi;           }
  void                   SetLumiLabel             ( Float_t lumi )                  { fLumiLabel = TString::Format("%.1f",lumi); }
  void                   SetIsCMSPrelim           ( Bool_t isPre )                                { fIsCMSPrelim = isPre;        }
  
 protected:

  UInt_t                     fPlotCounter;        // This is used so that making scratch plots does not overlap
  
  TTree*                     fDefaultTree;        // Default Tree if needed
  TString                    fDefaultCut;         // Default cut if needed
  TString                    fDefaultExpr;        // Default resolution expression if needed
  
  Double_t                   l1;                  // First X value of legend location
  Double_t                   l2;                  // First Y value of legend location
  Double_t                   l3;                  // Second X value of legend location
  Double_t                   l4;                  // Second Y value of legend location
  Int_t                      fLegendBorderSize;   // Border size of legend
  
  std::vector<TTree*>        fInTrees;            // Holds all the trees for each line if needed
  std::vector<TString>       fInCuts;             // Holds the cuts for the trees if needed
  std::vector<TString>       fInExpr;             // Holds multiple resolution expressions if needed
  
  Bool_t                     fIncludeErrorBars;   // Option to include error bars

  Int_t                      fDataIndex;          // Index in the plotter of the data line
  Bool_t                     fMakeRatio;          // Bool to make a ratio plot on bottom of image
  Int_t                      fRatioIndex;         // Pick which line to set as 1 in ratio plot
  Float_t                    fRatioMin;
  Float_t                    fRatioMax;

  std::vector<TString>       fLegendEntries;      // Number of legend entries should match number of lines

  // Takes number of bins, min and max, and dumps it into an already allocated array
  void                       ConvertToArray       ( Int_t NumXBins, Double_t MinX, Double_t MaxX, Double_t *XBins );

  // This is the powerhouse of all the plotting tools. Everything happens here. See function for more comments
  template<class T>  void    BaseCanvas           ( TString FileBase, std::vector<T*> theLines,
						    TString XLabel, TString YLabel, Bool_t logY, Bool_t logX = false );

  // This is just to determine which file types to make
  Bool_t                     bPDF;
  Bool_t                     bPNG;
  Bool_t                     bC;

 private:

  TString                    fCanvasName;         // The name of the output canvas
  Int_t                      fDefaultLineWidth;   // Line width to make all plots
  Int_t                      fDefaultLineStyle;   // Line style to use on all plots
  
  std::vector<Color_t>       fLineColors;         // Number of colors should match number of lines
  std::vector<Int_t>         fLineWidths;         // Will be filled with defaults unless
  std::vector<Int_t>         fLineStyles;         //   set explicitly with overloaded function

  Bool_t                     fOnlyRatioWithData;  // Suppresses the ratio of extra MC
  Bool_t                     fLegendFill;         // Gives fill option to legend drawing
  Int_t                      fDrawFirst;          // Can force one of the lines to be drawn first

  TString                    fLumiLabel;
  Bool_t                     fIsCMSPrelim;

  // Use this to get certain draw options correct (for data, for example)
  template<class T>  void    LineDrawing          ( std::vector<T*> theLines, Int_t index, Bool_t same );
  std::vector<TObject*>      fDeleteThese;
};

//--------------------------------------------------------------------
PlotBase::PlotBase() :
  fPlotCounter(0),
  fDefaultTree(0),
  fDefaultCut(""),
  fDefaultExpr(""),
  l1(0.6),
  l2(0.7),
  l3(0.9),
  l4(0.9),
  fLegendBorderSize(0),
  fIncludeErrorBars(false),
  fDataIndex(-1),
  fMakeRatio(false),
  fRatioIndex(-1),
  fRatioMin(0.0),
  fRatioMax(2.0),
  bPDF(true),
  bPNG(true),
  bC(true),
  fCanvasName("canvas"),
  fDefaultLineWidth(2),
  fDefaultLineStyle(1),
  fOnlyRatioWithData(false),
  fLegendFill(false),
  fDrawFirst(-1),
  fLumiLabel(""),
  fIsCMSPrelim(false)
{
  fInTrees.resize(0);
  fInCuts.resize(0);
  fInExpr.resize(0);
  fLegendEntries.resize(0);
  fLineColors.resize(0);
  fLineWidths.resize(0);
  fLineStyles.resize(0);
}

//--------------------------------------------------------------------
PlotBase::~PlotBase()
{}

//--------------------------------------------------------------------
void
PlotBase::AddLine(TTree *tree, TString cut, TString expr)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree != NULL) {
    std::cout << "Default tree already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultCut != "") {
    std::cout << "Default cut already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultExpr != "") {
    std::cout << "Default resolution expression already set! Check configuration..." << std::endl;
    exit(1);
  }
  fInTrees.push_back(tree);
  fInCuts.push_back(cut);
  fInExpr.push_back(expr);
}

//--------------------------------------------------------------------
void
PlotBase::AddTreeWeight(TTree *tree, TString cut)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree != NULL) {
    std::cout << "Default tree already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultCut != "") {
    std::cout << "Default cut already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultExpr == "") {
    std::cout << "Please set default resolution expression first!" << std::endl;
    exit(1);
  }
  fInTrees.push_back(tree);
  fInCuts.push_back(cut);
}

//--------------------------------------------------------------------
void
PlotBase::AddTreeExpr(TTree *tree, TString expr)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree != NULL) {
    std::cout << "Default tree already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultCut == "") {
    std::cout << "Please set default cut first!" << std::endl;
    exit(1);
  }
  if (fDefaultExpr != "") {
    std::cout << "Default resolution expression already set! Check configuration..." << std::endl;
    exit(1);
  }
  fInTrees.push_back(tree);
  fInExpr.push_back(expr);
}

//--------------------------------------------------------------------
void
PlotBase::AddWeightExpr(TString cut, TString expr)
{
  // Check for defaults. If none, set the values for each line.
  if (fDefaultTree == NULL) {
    std::cout << "Please set default tree first!" << std::endl;
    exit(1);
  }
  if (fDefaultCut != "") {
    std::cout << "Default cut already set! Check configuration..." << std::endl;
    exit(1);
  }
  if (fDefaultExpr != "") {
    std::cout << "Default resolution expression already set! Check configuration..." << std::endl;
    exit(1);
  }
  fInCuts.push_back(cut);
  fInExpr.push_back(expr);
}

//--------------------------------------------------------------------
void
PlotBase::SetLegendLocation(LegendY yLoc, LegendX xLoc, Double_t xWidth, Double_t yWidth)
  // This function is designed to be lazy about putting legends
  // There are nice enums, like kUpper, kLower and kLeft, kRight
  // The width and height are then fractions of the canvas
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
void
PlotBase::AddLegendEntry(TString LegendEntry, Color_t ColorEntry )
  // Uses default line width and style to label and draw line
{
  fLegendEntries.push_back(LegendEntry);
  fLineColors.push_back(ColorEntry);
  fLineWidths.push_back(fDefaultLineWidth);
  fLineStyles.push_back(fDefaultLineStyle);
}

//--------------------------------------------------------------------
void
PlotBase::AddLegendEntry(TString LegendEntry, Color_t ColorEntry, Int_t LineWidth, Int_t LineStyle)
  // Here alternative line styles can be specified
{
  fLegendEntries.push_back(LegendEntry);
  fLineColors.push_back(ColorEntry);
  fLineWidths.push_back(LineWidth);
  fLineStyles.push_back(LineStyle);
}

//--------------------------------------------------------------------
void
PlotBase::ConvertToArray(Int_t NumXBins, Double_t MinX, Double_t MaxX, Double_t *XBins)
  // Simple function to get arrays fast
{
  Double_t binWidth = (MaxX - MinX)/NumXBins;
  for (Int_t i0 = 0; i0 < NumXBins + 1; i0++)
    XBins[i0] = MinX + i0 * binWidth;
}

//--------------------------------------------------------------------
template<class T>
void
PlotBase::LineDrawing(std::vector<T*> theLines, Int_t index, Bool_t same)
  // This is used instead of Draw for lines so that the draw options are set in one place
{
  TString options = "hist";
  if (index == fDataIndex)
    options = "PE";
  if (same)
    options = options + ",same";
  theLines[index]->Draw(options);
  if (fRatioIndex != -1 && index != fDataIndex) {
    T* tempLine = (T*) theLines[fRatioIndex]->Clone();
    tempLine->SetFillColor(kGray);
    tempLine->SetFillStyle(3001);
    tempLine->Draw("e2,same");
    fDeleteThese.push_back(tempLine);
  }
}

//--------------------------------------------------------------------
template<class T>
void
PlotBase::BaseCanvas(TString FileBase, std::vector<T*> theLines,
                     TString XLabel, TString YLabel, Bool_t logY, Bool_t logX)
  // Main macro of the plotters
{
  gStyle->SetOptStat(0);

  // Font size and size of ratio plots are set here
  // Can make this configurable, but don't think I need to
  Float_t fontSize  = 0.04;
  Float_t ratioFrac = 0.7;

  // If user implicitly wants to make ratio by moving one of the defaults
  // set other possible values here
  if (fRatioIndex != -1)
    fMakeRatio = true;
  else if (fMakeRatio)
    fRatioIndex = fDataIndex;

  UInt_t NumPlots = theLines.size();
  // Initialize the canvas and legend
  TCanvas *theCanvas = new TCanvas(fCanvasName,fCanvasName);
  theCanvas->SetTitle(";"+XLabel+";"+YLabel);
  TLegend *theLegend = new TLegend(l1,l2,l3,l4);
  theLegend->SetBorderSize(fLegendBorderSize);

  // We will check for the largest line to plot first
  float maxValue = 0;
  UInt_t plotFirst = 0;

  // Loop through the lines
  for (UInt_t iLine = 0; iLine != NumPlots; ++iLine) {
    // Set title of lines and format
    theLines[iLine]->SetTitle(";"+XLabel+";"+YLabel);
    if (int(iLine) != fDataIndex) {
      theLines[iLine]->SetLineWidth(fLineWidths[iLine]);
      theLines[iLine]->SetLineStyle(fLineStyles[iLine]);
      theLines[iLine]->SetLineColor(fLineColors[iLine]);
    }
    else
      theLines[iLine]->SetMarkerStyle(8);

    // Add a formated legend entry
    if (fLegendFill && int(iLine) != fDataIndex)
      theLegend->AddEntry(theLines[iLine],fLegendEntries[iLine],"lf");
    else
      theLegend->AddEntry(theLines[iLine],fLegendEntries[iLine],"lp");
      
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
      theLines[iLine]->GetYaxis()->SetTitleSize(fontSize/ratioFrac);
      theLines[iLine]->GetYaxis()->SetLabelSize(fontSize/ratioFrac);
      theLines[iLine]->GetXaxis()->SetTitleSize(0);
      theLines[iLine]->GetXaxis()->SetLabelSize(0);
    }
    if (logX)
      pad1->SetLogx();
    // Assume that log Y is only in the case of the top plot of ratio
    if (logY)
      pad1->SetLogy();

    gPad->RedrawAxis();
  }

  // If the first draw was not specified by the user, use the maximum found before
  if (fDrawFirst == -1)
    LineDrawing(theLines,plotFirst,false);
  else
    LineDrawing(theLines,fDrawFirst,false);
  // Then loop through all the other lines to draw
  for (UInt_t iLine = 0; iLine != NumPlots; ++iLine)
    LineDrawing(theLines,iLine,true);
  // Draw the data again at the end to ensure it's on top
  if (fDataIndex != -1)
    LineDrawing(theLines,fDataIndex,true);

  theLegend->Draw();

  gPad->RedrawAxis();

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
    pad2->SetTopMargin(0.035);
    pad2->SetBottomMargin(0.4);
    pad2->Draw();
    pad2->cd();

    // This is to give ticks to the Y Axis, see ROOT documentation for more details
    Int_t divisions = 504;

    // We take the line equal to '1' and copy it
    T *ratioLine = (T*) theLines[fRatioIndex]->Clone("ValueHolder");
    SetZeroError(ratioLine);

    // Then we make a set of lines that are divided by the ratio line
    std::vector<T*> newLines = GetRatioToLine(theLines,ratioLine);

    // Now we do formatting for all of the lines
    for (UInt_t iLine = 0; iLine != NumPlots; ++iLine) {
      newLines[iLine]->GetXaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetYaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetXaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetYaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetXaxis()->SetTitleOffset(1.1);
      newLines[iLine]->GetYaxis()->SetTitleOffset((1 - ratioFrac)/ratioFrac);
      newLines[iLine]->GetYaxis()->SetNdivisions(divisions);
      newLines[iLine]->GetYaxis()->SetTitle("Ratio");
      newLines[iLine]->SetMinimum(fRatioMin);
      newLines[iLine]->SetMaximum(fRatioMax);
      newLines[iLine]->SetFillColor(0);
    }

    // If we only want the two lines (like top of stack and data) just draw those
    // I know I looped over all the lines, but the performance hit is negligible and it makes for less buggy code
    if (fOnlyRatioWithData) {
      // Make the ratio line much less distinguished
      newLines[fRatioIndex]->SetLineColor(1);
      LineDrawing(newLines,fRatioIndex,false);
      LineDrawing(newLines,fDataIndex,true);
    }
    // Otherwise draw everything
    else {
      newLines[plotFirst];
      for (UInt_t iLine = 0; iLine < NumPlots; iLine++)
        LineDrawing(newLines,iLine,true);
    }

    // There is no log Y on the ratio plot
    if (logX)
      pad2->SetLogx();

    gPad->RedrawAxis();
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

  if (fIsCMSPrelim) {
    TLatex* latex3 = new TLatex();
    latex3->SetNDC();
    latex3->SetTextSize(0.035);
    latex3->SetTextFont(62);
    latex3->SetTextAlign(11);
    latex3->DrawLatex(0.12, 0.96, "CMS");
    latex3->SetTextSize(0.030);
    latex3->SetTextFont(52);
    latex3->SetTextAlign(11);
    latex3->DrawLatex(0.20, 0.96, "Preliminary");
    fDeleteThese.push_back(latex3);
  }

  gPad->RedrawAxis();

  // Now save the picture we just finished making
  if (bC)
    theCanvas->SaveAs(FileBase+".C");
  if (bPNG)
    theCanvas->SaveAs(FileBase+".png");
  if (bPDF)
    theCanvas->SaveAs(FileBase+".pdf");

  // Cleanup is minimal
  delete theLegend;
  delete theCanvas;
  for (UInt_t iDelete = 0; iDelete != fDeleteThese.size(); ++iDelete)
    delete fDeleteThese[iDelete];
  fDeleteThese.resize(0);
}

#endif
