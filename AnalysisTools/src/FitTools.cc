#include "RooDataSet.h"
#include "RooKeysPdf.h"

#include "FitTools.h"

using namespace RooFit;

ClassImp(FitTools)


//--------------------------------------------------------------------
FitTools::FitTools()
{
  SetMultiplyLumi(true);
}

//--------------------------------------------------------------------
FitTools::~FitTools()
{ }

//--------------------------------------------------------------------
void
FitTools::FitCategories(TString CategoryVar, Int_t NumCategories,
                        TString ShapeVar, TString ShapeLabel)
{

  DisplayFunc(__func__);
  Message(eDebug, "About to fit %i categories %s to shape of %s",
          NumCategories, CategoryVar.Data(), ShapeVar.Data());
  Message(eDebug, "Data cut is %s", fDataBaseCut.GetTitle());
  Message(eDebug, "MC cut is   %s", fBaseCut.GetTitle());
  if (fSignalName != "")
    Message(eDebug, "Only reweighting %s", fSignalName.Data());

}
