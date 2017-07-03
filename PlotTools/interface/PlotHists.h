/**
  @file   PlotHists.h
  Definition of PlotHists class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTHISTS_H

#include <vector>

#include "TH1D.h"
#include "TCanvas.h"
#include "TProfile.h"

#include "PlotUtils.h"
#include "UncertaintyInfo.h"
#include "PlotBase.h"

/**
   @ingroup plotgroup
   @class PlotHists
   A flexible class that plots histograms.
   Many practical uses are mediated by other classes.
*/

class PlotHists : public PlotBase
{
 public:
  PlotHists();
  virtual ~PlotHists();

  /// Add uncertainty factors to some index of histograms about to be made
  void                   AddUncertainty           ( UInt_t index, TString FileName, TString HistName,
                                                    Int_t startBin = 1, Int_t endBin = 0 );
  /// This just return vectors of histograms for other uses
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t *XBins );
  std::vector<TH1D*>     MakeHists                ( Int_t NumXBins, Double_t MinX, Double_t MaxX );

  /// Alternatively, we can set values in PlotBase and then just give the binning
  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

  virtual   void         MakeCanvas               ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                                    TString XLabel, TString YLabel, Bool_t logY = false);

  /// Add uncertainty factors through branch expressions instead of a histogram
  void                   SetUncertaintySquared    ( TString expr )                       { fUncExpr = expr;       }
  /// We can set normalization to match a particular other hist (or just 1)
  void                   SetNormalizedHists       ( Bool_t b )                           { fNormalizedHists = b;  }
  /// Set index of which histogram to normalize to
  void                   SetNormalizeTo           ( Int_t to )                           { fNormalizeTo = to;     }
  /// We can set events per some value of x so that our legend entry is not wrong and variable binning doesn't look stupid
  void                   SetEventsPer             ( Double_t per )                       { fEventsPer = per;      }

  /// Simplest of Canvas makers with just histograms as arugments, allowing for user manipulation of histograms
  void                   MakeCanvas               ( TString FileBase, std::vector<TH1D*> theHists,
                                                    TString XLabel, TString YLabel, Bool_t logY = false)
                                                          { BaseCanvas(FileBase, theHists, XLabel, YLabel, logY); }

 protected:
  Double_t  fEventsPer = 0;                       ///< Histogram normalized to events per units of X axis

 private:
  Bool_t    fNormalizedHists = false;             ///< Can normalize histograms in order to compare shapes
  Int_t     fNormalizeTo = -1;                    ///< If not specified, normalized to 1
  std::vector<UInt_t>           fSysUncIndices;   ///< Indices of histograms to apply systematic uncertainties
  std::vector<UncertaintyInfo*> fUncerts;         ///< Uncertainties to apply to histograms
  std::vector<UncertaintyInfo*> fDeleteUnc;       ///< Uncertainties created by the class to delete at the end
  TString   fUncExpr = "";                        ///< Branch expressions to add to the systematic uncertainty

};

//--------------------------------------------------------------------
PlotHists::PlotHists()
{}

//--------------------------------------------------------------------
PlotHists::~PlotHists()
{
  for (UInt_t iDelete = 0; iDelete != fDeleteUnc.size(); ++iDelete)
    delete fDeleteUnc[iDelete];
}

