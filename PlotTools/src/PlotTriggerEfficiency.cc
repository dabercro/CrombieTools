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
  TGraphAsymmErrors* plot = new TGraphAsymmErrors(NumXBins);

  TString drawing = TString::Format("%s:%s>>Trigger2DHist", fTrigger.Data(), fDefaultExpr.Data());
  Message(eDebug, "Drawing with: %s", drawing.Data());

  fChain->Draw(drawing, fDefaultCut);

  // Let's explicitly declare this because fuck confusing indices.
  int iBin = 0;
  
  double NumPassed = 0;
  double NumFailed = 0;
  double width = 0;
  double eY_var = 0;  // The bit that is +-
  double eY_stat = 0; // The bit that is not +-
  double eY_denom = 0;

  for (int iPoint = 0; iPoint < NumXBins; iPoint++) {
    iBin = iPoint + 1;
    NumFailed = result_holder->GetBinContent(iBin, 1);
    NumPassed = result_holder->GetBinContent(iBin, 2);

    Message(eDebug, "At bin %i, passed: %f, failed: %f", iBin, NumPassed, NumFailed);

    if (NumFailed + NumPassed == 0.0) {
      plot->SetPoint(iPoint, (XBins[iBin] + XBins[iPoint])/2.0, 0.0);
      plot->SetPointError(iPoint, width/2.0, width/2.0, 0.0, 0.0);

      continue;
    }

    Message(eDebug, "At bin %i, (x, y): (%f, %f)",
            iBin, (XBins[iBin] + XBins[iPoint])/2.0, NumPassed/(NumPassed + NumFailed));
    plot->SetPoint(iPoint, (XBins[iBin] + XBins[iPoint])/2.0, NumPassed/(NumPassed + NumFailed));

    width = XBins[iBin] - XBins[iPoint];

    eY_var  = NumPassed - NumFailed;
    eY_stat = NumPassed * TMath::Sqrt(1 + 4 * NumFailed) + NumFailed * TMath::Sqrt(1 + 4 * NumPassed);

    NumPassed += NumFailed;
    eY_denom = 2 * (NumPassed) * (NumPassed);

    Message(eDebug, "At bin %i, (ey_down, ey_up): (%f, %f)",
            iBin, (eY_stat + eY_var)/eY_denom, (eY_stat - eY_var)/eY_denom);

    /// @todo Check the math for the errors for triggers

    plot->SetPointError(iPoint, width/2.0, width/2.0,  // Set the x-errors to make the bin width
                        (eY_stat + eY_var)/eY_denom,   // y-error down
                        (eY_stat - eY_var)/eY_denom);  // y-error up
  }
  
  plot->GetYaxis()->SetRangeUser(0.0, 1.2);
  mapped.push_back(plot);
  SetDrawOpts("ape");
  AddCutLine(1.0, false);

  BaseCanvas(FileBase, mapped, XLabel, "Efficiency", logY);

  mapped.clear();

  delete plot;
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
