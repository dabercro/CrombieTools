#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTBASE_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTBASE_H

#include <vector>
#include <iostream>
#include "TObject.h"
#include "TStyle.h"
#include "TTree.h"
#include "TString.h"

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
  void                   SetDefaultTree           ( TTree *tree )                                 { fDefaultTree = tree;         }
  void                   SetDefaultWeight         ( TString cut )                                 { fDefaultCut = cut;           }
  void                   SetDefaultExpr           ( TString expr )                                { fDefaultExpr = expr;         }
  
  void                   SetTreeList              ( std::vector<TTree*> treelist )                { fInTrees = treelist;         }
  void                   AddTree                  ( TTree *tree )                                 { fInTrees.push_back(tree);    }
  void                   AddWeight                ( TString cut )                                 { fInCuts.push_back(cut);      }
  void                   AddExpr                  ( TString expr )                                { fInExpr.push_back(expr);     }
  void                   ResetTree                ()                                              { fInTrees.resize(0);          }
  void                   ResetWeight              ()                                              { fInCuts.resize(0);           }
  void                   ResetExpr                ()                                              { fInExpr.resize(0);           }

  // Two parts at once. Only use if the other part of a line already has a default set.
  void                   AddTreeWeight            ( TTree *tree, TString cut );           // These are used to set multiple values at
  void                   AddTreeExpr              ( TTree *tree, TString expr );          //  the same time. It may be simpler to
  void                   AddWeightExpr            ( TString cut, TString expr );          //  just set single variables for most users.
  
  void                   SetDefaultLineWidth      ( Int_t width )                                 { fDefaultLineWidth = width;   }
  void                   SetDefaultLineStyle      ( Int_t style )                                 { fDefaultLineStyle = style;   }
  void                   SetIncludeErrorBars      ( Bool_t include )                              { fIncludeErrorBars = include; }

  enum LegendY { kUpper = 0, kLower };
  enum LegendX { kLeft  = 0, kRight };

  void                   SetLegendLocation        ( LegendY yLoc, LegendX xLoc, Double_t xWidth = 0.3, Double_t yWidth = 0.2 );
  void                   SetLegendLimits          ( Double_t lim1, Double_t lim2, Double_t lim3, Double_t lim4 );
  void                   AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry );    // Uses default line width and style
  void                   AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry, Int_t LineWidth, Int_t LineStyle );
  void                   SetLegendBorderSize      ( Int_t size )                                  { fLegendBorderSize = size;    }

  void                   SetDataIndex             ( Int_t data )                                  { fDataIndex = data;           }
  void                   SetMakeRatio             ( Bool_t ratio )                                { fMakeRatio = ratio;          }
  void                   SetRatioIndex            ( Int_t ratio )                                 { fRatioIndex = ratio;         }
  void                   SetOnlyRatioWithData     ( Bool_t only )                                 { fOnlyRatioWithData = only;   }
  void                   SetLegendFill            ( Bool_t fill )                                 { fLegendFill = fill;          }
  void                   SetDrawFirst             ( Int_t first )                                 { fDrawFirst = first;          }

  void                   OnlyPDF                  ()                                              { bPNG = false; bC = false;    }
  void                   OnlyPNG                  ()                                              { bPDF = false; bC = false;    }
  
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

  Int_t                      fDataIndex;
  Bool_t                     fMakeRatio;
  Int_t                      fRatioIndex;

  std::vector<TString>       fLegendEntries;      // Number of legend entries should match number of lines

  void                       ConvertToArray       ( Int_t NumXBins, Double_t MinX, Double_t MaxX, Double_t *XBins );

  template<class T>  void    BaseCanvas           ( TString FileBase, std::vector<T*> theLines,
						    TString XLabel, TString YLabel, Bool_t logY );

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

  Bool_t                     fOnlyRatioWithData;
  Bool_t                     fLegendFill;
  Int_t                      fDrawFirst;

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
  bPDF(true),
  bPNG(true),
  bC(true),
  fCanvasName("canvas"),
  fDefaultLineWidth(2),
  fDefaultLineStyle(1),
  fOnlyRatioWithData(false),
  fLegendFill(false),
  fDrawFirst(-1)
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
PlotBase::SetLegendLimits(Double_t lim1, Double_t lim2, Double_t lim3, Double_t lim4)
{
  l1 = lim1;
  l2 = lim2;
  l3 = lim3;
  l4 = lim4;
}

