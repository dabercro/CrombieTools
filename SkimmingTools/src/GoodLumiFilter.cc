#include "GoodLumiFilter.h"

ClassImp(GoodLumiFilter)

//--------------------------------------------------------------------
GoodLumiFilter::GoodLumiFilter() :
  fCurrRun(0),
  fCurrLumi(0),
  fCurrGood(false)
{}

//--------------------------------------------------------------------
Bool_t
GoodLumiFilter::IsGood(UInt_t run, UInt_t lumi)
{
  if (run == 1)
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
