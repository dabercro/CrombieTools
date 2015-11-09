#ifndef MITPLOTS_PLOT_PLOTBASE_H
#define MITPLOTS_PLOT_PLOTBASE_H

#include <vector>

#include <iostream>
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
  void                   ResetExpr                ()                                              { fInExpr.resize(0);           }
  void                   ResetWeight              ()                                              { fInCuts.resize(0);           }
  // Two parts at once. Only use if the other part of a line already has a default set.
  void                   AddTreeWeight            ( TTree *tree, TString cut );           // These are used to set multiple values at
  void                   AddTreeExpr              ( TTree *tree, TString expr );          //  the same time. It may be simpler to
  void                   AddWeightExpr            ( TString cut, TString expr );          //  just set single variables for most users.
  
  void                   SetDefaultLineWidth      ( Int_t width )                                 { fDefaultLineWidth = width;   }
  void                   SetDefaultLineStyle      ( Int_t style )                                 { fDefaultLineStyle = style;   }
  void                   SetIncludeErrorBars      ( Bool_t include )                              { fIncludeErrorBars = include; }
  void                   SetLegendLimits          ( Double_t lim1, Double_t lim2, Double_t lim3, Double_t lim4 );
  void                   AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry );    // Uses default line width and style
  void                   AddLegendEntry           ( TString LegendEntry, Color_t ColorEntry, Int_t LineWidth, Int_t LineStyle );
  void                   SetLegendBorderSize      ( Int_t size )                                  { fLegendBorderSize = size;    }

  void                   SetDataIndex             ( Int_t data )                                  { fDataIndex = data;           }
  void                   SetMakeRatio             ( Bool_t ratio )                                { fMakeRatio = ratio;          }
  void                   SetRatioIndex            ( Int_t ratio )                                 { fRatioIndex = ratio;         }
  
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

  void                       ConvertToArray       ( Int_t NumXBins, Double_t MinX, Double_t MaxX, Double_t *XBins );

  template<class T>  void    BaseCanvas           ( std::vector<T*> theLines, TString FileBase,
						    TString XLabel, TString YLabel, Bool_t logY );

 private:

  TString                    fCanvasName;         // The name of the output canvas
  Int_t                      fDefaultLineWidth;   // Line width to make all plots
  Int_t                      fDefaultLineStyle;   // Line style to use on all plots
  
  std::vector<TString>       fLegendEntries;      // Number of legend entries should match number of lines
  std::vector<Color_t>       fLineColors;         // Number of colors should match number of lines
  std::vector<Int_t>         fLineWidths;         // Will be filled with defaults unless
  std::vector<Int_t>         fLineStyles;         //   set explicitly with overloaded function

};

//--------------------------------------------------------------------
template<class T>
void
PlotBase::BaseCanvas(std::vector<T*> theLines, TString FileBase, TString XLabel, TString YLabel, Bool_t logY)
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
  float maxValue = 0.;
  UInt_t plotFirst = 0;
  for (UInt_t i0 = 0; i0 != NumPlots; ++i0) {
    theLines[i0]->SetTitle(";"+XLabel+";"+YLabel);
    if (int(i0) != fDataIndex) {
      theLines[i0]->SetLineWidth(fLineWidths[i0]);
      theLines[i0]->SetLineStyle(fLineStyles[i0]);
      theLines[i0]->SetLineColor(fLineColors[i0]);
    }

    theLegend->AddEntry(theLines[i0],fLegendEntries[i0],"lp");

    Double_t checkMax = -999;

    checkMax = theLines[i0]->GetMaximum();
      
    if (checkMax > maxValue) {
      maxValue = checkMax;
      plotFirst = i0;
    }
  }

  if (fMakeRatio) {
    TPad *pad1 = new TPad("pad1", "pad1", 0, 1.0 - ratioFrac, 1, 1.0);
    pad1->SetBottomMargin(0.025);
    pad1->Draw();
    pad1->cd();
    for (UInt_t i0 = 0; i0 < NumPlots; i0++) {
      theLines[i0]->GetYaxis()->SetTitleSize(fontSize/ratioFrac);
      theLines[i0]->GetYaxis()->SetLabelSize(fontSize/ratioFrac);
      theLines[i0]->GetXaxis()->SetTitleSize(0);
      theLines[i0]->GetXaxis()->SetLabelSize(0);
    }
  }

  theLines[plotFirst]->Draw();
  for (UInt_t i0 = 0; i0 != NumPlots; ++i0)
    theLines[i0]->Draw("same");

  theLegend->Draw();
  if (logY)
    theCanvas->SetLogy();

  if (fMakeRatio) {
    theCanvas->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 1 - ratioFrac);
    pad2->SetTopMargin(0.035);
    pad2->SetBottomMargin(0.4);
    pad2->Draw();
    pad2->cd();

    Int_t divisions = 506;

    T *ratioLine = SetZeroError((T*) theLines[fRatioIndex]->Clone("ValueHolder"));

    // Draw first the line that is through 1
    T *newLine  = (T*) theLines[fRatioIndex]->Clone();
    Division(newLine,ratioLine);
    newLine->SetTitle(";"+XLabel+";Ratio");
    newLine->GetXaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
    newLine->GetYaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
    newLine->GetXaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
    newLine->GetYaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
    newLine->GetXaxis()->SetTitleOffset(1.1);
    newLine->GetYaxis()->SetTitleOffset((1 - ratioFrac)/ratioFrac);
    newLine->GetYaxis()->SetNdivisions(divisions);
    newLine->Draw();

    // Then Draw everything else
    for (UInt_t iLines = 0; iLines < theLines.size(); iLines++) {
      if (int(iLines) == fRatioIndex)
        continue;
      newLine  = (T*) theLines[fRatioIndex]->Clone();
      Division(newLine,ratioLine);
      newLine->GetXaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
      newLine->GetYaxis()->SetTitleSize(fontSize/(1 - ratioFrac));
      newLine->GetXaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
      newLine->GetYaxis()->SetLabelSize(fontSize/(1 - ratioFrac));
      newLine->GetXaxis()->SetTitleOffset(1.1);
      newLine->GetYaxis()->SetTitleOffset((1 - ratioFrac)/ratioFrac);
      newLine->GetYaxis()->SetNdivisions(divisions);
      newLine->Draw("same");
    }
  }

  theCanvas->SaveAs(FileBase+".C");
  theCanvas->SaveAs(FileBase+".png");
  theCanvas->SaveAs(FileBase+".pdf");

  delete theLegend;
  delete theCanvas;

}

#endif
