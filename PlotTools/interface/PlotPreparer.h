/**
  @file   PlotPreparer.h

  Header of PlotPreparer class.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIE_PLOTTOOLS_PLOTPREPARER_H
#define CROMBIE_PLOTTOOLS_PLOTPREPARER_H 1

#include <map>
#include <vector>
#include <utility>

#include "TTreeFormula.h"

#include "FileConfigReader.h"
#include "ProgressReporter.h"

/**
   @ingroup plotgroup
   @class PlotInfo
   Structure for holding information about plots

*/

class PlotInfo {
 public:
  PlotInfo(TH1D* hist, Double_t& expr, Double_t& weight, Double_t eventsper)
    : hist{hist}, expr{expr}, weight{weight}, eventsper{eventsper} {}
  ~PlotInfo() { delete hist; }

  TH1D*           hist;   ///< The histogram that everything else is filling
  Double_t&       expr;   ///< The expression filling the plot
  Double_t&       weight; ///< The cut string filling the plot
  Double_t        eventsper;   ///< Holds the desired normalization of the bin for this plot
};

/**
   @ingroup plotgroup
   @class PlotPreparer
   Class used for making lots of plots in a single pass over files.
   Will usually be inherited by some other class that wants to make lots of plots.
*/

class PlotPreparer : public FileConfigReader, public ProgressReporter {
 public:
  // Use default constructor
  virtual ~PlotPreparer ();

  // Don't mask the FileConfigReader functions of the same name
  using FileConfigReader::GetHistList;
  /// Get the HistList for a given file name, and type
  std::vector<TH1D*> GetHistList ( TString FileBase, FileType type );

  /// Add a histogram to plot
  void AddHist ( TString FileBase, Int_t NumXBins, Double_t *XBins, TString dataExpr, TString mcExpr, TString dataCut, TString mcCut );
  /// Add a histogram to plot
  void AddHist ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX, TString dataExpr, TString mcExpr, TString dataCut, TString mcCut );

 protected:
  bool fPrepared {false}; ///< A flag indicating whether or not the plots have been prepared by this class

 private:
  /// A map containing pointers to all the histograms on the free store
  std::map<TString, std::map<FileType, std::vector<TH1D*>>>   fHistograms;
  /// A map containing all the formulas and results of expressions
  std::map<TString, std::pair<TTreeFormula*, Double_t>>       fFormulas;

  /// A vector of all the plots that are being made, mapped by input file names
  std::map<TString, std::vector<PlotInfo*>> fPlots;

  /// A function to clear the histograms map
  void ClearHists ();

  /// Prepare all of the plots if none have been prepared yet
  void PreparePlots ();

};

PlotPreparer::~PlotPreparer() {
  ClearHists();
}

void
PlotPreparer::ClearHists() {
  fHistograms.clear();
  for (auto& file : fPlots) {
    for (auto plot : file.second)
      delete plot;
    file.second.resize(0);
  }
  fPlots.clear();

  for (auto form : fFormulas)
    delete form.second.first;
  fFormulas.clear();
}

void
PlotPreparer::AddHist(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX, TString dataExpr, TString mcExpr, TString dataCut, TString mcCut)
{
  bool reset_per = (fEventsPer == 0);
  if (reset_per)
    SetEventsPer((MaxX - MinX)/NumXBins);

  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  AddHist(FileBase, NumXBins, XBins, dataExpr, mcExpr, dataCut, mcCut);

  if (reset_per)
    SetEventsPer(0);
}

void
PlotPreparer::AddHist(TString FileBase, Int_t NumXBins, Double_t* XBins, TString dataExpr, TString mcExpr, TString dataCut, TString mcCut)
{
  if (fHistograms.find(FileBase) == fHistograms.end())
    fHistograms[FileBase] = {};

  auto& hists = fHistograms[FileBase];

  const std::vector<TString> exprs {dataExpr, mcExpr, dataCut, mcCut};
  for (auto& expr : exprs) {
    if (fFormulas.find(expr) == fFormulas.end())
      fFormulas[expr] = std::make_pair<TTreeFormula*, Double_t>(nullptr, {});
  }

  for (auto type : gFileTypes) {
    if (hists.find(type) == hists.end())
      hists[type] = {};

    auto& hist_list = hists[type];

    auto& expr = (type == FileType::kData) ? dataExpr : mcExpr;
    auto& cut = (type == FileType::kData) ? dataCut : mcCut;

    const auto& infos = *(GetFileInfo(type));
    for (auto info : infos) {

      auto& inputname = info->fFileName;
      if (fPlots.find(inputname) == fPlots.end())
        fPlots[inputname] = {};
      auto& plots = fPlots[inputname];

      auto tempname = TempHistName();
      PlotInfo* tempinfo = new PlotInfo(new TH1D(tempname, tempname, NumXBins, XBins), fFormulas[expr].second, fFormulas[cut].second, fEventsPer);
      plots.push_back(tempinfo);
      hist_list.push_back(tempinfo->hist);
    }
  }
}

std::vector<TH1D*>
PlotPreparer::GetHistList (TString FileBase, FileType type) {
  if (not fPrepared)
    PreparePlots();

  return fHistograms.at(FileBase).at(type);
}

void
PlotPreparer::PreparePlots() {
  if (fPrepared)
    return;
  fPrepared = true;

  for (auto type : gFileTypes) {
    const auto& infos = *(GetFileInfo(type));
    for (auto info : infos) {
      auto filename = info->fFileName;
      Message(eDebug, "About to run over file %s", filename.Data()); 
      auto* inputfile = TFile::Open(filename);
      TTree* inputtree;
      if (fTreeName.Contains("/"))
        inputtree = static_cast<TTree*>(inputfile->Get(fTreeName));
      else
        inputtree = static_cast<TTree*>(inputfile->FindObjectAny(fTreeName));

      std::set<TString> needed;

      for (auto& formula : fFormulas) {
        delete formula.second.first;
        formula.second.first = new TTreeFormula(formula.first, formula.first, inputtree);
        AddNeccessaryBranches(needed, inputtree, formula.first);
      }

      inputtree->SetBranchStatus('*', 0);
      for (auto need : needed)
        inputtree->SetBranchStatus(need, 1);

      auto nentries = SetNumberOfEntries(inputtree);
      auto& plots = fPlots[filename];

      for (decltype(nentries) i_entry = 0; i_entry < nentries; ++i_entry) {
        ReportProgress(i_entry);
        inputtree->GetEntry(i_entry);

        for (auto& formula : fFormulas)
          formula.second.second = formula.second.first->EvalInstance();

        for (auto plot : plots)
          plot->hist->Fill(plot->expr, plot->weight);
      }

      inputfile->Close();

      for (auto plot : plots) {
        auto* hist = plot->hist;
        auto tempname = TempHistName();
        TH1D* tempHist = static_cast<TH1D*>(hist->Clone(TempHistName()));

        for (Int_t iBin = 1; iBin != tempHist->GetNbinsX() + 1; ++iBin)
          tempHist->SetBinContent(iBin, tempHist->GetBinWidth(iBin)/plot->eventsper);
        SetZeroError(tempHist);

        Division(hist, tempHist);

        delete tempHist;

        if (type != kData)
          hist->Scale(info->fXSecWeight);

      }
    }
  }
}

#endif
