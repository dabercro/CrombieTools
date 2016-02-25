#include <iostream>

#include "TRandom2.h"
#include "TString.h"
#include "OutTree.h"

void slimmer(TString inFileName, TString outFileName)
{
  TRandom2 *gen = new TRandom2(1234);
  UInt_t numEvents = 10000;
  OutTree *output = new OutTree("test",outFileName);

  // I should have a distribution to plot for signal
  // distribution for background (weight the MC, do Data correctly)
  // Different distributions for signal and background discriminators

  for (UInt_t iEvent = 0; iEvent != numEvents; ++iEvent) {
    if (inFileName == "Data.root") {
    
    }
    else if (iFileName == "Signal.root") {
    
    }
    else {
      if (iFileName == "MC1.root") {
      
      }
      else if (iFileName == "MC2.root") {
      
      }
      else if (iFileName == "MC3.root") {
        
      }
    }
    output->Fill();
  }

  output->Write();
  delete output;
  delete gen;
}
