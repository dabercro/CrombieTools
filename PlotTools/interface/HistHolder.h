#ifndef CROMBIETOOLS_PLOTTOOLS_HISTHOLDER_H
#define CROMBIETOOLS_PLOTTOOLS_HISTHOLDER_H

#include "TH1D.h"
#include "TH1.h"
#include "TString.h"

// This structure is used by PlotStack to automatically order MC histograms in stack without changing colors
struct HistHolder
{
  HistHolder ( TH1D *hist, TString entry, Color_t color, Bool_t force = false )
  {
    fHist = hist; fEntry = entry; fColor = color; fForceTop = force;
    fHist->SetFillStyle(1001);
    fHist->SetFillColor(fColor);
    fHist->SetMarkerSize(0);
  }
  virtual ~HistHolder()               {}

  TH1D *fHist;
  TString fEntry;
  Color_t fColor;
  Bool_t fForceTop;
};

// A function used to sort of vector of HistHolders
Bool_t
SortHistHolders ( HistHolder *h1, HistHolder *h2 )
{
  if (h1->fForceTop)
    return true;
  else if (h2->fForceTop)
    return false;
  else
    return h1->fHist->Integral() > h2->fHist->Integral(); 
}

#endif
