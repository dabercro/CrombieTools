#include "QuickPlot.h"

ClassImp(QuickPlot)

//--------------------------------------------------------------------
QuickPlot::QuickPlot()
{ }

//--------------------------------------------------------------------
QuickPlot::~QuickPlot()
{ }

//--------------------------------------------------------------------
void
QuickPlot::AddBackground(TString entry, Color_t color, FileType type)
{
  AddLegendEntry(entry, color);
  fBackgroundType.push_back(type);
}

//--------------------------------------------------------------------
void
QuickPlot::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                      TString XLabel, TString, Bool_t logY)
{
  std::vector<TH1D*> hists;

  for (UInt_t iBackground = 0; iBackground != fBackgroundType.size(); ++iBackground) {
    TH1D *tempHist = GetHist(NumXBins, XBins, fBackgroundType[iBackground],
                             fLegendEntries[iBackground], kLegendEntry);

    if (bCompareShapes)
      tempHist->Scale(1.0/tempHist->Integral("width"));

    hists.push_back(tempHist);
  }

  BaseCanvas(FileBase, hists, XLabel, "A.U.", logY);

  for (UInt_t i0 = 0; i0 != hists.size(); ++i0)
    delete hists[i0];
}

//--------------------------------------------------------------------
void
QuickPlot::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                      TString XLabel, TString, Bool_t logY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  MakeCanvas(FileBase, NumXBins, XBins, XLabel, "", logY);
}
