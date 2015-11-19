#ifndef CROMBIETOOLS_SKIMMINGTOOLS_GOODLUMIFILTER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_GOODLUMIFILTER_H

#include <map>
#include <set>

class GoodLumiFilter
{
 public:
  GoodLumiFilter()
  virtual ~GoodLumiFilter() {}

  void         AddLumi   ( UInt_t run, UInt_t lumi )      { fGoodLumis[run].insert(lumi); }
  Bool_t       IsGood    ( UInt_t run, UInt_t lumi );
    
 private:
  std::map<UInt_t, std::set<UInt_t>>  fGoodLumis;

  UInt_t       fCurrRun;
  UInt_t       fCurrLumi;
  Bool_t       fCurrGood;

  ClassDef(GoodLumiFilter,1)
};
