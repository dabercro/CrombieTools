#include <vector>

#include "RooArgList.h"
#include "RooRealVar.h"
#include "RooCategory.h"
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
RooAddPdf*
FitTools::GetJointPdf(const char* name, std::vector<TString> files,
                      RooRealVar &variable, RooCategory &categorty, FileType type)
{
  std::vector<FileInfo*> *info = GetFileInfo(type);

  // Open the files and get the TTrees
  OpenFiles(files);

  // Create the various pdfs from the TTrees
  RooArgList pdfList = RooArgList();
  RooArgList coefficientList = RooArgList();

  for (std::vector<TTree*>::iterator iTree = fTrees.begin(); iTree != fTrees.end(); ++iTree) {
    
  }

  CloseFiles();
  // Split and add these pdfs together with the proper relative weights

  // return the joint pdf
  RooAddPdf *outputPdf = new RooAddPdf(name, name, pdfList, coefficientList);
  return outputPdf;

}

//--------------------------------------------------------------------
void
FitTools::FitCategories(TString CategoryVar, Int_t NumCategories,
                        Double_t Shape_Min, Double_t Shape_Max, const char* ShapeLabel)
{

  DisplayFunc(__func__);
  Message(eDebug, "About to fit %i categories %s to shape of %s",
          NumCategories, CategoryVar.Data(), fDefaultExpr.Data());
  Message(eDebug, "Data cut is %s", fDataBaseCut.GetTitle());
  Message(eDebug, " MC  cut is %s", fBaseCut.GetTitle());
  if (fSignalName != "")
    Message(eDebug, "Only reweighting %s", fSignalName.Data());

  // Create category variable
  RooCategory category = RooCategory("categories", "categories");

  // Set category names
  for (UInt_t iCat = 0; iCat != fCategoryNames.size(); ++iCat)
    category.defineType(fCategoryNames[iCat], iCat);

  // Create the variable being plotted
  RooRealVar variable = RooRealVar(fDefaultExpr, ShapeLabel, Shape_Min, Shape_Max);

  // Get the pdf that we'll be floating
  RooAddPdf *FloatPdf = GetJointPdf("Floating", ReturnFileNames(fSignalType, fSignalName, fSearchBy, true),
                                    variable, category, fSignalType);

  // Get the other background files that will be static
  RooAddPdf *StaticPdf = GetJointPdf("Static", ReturnFileNames(kBackground, fSignalName, fSearchBy, false),
                                     variable, category);

}
