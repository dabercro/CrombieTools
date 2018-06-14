#ifndef CROMBIE_PLOTTER_H
#define CROMBIE_PLOTTER_H

#include <string>
#include <vector>
#include <map>
#include <list>
#include <mutex>

#include "Selection.h"
#include "PlotConfig.h"
#include "FileConfig.h"
#include "LoadTree.h"
#include "Hist.h"

#include "TFile.h"
#include "TH1.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"

namespace Crombie {
  namespace Plotter {

    /**
       This is the output running over a single file.
       The first number is the number of events for cross section normalization.
       The key corresponds to a selection, and the different hists are different process cuts.
    */
    using SingleOut = std::pair<double, std::map<std::string, std::vector<std::vector<Hist::Hist>>>>;

    /// Constructs a function that runs over a single file and produces all the necessary histograms
    std::function<SingleOut(const FileConfig::FileInfo&)>
      SingleFile (const std::vector<PlotConfig::Plot>& plots,
                  const Selection::SelectionConfig& selections);

    /// This class has everything needed to draw a plot
    class Plot {
    public:
      void  draw  (const std::string& filebase, double lumi);
      void  dumpdatacard (const std::string& filename);
      void  add (unsigned isub, const FileConfig::DirectoryInfo& info, const Hist::Hist& hist, double mcweight);
    private:
      struct PlotInfo {
      PlotInfo(const Hist::Hist& hist = {}, double mcweight = {}, double xsec = {},
               std::string entry = {}, FileConfig::Type type = {}, short style = {})
      : hist{hist}, mcweight{mcweight}, xsec{xsec}, entry{entry}, type{type}, style{style} {}
        Hist::Hist hist;
        double mcweight;
        double xsec;
        std::string entry;
        FileConfig::Type type;
        short style;
      };
      /// First key is directory, then label
      std::map<std::string, std::map<std::string, PlotInfo>> plotstore;
    };

    /**
       Merges the output of the SingleFile functional
    */
    std::map<std::string, std::vector<Plot>> Merge (const std::vector<FileConfig::DirectoryInfo>& dirinfos,
                                                    const std::map<std::string, std::vector<SingleOut>>& outputs);


    // IMPLEMENTATIONS BELOW HERE //

    namespace {

      class CutReader {
      public:
      CutReader(double& cut, double& expr, double& weight, double& sub, Hist::Hist& hist)
        : cut{cut}, expr{expr}, weight{weight}, sub{sub}, hist{hist} { }
        double& cut;    // For the selection
        double& expr;
        double& weight;
        double& sub;    // For the subprocess
        Hist::Hist& hist;
        void eval () {
          if (cut and sub)
            hist.fill(expr, weight);
        }
      };

    }

    std::function<SingleOut(const FileConfig::FileInfo&)>
      SingleFile (const std::vector<PlotConfig::Plot>& plots,
                  const Selection::SelectionConfig& selections) {
      return std::function<SingleOut(const FileConfig::FileInfo&)> {
        [&plots, &selections] (const FileConfig::FileInfo& info) {
          LoadTree::rootlock.lock();
          TFile input {info.name.data()};
          LoadTree::rootlock.unlock();

          SingleOut output {
            static_cast<TH1*>(input.Get(selections.mchistname.data()))->GetBinContent(1),
            {}
          };

          // Build the formulas and plots to use
          auto get_expr = (info.type == FileConfig::Type::Data) ?
            [] (const PlotConfig::Plot& iter) { return iter.data_var; } :
            [] (const PlotConfig::Plot& iter) { return iter.mc_var; };

          auto exprs = Misc::comprehension<std::string>(plots, get_expr);

          using SelIter = Selection::SelectionConfig::Selections::value_type;
          auto get_weight = (info.type == FileConfig::Type::Data) ?
            [] (const SelIter& iter) { return iter.second.data; } :
            [] (const SelIter& iter) { return iter.second.mc; };
          auto get_cut =  [] (const SelIter& iter) { return iter.second.cut; };

          auto weights = Misc::comprehension<std::string>(selections.selections, get_weight);
          auto cuts = Misc::comprehension<std::string>(selections.selections, get_cut);

          auto loaded = LoadTree::load_tree(input, exprs, weights, cuts, info.cuts);

          std::list<CutReader> readers {};

          for (auto& sel : selections.selections) {
            auto& selelement = output.second[sel.first];
            for (auto& plot : plots) {
              selelement.push_back({});
              auto& plotvec = selelement.back();
              // We want to reserve the location for the vector because we want the references to stay valid
              plotvec.reserve(info.cuts.size());
              for (auto& sub : info.cuts) {
                plotvec.push_back(plot.get_hist());
                readers.emplace_back(loaded.second.result(get_cut(sel)),
                                     loaded.second.result(get_expr(plot)),
                                     loaded.second.result(get_weight(sel)),
                                     loaded.second.result(sub),
                                     plotvec.back());
              }
            }
          }

          auto nentries = loaded.first->GetEntries();
          for (decltype(nentries) ientry = 0; ientry < nentries; ++ientry) {
            loaded.first->GetEntry(ientry);
            loaded.second.eval();
            for (auto& reader : readers)
              reader.eval();
          }

          LoadTree::rootlock.lock();
          input.Close();
          LoadTree::rootlock.unlock();

          return output;
        }
      };
    }

