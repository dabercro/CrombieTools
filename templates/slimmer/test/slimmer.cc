#include <iostream>

#include "TH1F.h"
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
  UInt_t seed = 1234;
  UInt_t numEvents = 100000;

  if (inFileName == "Data.root")
    seed += 100;
  else
    numEvents *= 5;

  if (inFileName == "Signal.root")
    seed += 1000;
  if (inFileName == "MC1.root")
    seed += 1;
  if (inFileName == "MC2.root")
    seed += 2;
  if (inFileName == "MC3.root")
    seed += 3;

  TRandom2 *gen = new TRandom2(seed);
  OutTree *output = new OutTree("test",outFileName);

  Float_t genHolder = 0;
  dataType holdType = kNotData;

  TH1F *htotal = new TH1F("htotal", "htotal", 1, -1, 1);

  // I should have a distribution to plot for signal
  // distribution for background (weight the MC, do Data correctly)
  // Different distributions for signal and background discriminators

  for (UInt_t iEvent = 0; iEvent != numEvents; ++iEvent) {
    output->Reset();
    htotal->Fill(0);

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
      output->isSignal = 1;
      output->example = gen->Gaus(60.0,5.0);
      output->exampleDisc1 = sqrt(gen->Uniform());
      output->exampleDisc2 = sqrt(gen->Uniform());
    }
    else {
      if (inFileName == "MC1.root" || holdType == kMC1) {       // MC1 is 1/2 of the events
        output->eventNum += numEvents * 1;
        genHolder = gen->Uniform(0.0,10.0);
        output->example = genHolder * genHolder;
        if (holdType == kMC1 && (output->example) < 40.0 && gen->Uniform() > (1.0 + (output->example - 40.0)/60.0))
          continue;
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

  output->ReturnFile()->WriteTObject(htotal, "htotal");
  output->Write();
  delete output;
  delete gen;
}
