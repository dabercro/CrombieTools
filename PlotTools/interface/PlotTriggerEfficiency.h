/**
  @file   PlotTriggerEfficiency.h
  Definition of PlotTriggerEfficiency class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/


#ifndef CROMBIETOOLS_PLOTTOOLS_PLOTTRIGGEREFFICIENCY_H
#define CROMBIETOOLS_PLOTTOOLS_PLOTTRIGGEREFFICIENCY_H

#include "TString.h"
#include "TChain.h"

#include "InDirectoryHolder.h"
#include "PlotBase.h"

/**
   @ingroup plotgroup
   @class PlotTriggerEfficiency
   A class that creates trigger efficiency plots using a base cut, a variable to plot, and a trigger.
   This class is accessed through CrombieTools.PlotTools.PlotTriggerEfficiency.
*/

class PlotTriggerEfficiency : public PlotBase, public InDirectoryHolder
{
 public:
  PlotTriggerEfficiency          ()   { fChain = new TChain("TriggerChain", "TriggerChain"); }
  virtual ~PlotTriggerEfficiency ()   { delete fChain; }

  /// Only takes data files, so this function is named appropriately
  void  AddDataFile      ( TString name )   { fChain->Add(AddInDir(name) + "/events"); }

  /// Reset the data files
  void  ResetDataFiles   ()                 { delete fChain; fChain = new TChain();    }

  /**
     Sets the cut for the trigger, which must evaluate to 1 or 0.
     The other cut should be set using PlotBase::SetDefaultCut
  */
  void  SetTriggerCut     ( char* cut )     { fTrigger = cut; }

  /// Plot the trigger efficiencies
  void  MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t *XBins,
                            TString XLabel, TString = "", Bool_t logY = false );
  /// Plot the trigger efficiencies
  void  MakeCanvas        ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX,
                            TString XLabel, TString = "", Bool_t logY = false);

 private:

  TChain *fChain;         ///< The TChain that holds all the files in the plot
  TString fTrigger = "";  ///< Cut for the trigger

  ClassDef(PlotTriggerEfficiency, 1)
};

#endif
