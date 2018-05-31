/**
  @file   PlotPreparer.h

  Header of PlotPreparer class.

  @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIE_PLOTTOOLS_PLOTPREPARER_H
#define CROMBIE_PLOTTOOLS_PLOTPREPARER_H 1

#include <sstream>
#include <map>
#include <vector>
#include <utility>
#include <cmath>

#include "TProfile.h"
#include "TTreeFormula.h"

#include "ParallelRunner.h"
#include "PlotUtils.h"

/**
   @ingroup plotgroup
   @class EnvelopeInfo
   Similar to PlotInfo, but slightly different information for envelopes
*/

class EnvelopeInfo {
 public:
  EnvelopeInfo(TProfile* hist, Double_t& env_expr, Int_t renorm_idx)
    : hist{hist}, env_expr{env_expr}, renorm_idx{renorm_idx} {}

  TProfile*       hist;
  Double_t&       env_expr;
  Int_t           renorm_idx;
};

/**
   @ingroup plotgroup
   @class PlotInfo
   Structure for holding information about plots
*/

class PlotInfo {
 public:
  PlotInfo(TH1D* hist, Double_t& expr, Double_t& cut, Double_t& weight, Double_t eventsper)
    : hist{hist}, expr{expr}, cut{cut}, weight{weight}, eventsper{eventsper} {}
  /* ~PlotInfo() { delete hist; } // ??? Causes crash, and I really don't know why */
  ~PlotInfo() { for (auto* p : envs) delete p; envs.clear(); }

  TH1D*           hist;   ///< The histogram that everything else is filling
  Double_t&       expr;   ///< Reference to the expression filling the plot
  Double_t&       cut;    ///< Reference to the cut
  Double_t&       weight; ///< Reference to the weight
  Double_t        eventsper;       ///< Holds the desired normalization of the bin for this plot
  std::vector<EnvelopeInfo*> envs; ///< Holds the different possible envelopes for this plot
};

/**
   @ingroup plotgroup
   @class PlotPreparer
   Class used for making lots of plots in a single pass over files.
   Will usually be inherited by some other class that wants to make lots of plots.
*/

class PlotPreparer : public ParallelRunner {
 public:
  // Use default constructor
  virtual ~PlotPreparer ();

  /// Add a histogram to plot
  void AddHist ( TString FileBase, Int_t NumXBins, Double_t *XBins, TString dataExpr, TString mcExpr, TString cut, TString dataWeight, TString mcWeight );
  /// Add a histogram to plot
  void AddHist ( TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX, TString dataExpr, TString mcExpr, TString cut, TString dataWeight, TString mcWeight );

  /// Start tracking branches for a given envelope
  void StartEnvelope ( Bool_t is_up )                            { env_up = is_up; }
  void AddEnvelopeWeight ( TString weight, Int_t index = 1 )     { env_branches.push_back(make_pair(weight, index)); }

 protected:
  // Don't mask the FileConfigReader functions of the same name
  using FileConfigReader::GetHistList;
  /// Get the HistList for a given file name, and type
  std::vector<TH1D*> GetHistList ( TString FileBase, FileType type );

  /// A flag indicating whether or not the plots have been prepared by this class
  bool fPrepared {false};

 private:
  /// A map containing pointers to all the histograms on the free store, mapped by output file and filetype
  std::map<TString, std::map<FileType, std::vector<TH1D*>>>   fHistograms;
  /// A map from filename to map containing all the formulas and results of expressions
  std::map<TString, std::map<TString, std::pair<TTreeFormula*, Double_t>>>   fFormulas;

  /// A vector of all the plots that are being made, mapped by input file names
  std::map<TString, std::vector<PlotInfo*>> fPlots;

  /// A vector of all the plots that are being made, mapped by output file names and process name
  std::map<TString, std::map<TString, std::vector<PlotInfo*>>> fOutPlots;

  /// A function to clear the histograms map
  void ClearHists   ();

  /// Prepare all of the plots if none have been prepared yet
  void PreparePlots ();

  /// Runs all plots over a single file
  void RunFile (FileInfo& info) override;

  /// Scales the plots according to the stored file infos
  void ScalePlots ();

  /// Says is envelope goes up or down for a given output file
  std::map<TString, Bool_t> output_is_up;

  Bool_t env_up;                      ///< Tells whether envelope is up or down for upcoming plot
  std::vector<std::pair<TString, Int_t>> env_branches;    ///< Vector to temporarily hold branch names and renormalization indices
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
  // fOutPlots doesn't own anything
  fOutPlots.clear();

