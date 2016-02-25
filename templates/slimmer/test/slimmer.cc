#include <iostream>

#include "TRandom2.h"
#include "TString.h"
#include "OutTree.h"

enum dataType 
  {
    kNotData = 0, 
    kSignal, kMC1, kMC2, kMC3
  };

void slimmer(TString inFileName, TString outFileName)
{
  TRandom2 *gen = new TRandom2(1234);
  UInt_t numEvents = 10000;
  OutTree *output = new OutTree("test",outFileName);

  Float_t genHolder = 0;
  dataType holdType = kNotData;

  // I should have a distribution to plot for signal
  // distribution for background (weight the MC, do Data correctly)
  // Different distributions for signal and background discriminators

  for (UInt_t iEvent = 0; iEvent != numEvents; ++iEvent) {

    output->lumiNum = iEvent % 100 + 1;
    output->eventNum  = iEvent + 1;

    if (inFileName != "Data.root") {
      output->weight = gen->Gaus(1.0,0.2);
      output->runNum = 1;
    }
    else {
      // If data, figure out which kind
      output->runNum = 2;
      genHolder = gen->Uniform();
      if (genHolder <= 0.02)
        holdType = kSignal;
      else if (genHolder <= 0.52)
        holdType = kMC1;
      else if (genHolder <= 0.9)
        holdType = kMC2;
      else
        holdType = kMC3;
    }

    if (inFileName == "Signal.root" || holdType == kSignal) {   // Signal is 1/50 of the events
      output->example = gen->Gaus(60.0,5.0);
      output->exampleDisc1 = sqrt(gen->Uniform());
      output->exampleDisc2 = sqrt(gen->Uniform());
    }
    else {
      if (inFileName == "MC1.root" || holdType == kMC1) {       // MC1 is 1/2 of the events
        output->eventNum += numEvents * 1;
        genHolder = gen->Uniform(0.0,10.0);
        output->example = genHolder * genHolder;
      }
      else if (inFileName == "MC2.root" || holdType == kMC2) {  // MC2 is 2/5 - 1/50 of the events
        output->eventNum += numEvents * 2;
        if (holdType == kNotData)
          output->example = gen->Uniform(0.0,100.0);            // This needs correcting in later test
        else
          output->example = 100 * (1 - sqrt(gen->Uniform()));
      }
      else if (inFileName == "MC3.root" || holdType == kMC3) {  // MC3 is 1/10 of the events
        output->eventNum += numEvents * 3;
        output->example = gen->Uniform(0.0,100.0);
      }
      else {
        std::cout << "A wrong input FileName was given. Edit slimmer.cc if you are not running tests." << std::endl;
        exit(1);
      }
      output->exampleDisc1 = 1 - sqrt(gen->Uniform());
      output->exampleDisc2 = gen->Uniform();
    }

    output->Fill();
  }

  output->Write();
  delete output;
  delete gen;
}
