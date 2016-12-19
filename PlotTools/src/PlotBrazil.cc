/**
  @file   PlotBrazil.cc
  Defines the PlotBrazil class.
  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#include <vector>
#include <fstream>

#include "PlotBrazil.h"

ClassImp(PlotBrazil)

//--------------------------------------------------------------------
PlotBrazil::PlotBrazil( TString fileName )
{

  if (fileName != "")
    ReadConfig(fileName);

}

//--------------------------------------------------------------------
PlotBrazil::~PlotBrazil()
{ }


//--------------------------------------------------------------------
void
PlotBrazil::AddPoint ( Double_t point, Double_t observed, Double_t lowest, Double_t low,
                       Double_t mid, Double_t high, Double_t highest )
{

  DisplayFunc(__func__);
  Message(eDebug, "Adding point at %f, observed: %f, limit band: %f, %f, %f, %f, %f",
          point, observed, lowest, low, mid, high, highest);

  Int_t point = fObserved.GetN();

  // Set the single lines

  fObserved.SetPoint(point, observed);
  fExpected.SetPoint(point, mid);

  // Make the bands

  fOneSigma.SetPoint(point, (low + high)/2);
  fOneSigma.SetPointError(point, 0.0, (high - low)/2);

  fTwoSigma.SetPoint(point, (lowest + highest)/2);
  fTwoSigma.SetPointError(point, 0.0, (highest - lowest)/2);

  // Sort everything, just in case

  fExpected.Sort();
  fObserved.Sort();
  fOneSigma.Sort();
  fTwoSigma.Sort();

}

//--------------------------------------------------------------------
void
PlotBrazil::ReadConfig(TString filename)
{

  DisplayFunc(__func__);
  Message(eInfo, "Reading config file: %s", filename.Data());

  std::ifstream configFile;
  configFile.open(filename.Data());
  TString xVal;
  TString observed;
  TString lowest, low, mid, high, highest;
  Bool_t reading = true;

  while (reading) {
    configFile >> xVal >> observed >> lowest >> low >> mid >> high >> highest;
    if (highest != "")
      AddPoint(xVal, observed, lowest, low, mid, high, highest);
    else
      reading = false;
  }

  configFile.close();

}

//--------------------------------------------------------------------
void
MakePlot(TString FileBase, TString XLabel, TString YLabel,
         Bool_t logY, Bool_t logX)
{

  std::vector<TGraph*> theLines;

  fTwoSigma.SetFillColor(5);
  fOneSigma.SetFillColor(3);
  fTwoSigma.SetFillStyle(1001);
  fOneSigma.SetFillStyle(1001);

  fExpected.SetLineStyle(2);
  fObserved.SetLineStyle(1);
  fExpected.SetLineWidth(2);
  fObserved.SetLineWidth(2);

  theLines.push_back(&fTwoSigma);
  theLines.push_back(&fOneSigma);
  theLines.push_back(&fExpected);
  theLines.push_back(&fObserved);

  SetDrawFirst(0);

  BaseCanvas(FileBase, theLines, XLabel, YLabel, logY, logX);

}
