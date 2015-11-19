#include "GoodLumiFilter.h"

ClassImp(GoodLumiFilter)

//--------------------------------------------------------------------
GoodLumiFilter::GoodLumiFilter() :
  fLastRun(0),
  fLastLumi(0),
  fLastGood(false)
{}

//--------------------------------------------------------------------
void
GoodLumiFilter::IsGood(UInt_t run, UInt_t lumi)
{
  if (run == 1)
    return true;

  if (run == fCurrRun && lumi == fCurrLumi)
    return fCurrGood;

  fCurrRun = run;
  fCurrLumi = lumi;
  fCurrGood = false;

  auto theRun = fGoodLumis.find(fCurrRun)
  if (theRun != fGoodLumis.end())
    if (theRun.find(fCurrLumi) != theRun.end())
      fCurrGood = true;

  return fCurrGood;
}
