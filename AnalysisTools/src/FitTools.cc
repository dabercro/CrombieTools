#include <vector>

#include "TH1D.h"
#include "TList.h"

#include "RooAddPdf.h"
#include "RooArgSet.h"
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
void
FitTools::GetJointPdf(const char* name, std::vector<TString> files, FileType type)
{
  std::vector<FileInfo*> *info = GetFileInfo(type);

  // Open the files and get the TTrees
  OpenFiles(files);

  // Initialize an empty dataset
  const char *dataname = TString::Format("Dataset_%s", name);
  RooDataSet dataset(dataname, dataname, 
                     RooArgSet(*fWorkspace.var(fDefaultExpr),
                               *fWorkspace.cat("categories")));

  // Add the data from each tree to the dataset
  for (UInt_t iTree = 0; iTree != fInTrees.size(); ++iTree) {
    const char *datasetname = TString::Format("Dataset_%s_%i", name, iTree).Data();
    RooDataSet *tempData = new RooDataSet(datasetname, datasetname, fInTrees[iTree],
                                          RooArgSet(*fWorkspace.var(fDefaultExpr),
                                                    *fWorkspace.cat("categories")),
                                          fBaseCut, fMCWeight);
    dataset.append(*tempData);
  }

  // Split and add these pdfs together with the proper relative weights, if needed
  const char *pdfName = TString::Format("pdf_%s", name);

  if (name[0] == 'F') {

    TList *dataList = dataset.split(*fWorkspace.cat("categories"));

    Message(eDebug, "Dataset list size: %i", dataList->GetSize());

    RooArgSet pdfSet;
    RooArgSet coeffSet;

    for (UInt_t iBin = 0; iBin != fCategoryNames.size(); ++iBin) {

      const char *pdfNameTemp = TString::Format("pdf_%s_%i", name, iBin);
      RooKeysPdf *tempPdf = new RooKeysPdf(pdfNameTemp, pdfNameTemp, *fWorkspace.var(fDefaultExpr),
                                           *(static_cast<RooDataSet*>(dataList->At(iBin))));
      pdfSet.add(*tempPdf);

      if (iBin != fCategoryNames.size() - 1) {
        const char *coeffName = TString::Format("coeff_%s_%i", name, iBin);
        RooRealVar *tempVar = new RooRealVar(coeffName, coeffName, 0.0, 1.0);
        coeffSet.add(*tempVar);
      }
    }

    RooAddPdf finalPdf(pdfName, pdfName, pdfSet, coeffSet);
    fWorkspace.import(finalPdf);

  }
  else {

    RooKeysPdf finalPdf(pdfName, pdfName, *fWorkspace.var(fDefaultExpr), dataset);
    fWorkspace.import(finalPdf);

  }

  CloseFiles();

  // create the pdf

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

  Double_t XBins[2];
  ConvertToArray(1, Shape_Min, Shape_Max, XBins);

  // Create the variable being plotted
  RooRealVar variable = RooRealVar(fDefaultExpr, ShapeLabel, Shape_Min, Shape_Max);

  // Create category variable
  RooCategory category = RooCategory("categories", "categories");

  // Set category names
  for (UInt_t iCat = 0; iCat != fCategoryNames.size(); ++iCat)
    category.defineType(fCategoryNames[iCat], iCat);

  fWorkspace.import(variable);
  fWorkspace.import(category);

  TH1D *FloatSize = GetHist(1, XBins, fSignalType, fSignalName, fSearchBy, true);

  // Get the pdf that we'll be floating, named "pdf_Floating"
  GetJointPdf("Floating", ReturnFileNames(fSignalType, fSignalName, fSearchBy, true), fSignalType);

  TH1D *StaticSize = GetHist(1, XBins, kBackground, fSignalName, fSearchBy, false);

  // Get the other background files that will be static, named "pdf_Static"
  GetJointPdf("Static", ReturnFileNames(kBackground, fSignalName, fSearchBy, false));

}