//--------------------------------------------------------------------
void
PlotHists::AddUncertainty(UInt_t index, TString FileName, TString HistName,
                          Int_t startBin, Int_t endBin )
{
  fSysUncIndices.push_back(index);
  UncertaintyInfo* Uncert = new UncertaintyInfo("", FileName, HistName, startBin, endBin);
  fUncerts.push_back(Uncert);
  fDeleteUnc.push_back(Uncert);
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotHists::MakeHists(Int_t NumXBins, Double_t *XBins)
{
  DisplayFunc(__func__);

  UInt_t NumPlots = 0;
  std::vector<TH1D*> theHists;

  if (fNormalizeTo != -1)
    fNormalizedHists = true;

  if (fInTrees.size() != 0)
    NumPlots = fInTrees.size();
  else if (fInCuts.size() != 0)
    NumPlots = fInCuts.size();
  else
    NumPlots = fInExpr.size();

  if(NumPlots == 0){
    Message(eError, "Number of trees: %i, cuts: %i, expressions: %i",
            fInTrees.size(), fInCuts.size(), fInExpr.size());
    Message(eError, "Nothing has been initialized in hists plot.");
    exit(1);
  }

  TTree *inTree = fDefaultTree;
  TString inExpr = fDefaultExpr;
  TCut inCut = fDefaultCut;

  TH1D *tempHist;

  for (UInt_t iPlot = 0; iPlot != NumPlots; ++iPlot) {

    if (fInTrees.size() != 0)
      inTree = fInTrees[iPlot];
    if (fInCuts.size()  != 0)
      inCut  = fInCuts[iPlot];
    if (fInExpr.size() != 0)
      inExpr = fInExpr[iPlot];

    if (!inTree || inExpr == "" || inCut == "") {
      Message(eError, "There is a problem with one of these in plot %i", iPlot);
      Message(eError, "Tree: %p, Expression: %s, Cut: %s", inTree, inExpr.Data(), inCut.GetTitle());
      exit(12);
    }

    TString tempName;
    tempName.Form("Hist_%d", fPlotCounter);
    fPlotCounter++;
    tempHist = new TH1D(tempName, tempName, NumXBins, XBins);
    tempHist->Sumw2();

    Message(eDebug, "About to draw %s on %s, with cut %s",
            inExpr.Data(), tempName.Data(), inCut.GetTitle());

    inTree->Draw(inExpr + ">>" + tempName, inCut);

    if (fUncExpr != "" && tempHist->GetEntries() != 0) {

      // If there's an uncertainty expression, add systematics to the plot
      tempName += "_unc";
      TProfile *uncProfile = new TProfile(tempName, tempName, NumXBins, XBins);
      inTree->Draw(fUncExpr + ":" + inExpr + ">>" + tempName, inCut);
      for (Int_t iBin = 1; iBin != NumXBins + 1; ++iBin) {
        Double_t uncSquared = uncProfile->GetBinContent(iBin);
        if (uncSquared == 0.0)
          continue;
        Message(eDebug, "About to apply uncertainty %f", TMath::Sqrt(uncSquared));

        Double_t content = tempHist->GetBinContent(iBin);
        Message(eDebug, "For hist %s, bin %i before: %f +- %f",
                tempName.Data(), iBin, content, tempHist->GetBinError(iBin));
        tempHist->SetBinError(iBin,
                              TMath::Sqrt(pow(tempHist->GetBinError(iBin), 2) +
                                          content * content * uncSquared));
        Message(eDebug, "For hist %s, bin %i after:  %f +- %f",
                tempName.Data(), iBin, content, tempHist->GetBinError(iBin));
      }
      delete uncProfile;
    }

    Message(eDebug, "Number of events: %i, integral: %f",
            (Int_t) tempHist->GetEntries(), tempHist->Integral("width"));

    theHists.push_back(tempHist);
  }

  if (fEventsPer > 0) {
    Message(eDebug, "Events per: %f", fEventsPer);

    TString tempName;
    tempName.Form("Hist_%d", fPlotCounter);
    fPlotCounter++;
    tempHist = new TH1D(tempName, tempName, NumXBins, XBins);
    for (Int_t iBin = 1; iBin != NumXBins + 1; ++iBin)
      tempHist->SetBinContent(iBin, tempHist->GetBinWidth(iBin)/fEventsPer);

    SetZeroError(tempHist);
    for (UInt_t iHist = 0; iHist != theHists.size(); ++iHist) {
      Message(eDebug, "Now for %s, number of events: %i, integral: %f",
              theHists[iHist]->GetName(),
              (Int_t) tempHist->GetEntries(),
              tempHist->Integral("width"));

      Division(theHists[iHist], tempHist);
    }

    delete tempHist;
  }

  if (fNormalizedHists) {
    Double_t normInt = 1;
    if (fNormalizeTo != -1)
      normInt = theHists[fNormalizeTo]->Integral("width");

    for (UInt_t iHist = 0; iHist != NumPlots; ++iHist)
      theHists[iHist]->Scale(normInt/theHists[iHist]->Integral("width"));
  }

  for (UInt_t iUncert = 0; iUncert != fSysUncIndices.size(); ++iUncert)
    ApplyUncertainty(theHists[fSysUncIndices[iUncert]], fUncerts[iUncert]);

  return theHists;
}

//--------------------------------------------------------------------
std::vector<TH1D*>
PlotHists::MakeHists(Int_t NumXBins, Double_t MinX, Double_t MaxX)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  return MakeHists(NumXBins, XBins);
}

//--------------------------------------------------------------------
void
PlotHists::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  std::vector<TH1D*> hists = MakeHists(NumXBins, XBins);
  BaseCanvas(FileBase, hists, XLabel, YLabel, logY);

  for (UInt_t i0 = 0; i0 != hists.size(); ++i0)
    delete hists[i0];
}

//--------------------------------------------------------------------
void
PlotHists::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                      TString XLabel, TString YLabel, Bool_t logY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  MakeCanvas(FileBase, NumXBins, XBins, XLabel, YLabel, logY);
}

#endif
