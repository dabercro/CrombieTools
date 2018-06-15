#ifndef CROMBIE_PLOTTER_H
#define CROMBIE_PLOTTER_H

#include <string>
#include <vector>
#include <map>
#include <list>
#include <mutex>

#include "crombie/Selection.h"
#include "crombie/PlotConfig.h"
#include "crombie/FileConfig.h"
#include "crombie/LoadTree.h"
#include "crombie/Hist.h"

#include "TFile.h"
#include "TH1.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"

namespace crombie {
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
      /**
         Draws the output file asked for with a given lumi
      */
      void  draw  (const std::string& filebase);
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

        void eval () {
          if (cut and sub)
            hist.fill(expr, weight);
        }
      private:
        double& cut;    // For the selection
        double& expr;
        double& weight;
        double& sub;    // For the subprocess
        Hist::Hist& hist;
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

    namespace {
      // Stylize a histogram based on its type
      void style(TH1* hist, FileConfig::Type type, short style) {
        hist->SetLineColor(kBlack);
        switch(type) {
        case(FileConfig::Type::Data) :
          Debug::Debug("New data hist");
          hist->SetMarkerStyle(8);
          return;
        case(FileConfig::Type::Signal) :
          hist->SetLineStyle(style);
          break;
        case(FileConfig::Type::Background) :
          hist->SetFillStyle(1001);
          hist->SetFillColor(style);
          break;
        default: // Don't know what you would want to do here
          throw;
        }
        hist->SetLineWidth(2);
      }

      TLegend legend(const Hist::Hist& hist) {
        auto bins = hist.get_maxbin_outof();
        // The upper left corner of the legend;
        double x_left = ((bins.first * 2)/bins.second) ? 0.15 : 0.65;

        Debug::Debug("max bin", bins.first, bins.second, (bins.first * 2)/bins.second, x_left);

        TLegend leg{x_left, 0.4, x_left + 0.25, 0.9};
        leg.SetBorderSize(0);
        leg.SetFillStyle(0);
        return leg;
      }

    }

    namespace {
      // Want this for the different plotting things
      double lumi = std::stod(Misc::env("lumi"));
    }

    void Plot::draw(const std::string& filebase) {

      // Legend label is the key of the map
      std::map<FileConfig::Type, std::map<std::string, TH1D*>> hists;
      // Use this to store sums
      Hist::Hist bkg_hist {};
      for (auto& dir : plotstore) {
        for (auto& proc : dir.second) {
          // Scale the histogram
          if (proc.second.type != FileConfig::Type::Data)
            proc.second.hist.scale(proc.second.xsec * lumi / proc.second.mcweight);

          if (proc.second.type == FileConfig::Type::Background)
            bkg_hist.add(proc.second.hist);
          auto* newhist = proc.second.hist.roothist();
          auto& histcol = hists[proc.second.type];
          if (histcol.find(proc.second.entry) != histcol.end())
            histcol[proc.second.entry]->Add(newhist);
          else {
            // Styling for each type
            style(newhist, proc.second.type, proc.second.style);
            histcol[proc.second.entry] = newhist;
          }
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
      // Get the maximum value
      auto max = bkg_hist.max_w_unc();
      // Check the data histogram(s)
      for(auto& dat : hists[FileConfig::Type::Data])
        max = std::max(dat.second->GetMaximum(), max); // Doesn't include the uncertainties

      hs.SetMaximum(max);
      // Need to add to stack in reverse order
      auto mc = mcvec.end();
      while(mc != mcvec.begin())
        hs.Add((--mc)->second);

      // Make the legend, determining location summed histograms
      TLegend leg{legend(bkg_hist)};

      // MC entries
      for (auto& mc : mcvec)
        leg.AddEntry(mc.second, mc.first.data(), "f");

      // Draw everything
      TCanvas canv{"canv", "canv"};
      canv.cd();
      if (mcvec.size()) {
        hs.Draw("hist");
        auto* bkg_sum = bkg_hist.roothist();
        bkg_sum->SetFillStyle(3001);
        bkg_sum->SetFillColor(kGray);
        bkg_sum->Draw("e2,same");

        for (auto& sig : sigvec) {
          leg.AddEntry(sig.second, sig.first.data(), "lp");
          sig.second->Add(bkg_sum);
          sig.second->Draw("hist,same");
        }
      }

      for (auto& data : hists[FileConfig::Type::Data]) {
        Debug::Debug("Drawing data hist with", data.second->Integral(), "entries");
        leg.AddEntry(data.second, data.first.data(), "lp");
        data.second->Draw("PE,same");
      }

      leg.Draw();

      // Save everything
      for (auto& suff : {".pdf", ".png", ".C"}) {
        auto output = filebase + suff;
        canv.SaveAs(output.data());
      }

    }

  }
}

#endif
