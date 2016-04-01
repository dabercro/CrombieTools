/**
  @file   GoodLumiFilter.cc
  File describing GoodLumiFilter::IsGood()
  @author Daniel Abercrombie <dabercro@mit.edu> */

#include "GoodLumiFilter.h"

ClassImp(GoodLumiFilter)

//--------------------------------------------------------------------
GoodLumiFilter::GoodLumiFilter()
{ }

//--------------------------------------------------------------------
GoodLumiFilter::~GoodLumiFilter()
{ }

//--------------------------------------------------------------------
Bool_t
GoodLumiFilter::IsGood(UInt_t run, UInt_t lumi)
{
  if (run == 1 || fGoodLumis.size() == 0)
    return true;

  if (run == fCurrRun && lumi == fCurrLumi)
    return fCurrGood;

  fCurrRun = run;
  fCurrLumi = lumi;
  fCurrGood = false;

  if (fGoodLumis.find(fCurrRun) != fGoodLumis.end())
    if (fGoodLumis[fCurrRun].find(fCurrLumi) != fGoodLumis[fCurrRun].end())
      fCurrGood = true;

  return fCurrGood;
}
