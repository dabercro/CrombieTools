#include <vector>

#include "TMath.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include "PlotTriggerEfficiency.h"

ClassImp(PlotTriggerEfficiency)

//--------------------------------------------------------------------
void
PlotTriggerEfficiency::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t *XBins,
                                  TString XLabel, TString, Bool_t logY)
{
  TH2D* result_holder = new TH2D("Trigger2DHist", "Trigger2DHist", NumXBins, XBins, 2, 0, 2);
  std::vector<TGraphAsymmErrors*> mapped;

  TString drawing = TString::Format("%s:%s>>Trigger2DHist", fTrigger.Data(), fDefaultExpr.Data());
  Message(eDebug, "Drawing with: %s", drawing.Data());

  fChain->Draw(drawing, fDefaultCut);
  TH1D *allhist = result_holder->ProjectionX("all");
  for (int iBin = 1; iBin < NumXBins + 1; iBin++) {
    Message(eDebug, "Bin: %i, pass: %f, fail: %f", iBin,
            result_holder->GetBinContent(iBin, 2), result_holder->GetBinContent(iBin, 1));

    // If no events at all, just pretend a buttload failed.
    if (!allhist->GetBinContent(iBin))
      allhist->SetBinContent(iBin, 1000000);
  }

  TGraphAsymmErrors* plot = new TGraphAsymmErrors(NumXBins);
  Message(eDebug, "Number of graph points: %i", plot->GetN());
  plot->Divide(result_holder->ProjectionX("pass", 2, -1), allhist);
  Message(eDebug, "Number of graph points: %i", plot->GetN());

  plot->GetYaxis()->SetRangeUser(0.0, 1.2);
  mapped.push_back(plot);
  SetDrawOpts("ape");
  AddCutLine(1.0, false);

  BaseCanvas(FileBase, mapped, XLabel, "Efficiency", logY);

  mapped.clear();

  delete plot;
  delete allhist;
  delete result_holder;
}

//--------------------------------------------------------------------
void
PlotTriggerEfficiency::MakeCanvas(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                                  TString XLabel, TString, Bool_t logY)
{
  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  MakeCanvas(FileBase, NumXBins, XBins, XLabel, "", logY);
}