  for (auto formulas : fFormulas)
    for (auto form : formulas.second)
      delete form.second.first;
  fFormulas.clear();

  fPrepared = false;
}

void
PlotPreparer::AddHist(TString FileBase, Int_t NumXBins, Double_t MinX, Double_t MaxX, TString dataExpr, TString mcExpr, TString cut, TString dataWeight, TString mcWeight)
{
  bool reset_per = (fEventsPer == 0);
  if (reset_per)
    SetEventsPer((MaxX - MinX)/NumXBins);

  Double_t XBins[NumXBins+1];
  ConvertToArray(NumXBins, MinX, MaxX, XBins);
  AddHist(FileBase, NumXBins, XBins, dataExpr, mcExpr, cut, dataWeight, mcWeight);

  if (reset_per)
    SetEventsPer(0);
}

void
PlotPreparer::AddHist(TString FileBase, Int_t NumXBins, Double_t* XBins, TString dataExpr, TString mcExpr, TString cut, TString dataWeight, TString mcWeight)
{
  DisplayFunc(__func__);
  Message(eDebug, "File Name:", FileBase);
  Message(eDebug, "Data expr:", dataExpr);
  Message(eDebug, "MC expr:", mcExpr);
  Message(eDebug, "Cut:", cut);
  Message(eDebug, "Data cut:", dataWeight);
  Message(eDebug, "MC cut:", mcWeight);

  if (fHistograms.find(FileBase) == fHistograms.end())
    fHistograms[FileBase] = {};

  auto& hists = fHistograms[FileBase];

  // Track if this is an up or down envelope
  if (env_branches.size())
    output_is_up[FileBase] = env_up;

  for (auto type : gFileTypes) {
    if (hists.find(type) == hists.end())
      hists[type] = {};

    auto& hist_list = hists[type];

    auto exprs = (type == FileType::kData) ?
      std::vector<TString>{dataExpr, cut, dataWeight} : std::vector<TString>{mcExpr, cut, mcWeight};
    auto expr = exprs[0];
    auto weight = exprs[2];

    // Include the branches for the envelope calculation
    for (auto env : env_branches)
      exprs.push_back(env.first);

    const auto& infos = *(GetFileInfo(type));
    for (auto info : infos) {

      auto& outplots = fOutPlots[FileBase][info->fTreeName];
      auto& inputname = info->fFileName;
      auto& plots = fPlots[inputname];
      auto& formulas = fFormulas[inputname];

      for (auto& expr : exprs) {
        if (formulas.find(expr) == formulas.end()) {
          Message(eDebug, inputname, "Adding formula expression:", expr);
          formulas[expr] = std::make_pair<TTreeFormula*, Double_t>(nullptr, {});
        }
      }

      auto tempname = TempHistName();
      PlotInfo* tempinfo = new PlotInfo(new TH1D(tempname, tempname, NumXBins, XBins), formulas[expr].second, formulas[cut].second, formulas[weight].second, fEventsPer);
      for (auto env : env_branches) {
        auto tempname = TempHistName();
        tempinfo->envs.push_back(new EnvelopeInfo(new TProfile(tempname, tempname, NumXBins, XBins), formulas[env.first].second, env.second));
      }

      plots.push_back(tempinfo);
      hist_list.push_back(tempinfo->hist);
      outplots.push_back(tempinfo);
    }
  }
  env_branches.clear();
}

std::vector<TH1D*>
PlotPreparer::GetHistList (TString FileBase, FileType type) {
  PreparePlots();
  Message(eDebug, "Getting hists for FileName", FileBase, "and type", type);
  return fHistograms.at(FileBase).at(type);
}