//--------------------------------------------------------------------
void
PlotBase::AddLegendEntry(TString LegendEntry, Color_t ColorEntry )
{
  // Uses default line width and style.
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
template<class T>
void
PlotBase::BaseCanvas(TString FileBase, std::vector<T*> theLines, TString XLabel, TString YLabel, Bool_t logY)
{
  gStyle->SetOptStat(0);

  Float_t fontSize  = 0.04;
  Float_t ratioFrac = 0.7;

  if (fRatioIndex != -1)
    fMakeRatio = true;
  else if (fMakeRatio)
    fRatioIndex = fDataIndex;

  UInt_t NumPlots = theLines.size();
  TCanvas *theCanvas = new TCanvas(fCanvasName,fCanvasName);
  theCanvas->SetTitle(";"+XLabel+";"+YLabel);
  TLegend *theLegend = new TLegend(l1,l2,l3,l4);
  theLegend->SetBorderSize(fLegendBorderSize);
  float maxValue = 0;
  UInt_t plotFirst = 0;

  for (UInt_t iLine = 0; iLine != NumPlots; ++iLine) {
    theLines[iLine]->SetTitle(";"+XLabel+";"+YLabel);
    if (int(iLine) != fDataIndex) {
      theLines[iLine]->SetLineWidth(fLineWidths[iLine]);
      theLines[iLine]->SetLineStyle(fLineStyles[iLine]);
      theLines[iLine]->SetLineColor(fLineColors[iLine]);
    }
    else
      theLines[iLine]->SetMarkerStyle(8);

    if (fLegendFill && int(iLine) != fDataIndex)
      theLegend->AddEntry(theLines[iLine],fLegendEntries[iLine],"lf");
    else
      theLegend->AddEntry(theLines[iLine],fLegendEntries[iLine],"lp");
      
    if (fDrawFirst == -1) {
      Double_t checkMax = theLines[iLine]->GetMaximum();
      
      if (checkMax > maxValue) {
        maxValue = checkMax;
        plotFirst = iLine;
      }
    }
  }

  if (fMakeRatio) {
    TPad *pad1 = new TPad("pad1", "pad1", 0, 1.0 - ratioFrac, 1, 1.0);
    pad1->SetBottomMargin(0.025);
    pad1->Draw();
    pad1->cd();
    for (UInt_t iLine = 0; iLine != NumPlots; ++iLine) {
      theLines[iLine]->GetYaxis()->SetTitleSize(fontSize/ratioFrac);
      theLines[iLine]->GetYaxis()->SetLabelSize(fontSize/ratioFrac);
      theLines[iLine]->GetXaxis()->SetTitleSize(0);
      theLines[iLine]->GetXaxis()->SetLabelSize(0);
    }
    if (logY)
      pad1->SetLogy();
  }

  if (fDrawFirst == -1)
    theLines[plotFirst]->Draw();
  else
    theLines[fDrawFirst]->Draw();
  for (UInt_t iLine = 0; iLine != NumPlots; ++iLine)
    theLines[iLine]->Draw("same");
  if (fDataIndex != -1)
    theLines[fDataIndex]->Draw("PE,same");

  theLegend->Draw();
  if (logY && !fMakeRatio)
    theCanvas->SetLogy();

  if (fMakeRatio) {
    theCanvas->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 1 - ratioFrac);
    pad2->SetTopMargin(0.035);
    pad2->SetBottomMargin(0.4);
    pad2->Draw();
    pad2->cd();

    Int_t divisions = 506;

    T *ratioLine = (T*) theLines[fRatioIndex]->Clone("ValueHolder");
    SetZeroError(ratioLine);

    std::vector<T*> newLines = GetRatioToLine(theLines,ratioLine);

    for (UInt_t iLine = 0; iLine != theLines.size(); ++iLine) {
      newLines[iLine]->GetXaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetYaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetXaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetYaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
      newLines[iLine]->GetXaxis()->SetTitleOffset(1.1);
      newLines[iLine]->GetYaxis()->SetTitleOffset((1 - ratioFrac)/ratioFrac);
      newLines[iLine]->GetYaxis()->SetNdivisions(divisions);
      newLines[iLine]->GetYaxis()->SetTitle("Ratio");
      newLines[iLine]->SetFillColor(0);
    }

    if (fOnlyRatioWithData) {
      newLines[fRatioIndex]->SetLineColor(1);
      newLines[fRatioIndex]->Draw();
      newLines[fDataIndex]->Draw("same");
    }
    else {
      newLines[plotFirst];
      for (UInt_t iLine = 0; iLine < theLines.size(); iLine++)
	newLines[iLine]->Draw("same");
    }
  }

  if (bC)
    theCanvas->SaveAs(FileBase+".C");
  if (bPNG)
    theCanvas->SaveAs(FileBase+".png");
  if (bPDF)
    theCanvas->SaveAs(FileBase+".pdf");

  delete theLegend;
  delete theCanvas;

}

#endif
