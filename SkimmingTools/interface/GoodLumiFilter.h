/**
   @file   GoodLumiFilter.h
   Header file for GoodLumiFilter class.
   @author Daniel Abercrombie <dabercro@mit.edu> */

#ifndef CROMBIETOOLS_SKIMMINGTOOLS_GOODLUMIFILTER_H
#define CROMBIETOOLS_SKIMMINGTOOLS_GOODLUMIFILTER_H

#include <map>
#include <set>

/**
   @ingroup skimminggroup
   @class GoodLumiFilter
   @brief Created using CrombieTools.SkimmingTools.GoodLumiFilter.MakeFilter().

   Holds good events from JSON file and can return whether
   a given event is good or not. */

class GoodLumiFilter
{
 public:
  /// Returns if event with a given run and lumi number is good
  Bool_t          IsGood    ( UInt_t run, UInt_t lumi );

  GoodLumiFilter() {}
  virtual ~GoodLumiFilter() {}

  /// Add a good run and lumi number
  void            AddLumi   ( UInt_t run, UInt_t lumi )      { fGoodLumis[run].insert(lumi); }
    
 private:
  std::map<UInt_t, std::set<UInt_t> >  fGoodLumis;  ///< Map of good lumis

  UInt_t       fCurrRun = 0;                        ///< Holds current run to improve performance
  UInt_t       fCurrLumi = 0;                       ///< Holds current lumi to improve performance
  Bool_t       fCurrGood = false;                   ///< Holds current IsGood() result to improve performance

  ClassDef(GoodLumiFilter,1)
};

#endif