void
PlotPreparer::PreparePlots() {
  if (fPrepared)
    return;
  fPrepared = true;

  RunThreads();
  ScalePlots();

  // Do envelope calculations
  for (auto& env : output_is_up) {
    // env.first holds the output file name
    // env.second holds the bool of whether this is up

    for (auto& procs : fOutPlots[env.first]) {
      // procs.first holds the process name
      // procs.second holds the list of PlotInfo for this process

      // Build a map of processes to up and down envelopes
      std::vector<TProfile*> profiles;
      for (auto *prof_info : fOutPlots[env.first][procs.first][0]->envs) {
        TProfile* temp_scale = static_cast<TProfile*>(prof_info->hist->Clone(TempHistName()));
        temp_scale->Reset("M");
        profiles.push_back(temp_scale);
      }
      for (auto* hist : procs.second) {
        for (unsigned i_prof = 0; i_prof < hist->envs.size(); ++i_prof)
          profiles[i_prof]->Add(hist->envs[i_prof]->hist);
      }
      // Get the max or minimum for each bin
      for (int i_bin = 1; i_bin <= profiles[0]->GetNbinsX(); ++i_bin) {
        double scale = 1.0;
        for (auto* prof : profiles) {
          auto check = prof->GetBinContent(i_bin);
          if ((check > scale) == env.second)
            scale = check;
        }
        // Scale each bin in these histograms for processes
        for (auto* hist : procs.second) {
          auto content = hist->hist->GetBinContent(i_bin);
          auto error = hist->hist->GetBinError(i_bin);
          hist->hist->SetBinContent(i_bin, content * scale);
          hist->hist->SetBinError(i_bin, error * scale);
        }
      }
      for (auto* prof : profiles)
        delete prof;
      profiles.clear();
    }    
  }
}

void
PlotPreparer::RunFile(FileInfo& info) {
  auto filename = info.fFileName;

  output_lock.Lock();
  std::cout << "About to run over file " << filename << std::endl;
  output_lock.UnLock();

  root_lock.Lock();
  auto* inputfile = TFile::Open(filename);
  TTree* inputtree = fTreeName.Contains("/") ? static_cast<TTree*>(inputfile->Get(fTreeName)) : static_cast<TTree*>(inputfile->FindObjectAny(fTreeName));
  root_lock.UnLock();

  std::set<TString> needed;

  auto& formulas = fFormulas[filename];
  for (auto& formula : formulas) {
    root_lock.Lock();
    delete formula.second.first;
    formula.second.first = new TTreeFormula(formula.first, formula.first, inputtree);
    root_lock.UnLock();
    AddNecessaryBranches(needed, inputtree, formula.first);
  }

  inputtree->SetBranchStatus("*", 0);
  for (auto need : needed)
    inputtree->SetBranchStatus(need, 1);

  auto nentries = inputtree->GetEntries();
  auto& plots = fPlots[filename];

  for (decltype(nentries) i_entry = 0; i_entry < nentries; ++i_entry) {
    if (i_entry % 1000000 == 0) {
      output_lock.Lock();
      std::cout << filename << " " << double(i_entry)/double(nentries)*100 << "%" << std::endl;
      output_lock.UnLock();
    }
    inputtree->GetEntry(i_entry);

    for (auto& formula : formulas)
      if (formula.second.first) {
        formula.second.second = formula.second.first->EvalInstance();
    }

    for (auto plot : plots) {
      if (plot->cut) {
        if (std::isnan(plot->weight)) {
          output_lock.Lock();
          Message(eError, "For file", filename, "row", i_entry, "NaN weight", plot->weight, "and expr", plot->expr);
          output_lock.UnLock();
        }
        plot->hist->Fill(plot->expr, plot->weight);
        for (auto* env : plot->envs)
          env->hist->Fill(plot->expr, env->env_expr, plot->weight);
      }
    }
  }

  root_lock.Lock();
  inputfile->Close();
  root_lock.UnLock();

  output_lock.Lock();
  std::cout << "Finished file " << filename << std::endl;
  output_lock.UnLock();
}

void PlotPreparer::ScalePlots() {
  for (auto type : gFileTypes) {
    const auto& infos = *(GetFileInfo(type));
    for (auto* info : infos) {

      auto& plots = fPlots[info->fFileName];

      for (auto plot : plots) {
        auto* hist = plot->hist;
        auto tempname = TempHistName();
        TH1D* tempHist = static_cast<TH1D*>(hist->Clone(TempHistName()));

        for (Int_t iBin = 1; iBin != tempHist->GetNbinsX() + 1; ++iBin)
          tempHist->SetBinContent(iBin, tempHist->GetBinWidth(iBin)/plot->eventsper);
        SetZeroError(tempHist);

        Division(hist, tempHist);

        delete tempHist;

        if (info->fXSec > 0) {
          hist->Scale(info->fXSecWeight);
          for (auto* env : plot->envs) {
            auto denom = info->fRenormHistogram.GetBinContent(std::min(env->renorm_idx,
                                                                       info->fRenormHistogram.GetNbinsX()));
            if (denom)
              env->hist->Scale(info->fRenormHistogram.GetBinContent(1)/denom);
          }
        }
      }
    }
  }
}

#endif
