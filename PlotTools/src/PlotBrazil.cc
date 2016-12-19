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

  AddLegendEntry("2-#sigma band", 5);
  AddLegendEntry("1-#sigma band", 3);
  AddLegendEntry("Expected", 1, 3, 2);
  AddLegendEntry("Observed", 1, 3, 1);

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

  Int_t Npoint = fObserved.GetN();

  // Set the single lines

  fObserved.SetPoint(Npoint, point, observed);
  fObserved.SetPointError(Npoint, 0.0, 0.0);
  fExpected.SetPoint(Npoint, point, mid);
  fExpected.SetPointError(Npoint, 0.0, 0.0);

  // Make the bands

  fOneSigma.SetPoint(Npoint, point, (low + high)/2);
  fOneSigma.SetPointError(Npoint, 0.0, (high - low)/2);

  fTwoSigma.SetPoint(Npoint, point, (lowest + highest)/2);
  fTwoSigma.SetPointError(Npoint, 0.0, (highest - lowest)/2);

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
      AddPoint(xVal.Atof(), observed.Atof(), lowest.Atof(), low.Atof(),
               mid.Atof(), high.Atof(), highest.Atof());
    else
      reading = false;
  }

  configFile.close();

}

//--------------------------------------------------------------------
void
PlotBrazil::MakePlot(TString FileBase, TString XLabel, TString YLabel,
                     Bool_t logY, Bool_t logX)
{

  std::vector<TGraphErrors*> theLines;

  fTwoSigma.SetFillColor(5);
  fOneSigma.SetFillColor(3);
  fTwoSigma.SetFillStyle(1001);
  fOneSigma.SetFillStyle(1001);

  theLines.push_back(&fTwoSigma);
  theLines.push_back(&fOneSigma);
  theLines.push_back(&fExpected);
  theLines.push_back(&fObserved);

  SetDrawFirst(0);

  BaseCanvas(FileBase, theLines, XLabel, YLabel, logY, logX);

}
