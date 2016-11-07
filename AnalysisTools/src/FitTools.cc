#include <vector>

#include "TROOT.h"
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
TString
FitTools::GetJointPdf(const char* name, std::vector<TString> files, FileType type)
{
  DisplayFunc(__func__);

  Message(eDebug, "Input name: %s", name);

  std::vector<FileInfo*> *info = GetFileInfo(type);

  // Open the files and get the TTrees
  OpenFiles(files);

  // Initialize an empty dataset
  TString dataname = TString::Format("Dataset_%s", name);

  Message(eDebug, "Created dataset with name %s", dataname.Data());
  
  RooArgSet argset = RooArgSet(*fWorkspace.var(fDefaultExpr.Data()),
                               *fWorkspace.var(fMCWeight.GetTitle()),
                               *fWorkspace.cat(fCategoryBranch));

  RooDataSet dataset(dataname, dataname, argset);

  // Add the data from each tree to the dataset
  gROOT->cd();
  for (UInt_t iTree = 0; iTree != fInTrees.size(); ++iTree) {
    Message(eDebug, "Getting data from tree %i / %i", iTree, fInTrees.size());

    TString datasetname = TString::Format("Dataset_%s_%i", name, iTree);
    TTree *copiedTree = fInTrees[iTree]->CopyTree(fBaseCut.GetTitle());
    RooDataSet *tempData = new RooDataSet(datasetname, datasetname, copiedTree,
                                          argset, 0, fMCWeight);

    dataset.append(*tempData);
    Message(eDebug, "Dataset now has %i entries", dataset.numEntries());
  }

  // Split and add these pdfs together with the proper relative weights, if needed
  TString pdfName = TString::Format("pdf_%s", name);

  Message(eDebug, "Creating pdf: %s", pdfName.Data());

  // create the pdf

  if (name[0] == 'F') {

    TList *dataList = dataset.split(*fWorkspace.cat(fCategoryBranch));

    Message(eDebug, "Dataset list size: %i", dataList->GetSize());

    RooArgSet pdfSet;
    RooArgSet coeffSet;

    for (UInt_t iBin = 0; iBin != fCategoryNames.size(); ++iBin) {

      Message(eDebug, "Getting pdf for category: %s", fCategoryNames[iBin].Data());

      TString pdfNameTemp = TString::Format("pdf_%s_%i", name, iBin);
      Message(eDebug, "The pdf name will be %s", pdfNameTemp.Data());
      RooKeysPdf *tempPdf = new RooKeysPdf(pdfNameTemp, pdfNameTemp, *fWorkspace.var(fDefaultExpr),
                                           *(static_cast<RooDataSet*>(dataList->At(iBin))));
      Message(eDebug, "Keys pdf created at %p", tempPdf);
      pdfSet.add(*tempPdf);

      if (iBin != fCategoryNames.size() - 1) {
        TString coeffName = TString::Format("coeff_%s_%i", name, iBin);
        Message(eDebug, "Added coefficient named %s", coeffName.Data());
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

  return pdfName;

}

//--------------------------------------------------------------------
void
FitTools::FitCategories(Double_t Shape_Min, Double_t Shape_Max, const char* ShapeLabel)
{

  DisplayFunc(__func__);
  Message(eDebug, "About to fit %i categories %s to shape of %s",
          fCategoryNames.size(), fCategoryBranch, fDefaultExpr.Data());
  Message(eDebug, "Data cut is %s", fDataBaseCut.GetTitle());
  Message(eDebug, " MC  cut is %s", fBaseCut.GetTitle());
  if (fSignalName != "")
    Message(eDebug, "Only reweighting %s", fSignalName.Data());

  Double_t XBins[2];
  ConvertToArray(1, Shape_Min, Shape_Max, XBins);

  // Create the variable being plotted
  RooRealVar variable = RooRealVar(fDefaultExpr.Data(), ShapeLabel, Shape_Min, Shape_Max);

  // Create weight variable
  RooRealVar MCWeight = RooRealVar(fMCWeight.GetTitle(), "MC Weight", 0.0);

  // Create category variable
  RooCategory category = RooCategory(fCategoryBranch, "categories");

  // Set category names
  for (UInt_t iCat = 0; iCat != fCategoryNames.size(); ++iCat)
    category.defineType(fCategoryNames[iCat], iCat);

  fWorkspace.import(variable);
  fWorkspace.import(MCWeight);
  fWorkspace.import(category);

  TH1D *FloatSizer = GetHist(1, XBins, fSignalType, fSignalName, fSearchBy, true);
  Double_t FloatSize = FloatSizer->Integral("width");

  // Get the pdf that we'll be floating, named "pdf_Floating"
  TString float_name = GetJointPdf("Floating", ReturnFileNames(fSignalType, fSignalName, fSearchBy, true), fSignalType);

  TH1D *StaticSizer = GetHist(1, XBins, kBackground, fSignalName, fSearchBy, false);
  Double_t StaticSize = StaticSizer->Integral("width") * (1.0 + fBackgroundChange);

  // Get the other background files that will be static, named "pdf_Static"
  TString static_name = GetJointPdf("Static", ReturnFileNames(kBackground, fSignalName, fSearchBy, false));

  Double_t relativeSize = FloatSize/(FloatSize + StaticSize);

  Message(eDebug, "Finished importing pdfs. Floating: %f, Static: %f, Relative fraction: %f", FloatSize, StaticSize, relativeSize);

  Message(eDebug, "About to make histograms from %s and %s at %p and %p",
          float_name.Data(), static_name.Data(), fWorkspace.pdf(float_name), fWorkspace.pdf(static_name));

  RooRealVar relativePdfs = RooRealVar("Relative", "Relative", relativeSize);
  RooAddPdf finalPdf = RooAddPdf("Final Pdf", "Final Pdf",
                                 *fWorkspace.pdf(float_name),
                                 *fWorkspace.pdf(static_name),
                                 relativePdfs);

  Message(eDebug, "Final pdf created at %p", &finalPdf);

  // Get the dataset to fit
  TChain *dataChain = ReturnTChain(fTreeName, kData);
  TTree *dataTree = dataChain->CopyTree(fDataBaseCut.GetTitle());

  Message(eDebug, "TTree at %p created from TChain at %p", dataTree, dataChain);

  RooDataSet dataDataset = RooDataSet("data", "data", dataTree, RooArgSet(variable));

  Message(eDebug, "Data imported with size %i", dataDataset.numEntries());

  for (UInt_t iBin = 0; iBin != fCategoryNames.size(); ++iBin) {
    TString coeffName = TString::Format("coeff_%s_%i", "Floating", iBin);
    Message(eInfo, "Coefficient %s has value %f", coeffName.Data(), fWorkspace.var(coeffName)->getValV());
  }

  finalPdf.fitTo(dataDataset);

  for (UInt_t iBin = 0; iBin != fCategoryNames.size(); ++iBin) {
    TString coeffName = TString::Format("coeff_%s_%i", "Floating", iBin);
    Message(eInfo, "Coefficient %s has value %f", coeffName.Data(), fWorkspace.var(coeffName)->getValV());
  }

  delete FloatSizer;
  delete StaticSizer;

}