    std::map<std::string, std::vector<Plot>> Merge (const std::vector<FileConfig::DirectoryInfo>& dirinfos,
                                                    const std::map<std::string, std::vector<SingleOut>>& outputs) {

      // using SingleOut = std::pair<double, std::map<std::string, std::vector<std::vector<Hist::Hist>>>>;

      std::map<std::string, std::vector<Plot>> output {};
      if (dirinfos.size()) {
        // A vector of outputs from each file
        const auto& files = outputs.at(dirinfos.front().name);
        // We only care about map for the first for now
        auto& singleout = files.front().second;
        
        for (auto& sel : singleout) {
          for (auto& plot : sel.second)
            output[sel.first].push_back({});
        }

        for (auto& info : dirinfos) {
          const auto& dirhists = outputs.at(info.name);
          for (auto& single : dirhists) {
            // Selections
            for (auto& sel : single.second) {
              // Plots
              for (unsigned iexpr = 0; iexpr < sel.second.size(); ++iexpr) {
                for (unsigned isub = 0; isub < info.processes.size(); ++isub) {
                  output[sel.first][iexpr].add(isub, info, sel.second[iexpr][isub], single.first);
                }
              }
            }
          }
        }
      }

      return output;
    }

    void Plot::add(unsigned isub, const FileConfig::DirectoryInfo& info, const Hist::Hist& hist, double mcweight) {
      Debug::Debug("weight", mcweight);
      // Process name
      auto& proc = info.processes.at(isub);
      auto& dir = plotstore[info.name];
      if (dir.find(proc.treename) == dir.end())
        dir[proc.treename] = PlotInfo(hist, mcweight, info.xs, proc.legendentry, info.type, proc.style);
      else {
        auto& toadd = dir[proc.treename];
        toadd.hist.add(hist);
        toadd.mcweight += mcweight;
      }
    }

    void Plot::draw(const std::string& filebase, double lumi) {
      // Legend label
      std::map<FileConfig::Type, std::map<std::string, TH1D*>> hists;
      for (auto& dir : plotstore) {
        for (auto& proc : dir.second) {
          auto& histcol = hists[proc.second.type];
          auto* newhist = proc.second.hist.roothist();
          Debug::Debug("New hist at", newhist, newhist->Integral());
          Debug::Debug("Scale", proc.second.xsec, lumi, proc.second.mcweight);
          newhist->Scale(proc.second.xsec * lumi / proc.second.mcweight);
          if (histcol.find(proc.second.entry) != histcol.end())
            histcol[proc.second.entry]->Add(newhist);
          else {
            Debug::Debug("new");
            newhist->SetFillColor(proc.second.style);
            histcol[proc.second.entry] = newhist;
          }
          auto* hist = histcol[proc.second.entry];
          Debug::Debug("Master hist at", hist, hist->Integral());
        }
      }
      // Now stuff MC into a vector and sort
      auto sorted_vec = [] (auto& hists) {
        std::vector<std::pair<std::string, TH1D*>> sortvec;
        sortvec.insert(sortvec.end(), hists.begin(), hists.end());
        std::sort(sortvec.begin(), sortvec.end(), [] (auto& a, auto& b) {
            Debug::Debug(a.second->Integral());
            return a.second->Integral() > b.second->Integral();
          });
        return sortvec;
      };

      auto mcvec = sorted_vec(hists[FileConfig::Type::Background]);
      auto sigvec = sorted_vec(hists[FileConfig::Type::Signal]);

      THStack hs{"hs", ""};
      TLegend leg{};
      for (auto& mc : mcvec) {
        leg.AddEntry(mc.second, mc.first.data());
        hs.Add(mc.second);
      }

      TCanvas canv{"canv", "canv"};
      canv.cd();
      hs.Draw();
      leg.Draw();

      for (auto& suff : {".pdf", ".png", ".C"}) {
        auto output = filebase + suff;
        canv.SaveAs(output.data());
      }

    }

  }
}

#endif
