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

  if (inFileName == "Data.root") {
    
  }
  else if (iFileName == "MC.root") {
    
  }
  else if (iFileName == "Signal.root") {
    
  }
  else {
    std::cout << "You don't seem to be running the test script." << std::endl;
    std::cout << "You should probably modify slimmer.cc for your analysis!" << std::endl;
  }

  delete gen;
  delete output;
}
