#include "RatioComparison.h"

ClassImp(RatioComparison)

//--------------------------------------------------------------------
RatioComparison::RatioComparison()
{ }

//--------------------------------------------------------------------
RatioComparison::~RatioComparison()
{ }

//--------------------------------------------------------------------
void
RatioComparison::MakeCompare(TString FileBase, Int_t NumXBins, Double_t *XBins,
                             TString XLabel, TString YLabel)
{
  DisplayFunc(__func__);

  std::vector<TH1D*> hists;

  for (UInt_t iLine = 0; iLine != fTypes.size(); ++iLine) {
    SetDefaultWeight(fNumeratorCuts[iLine].Data());
    TH1D *tempHist = GetHist(NumXBins, XBins, fTypes[iLine]);
    SetDefaultWeight(fDenominatorCuts[iLine].Data());
    tempHist->Divide(GetHist(NumXBins, XBins, fTypes[iLine]));

    /// @todo Add an option for background subraction for the ratios

    hists.push_back(tempHist);
  }

  BaseCanvas(FileBase, hists, XLabel, YLabel);

  for (UInt_t i0 = 0; i0 != hists.size(); ++i0)
    delete hists[i0];
}

//--------------------------------------------------------------------
void
RatioComparison::MakeCompare(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                             TString XLabel, TString YLabel)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  MakeCompare(FileBase, NumXBins, XBins, XLabel, YLabel);
}

//--------------------------------------------------------------------
void
RatioComparison::AddRatioCuts(TString num_cut, TString denom_cut, FileType type, TString entry, Int_t color)
{
  fNumeratorCuts.push_back(num_cut);
  fDenominatorCuts.push_back(denom_cut);
  fTypes.push_back(type);
  AddLegendEntry(entry, color);
}

//--------------------------------------------------------------------
void
RatioComparison::Reset()
{
  ResetLegend();
  fNumeratorCuts.resize(0);
  fDenominatorCuts.resize(0);
  fTypes.resize(0);
}
