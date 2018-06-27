#ifndef CROMBIE_PLOTTER_H
#define CROMBIE_PLOTTER_H

#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <mutex>

#include "crombie/Types.h"
#include "crombie/Selection.h"
#include "crombie/PlotConfig.h"
#include "crombie/FileConfig.h"
#include "crombie/LoadTree.h"
#include "crombie/Hist.h"

#include "TH1.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TLatex.h"

namespace crombie {
  namespace Plotter {

    /**
       This is the output running over a single file.
       The first number is the number of events for cross section normalization.
       The key corresponds to a "selection_plotname", and the different hists are different process cuts.
    */
    using SingleOut = std::pair<double, Types::map<std::vector<Hist::Hist>>>;

    /// Constructs a function that runs over a single file and produces all the necessary histograms
    FileConfig::MapFunc<SingleOut>
      SingleFile (const std::vector<PlotConfig::Plot>& plots,
                  const Selection::SelectionConfig& selections);

    /// This class has everything needed to draw a plot
    class Plot {
    public:
      /// One of the messier functions which draws the plot at the desired location
      void  draw  (const std::string& filebase);
      void  dumpdatacard (const std::string& filename);
      /// Add a plot to the inner store. Processes are merged together
      void  add (unsigned isub, const FileConfig::DirectoryInfo& info, const Hist::Hist& hist, double mcweight);
      /// Scale the MC plots to match the given luminosity
      void  scale (double lumi);
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
      Types::map<Types::map<PlotInfo>> plotstore {};
      /// The last luminosity scaled to. 0 if not set yet.
      double currentlumi {0.0};
    };

    /// The key is a combination of "selection_plotname"
    using MergeOut = Types::map<Plot>;
    /// The FileConfig::MergeFunc instantiation for this namespace
    using MergeFunc = FileConfig::MergeFunc<MergeOut, SingleOut>;

    /**
       Gets a function that merges the output of the SingleFile functional
    */
    MergeFunc Merge (const FileConfig::FileConfig& files);


    // IMPLEMENTATIONS BELOW HERE //


    namespace {

      class CutReader {
      public:
      CutReader(double& cut, double& expr, double& weight, double& sub, Hist::Hist& hist)
        : cut{cut}, expr{expr}, weight{weight}, sub{sub}, hist{hist} {
          Debug::Debug(__PRETTY_FUNCTION__, "Reading for hist at", &hist);
        }

        void eval () {
          if (cut and sub)
            hist.fill(expr, weight * sub);
        }
      private:
        double& cut;    // For the selection
        double& expr;
        double& weight;
        double& sub;    // For the subprocess
        Hist::Hist& hist;
      };

    }

    FileConfig::MapFunc<SingleOut>
      SingleFile (const std::vector<PlotConfig::Plot>& plots,
                  const Selection::SelectionConfig& selections) {
      return FileConfig::MapFunc<SingleOut> {
        [&plots, &selections] (const FileConfig::FileInfo& info) {
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

          // Cover bases here
          Types::strings nminus1;
          for (auto& expr : exprs) {
            for (auto& cut : cuts) {
              if (cut.find(expr) != std::string::npos)
                nminus1.push_back(Selection::nminus1(expr, cut));
            }
          }

          LoadTree::Tree loaded{info.name, exprs, weights, cuts, info.cuts, nminus1};

          SingleOut output {
            loaded.get<TH1>(selections.mchistname)->GetBinContent(1),
            {}
          };

          Debug::Debug(__PRETTY_FUNCTION__, "MC hist contents:", output.first);

          std::list<CutReader> readers {};

          for (auto& sel : selections.selections) {
            for (auto& plot : plots) {
              auto& plotvec = output.second[sel.first + "_" + plot.name];
              // We want to reserve the location for the vector because we want the references to stay valid
              plotvec.reserve(info.cuts.size());
              for (auto& sub : info.cuts) {
                plotvec.push_back(plot.get_hist());
                auto cut = get_cut(sel);
                auto expr = get_expr(plot);
                readers.emplace_back(loaded.result(Selection::nminus1(expr, cut)),
                                     loaded.result(expr),
                                     loaded.result(get_weight(sel)),
                                     loaded.result(sub),
                                     plotvec.back());
              }
            }
          }

          while (loaded.next()) {
            for (auto& reader : readers)
              reader.eval();
          }

          return output;
        }
      };
    }

    MergeFunc Merge(const FileConfig::FileConfig& files) {
      // Put lumi search here so that the "missing lumi" error is thrown early
      double lumi = files.has_mc() ? std::stod(Misc::env("lumi")) : 0.0;
      return MergeFunc { [&files, lumi] (auto& outputs) {
          MergeOut output {};
          for (auto& dir : files.get_dirs()) {
            // Each of plots is a SingleOut
            for (auto& plots : outputs.at(dir.name)) {
              // Key of "plot" is key of "output", value of "plot" is list of histograms for processes
              for (auto& plot : plots.second) {
                auto& outputplot = output[plot.first];
                for (unsigned iproc = 0; iproc != plot.second.size(); ++iproc)
                  outputplot.add(iproc, dir, plot.second.at(iproc), plots.first);
              }
            }
          }
          for (auto& plot : output)
            plot.second.scale(lumi);
          return output;
        }
      };
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
      TH1* style(TH1* hist, FileConfig::Type type, short style) {
        hist->SetLineColor(kBlack);
        switch(type) {
        case(FileConfig::Type::Data) :
          Debug::Debug(__PRETTY_FUNCTION__, "New data hist");
          hist->SetMarkerStyle(8);
          hist->SetMarkerColor(style);
          hist->SetLineColor(style);
          break;
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
        return hist; // Return for chaining
      }

      TLegend legend(const Hist::Hist& hist, unsigned numlabels) {
        auto bins = hist.get_maxbin_outof();
        // The upper left corner of the legend;
        double x_left = ((bins.first * 2)/bins.second) ? 0.15 : 0.65;

        Debug::Debug(__PRETTY_FUNCTION__, "max bin", bins.first, bins.second, (bins.first * 2)/bins.second, x_left);

        // Height determined by number of anticipated legend entries
        TLegend leg{x_left, 0.875 - std::min(0.5, 0.075 * numlabels), x_left + 0.25, 0.875};
        leg.SetBorderSize(0);
        leg.SetFillStyle(0);
        return leg;
      }

    }

    void Plot::scale(double lumi) {
      for (auto& dir : plotstore) {
        for (auto& proc : dir.second) {
          if (proc.second.type != FileConfig::Type::Data) {
            // Need to do a quick check that scale hasn't been called before
            double scale = currentlumi ? lumi/currentlumi : proc.second.xsec*lumi/proc.second.mcweight;
            proc.second.hist.scale(scale);
          }
        }
      }
      currentlumi = lumi;
    }

    void Plot::draw(const std::string& filebase) {

      // Legend label is the key of the map
      std::map<FileConfig::Type, Types::map<TH1D*>> hists;
      // Use this to store sums for ratios
      Hist::Hist bkg_hist {};
      // Both final style and histogram
      using StyledHist = std::pair<short, Hist::Hist>;
      StyledHist data_hist {};
      StyledHist signal_hist {};
      for (auto& dir : plotstore) {
        for (auto& proc : dir.second) {
          // Scale the histogram
          switch(proc.second.type) {
          case(FileConfig::Type::Data):
            data_hist.first = proc.second.style;
            data_hist.second.add(proc.second.hist);
            break;
          case(FileConfig::Type::Background):
            bkg_hist.add(proc.second.hist);
            // Add backgrounds to signal sums too
          case(FileConfig::Type::Signal):
            signal_hist.first = proc.second.style;
            signal_hist.second.add(proc.second.hist);
          }
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
            Debug::Debug(__PRETTY_FUNCTION__, a.second->Integral());
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

      unsigned numlabels = 0;
      for (auto& type : hists)
        numlabels += type.second.size();

      // Make the legend, determining location summed histograms
      TLegend leg{legend(bkg_hist, numlabels)};

      // MC entries
      for (auto& mc : mcvec)
        leg.AddEntry(mc.second, mc.first.data(), "f");

      // Draw everything
      TCanvas canv{"canv", "canv", 600, 700};
      canv.cd();
      // Top pad
      const double bottom = mcvec.size() ? 0.3 : 0.0;

      TPad pad1{"pad1", "pad1", 0.0, bottom, 1.0, 1.0};
      Debug::Debug(__PRETTY_FUNCTION__, "Pad number", pad1.GetNumber(), pad1.GetMother(), &canv);
      pad1.SetBottomMargin(bottom ? 0.025 : 0.1);
      pad1.Draw();
      pad1.cd();

      const double nomfont = 0.03;          // Target font size for plot labels
      const double titleoff = 1.55;         // Title offset

      if (mcvec.size()) {
        hs.Draw("hist");

        hs.GetYaxis()->SetLabelSize(nomfont/(1 - bottom));
        hs.GetYaxis()->SetTitleSize(nomfont/(1 - bottom));
        hs.GetYaxis()->SetTitleOffset(titleoff);
        hs.GetYaxis()->SetTitle("Events/Bin");

        hs.GetXaxis()->SetLabelSize(0);
        hs.GetXaxis()->SetTitleSize(0);

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
        Debug::Debug(__PRETTY_FUNCTION__, "Drawing data hist with", data.second->Integral(), "entries");
        leg.AddEntry(data.second, data.first.data(), "lp");
        data.second->Draw("PE,same");
      }

      canv.cd();
      TPad pad2{"pad2", "pad2", 0.0, 0.0, 1.0, bottom};
      Debug::Debug(__PRETTY_FUNCTION__, "Pad number", pad2.GetNumber(), pad2.GetMother(), &canv);

      if (bottom) {
        Debug::Debug(__PRETTY_FUNCTION__, "Making bottom pad");
        pad2.SetTopMargin(0.025);
        pad2.SetBottomMargin(0.4);
        pad2.cd();

        auto bkg_ratio = bkg_hist.ratio(bkg_hist);
        auto data_ratio = data_hist.second.ratio(bkg_hist);

        auto set_yaxis = [bottom, titleoff] (auto* hist) {
          auto* axis = hist->GetYaxis();
          axis->SetNdivisions(504, true);
          axis->SetTitle("Data/Pred.");
          axis->SetTitleOffset((bottom)/(1-bottom) * titleoff);
          axis->CenterTitle();
          return hist;
        };

        auto* bhist = set_yaxis(bkg_ratio.roothist());

        for (auto* axis : {bhist->GetXaxis(), bhist->GetYaxis()}) {
          axis->SetTitleSize(nomfont/bottom);
          axis->SetLabelSize(nomfont/bottom);
        }

        bhist->SetFillStyle(3001);
        bhist->SetFillColor(kGray);
        bhist->SetMinimum(std::min(bkg_ratio.min_w_unc(), data_ratio.min_w_unc()));
        bhist->SetMaximum(std::max(bkg_ratio.max_w_unc(), data_ratio.max_w_unc()));
        bhist->Draw("e2");

        style(signal_hist.second.ratio(bkg_hist).roothist(), FileConfig::Type::Signal, signal_hist.first)->Draw("hist,same");
        style(data_ratio.roothist(), FileConfig::Type::Data, data_hist.first)->Draw("PE,same");

        pad2.SetGridy(1);

        canv.cd();
        pad2.Draw();
      }

      pad1.cd();
      leg.Draw();

      canv.cd();
      // Labels
      TLatex latex{};
      constexpr double toplocation = 0.96;
      latex.SetTextSize(0.035);
      if (currentlumi) {
        latex.SetTextAlign(31);

        std::stringstream lumistream;
        lumistream << std::setprecision(3) << currentlumi/1000.0;
        std::string lumilabel;
        lumistream >> lumilabel;
        lumilabel += " fb^{-1} (13 TeV)";

        latex.DrawLatex(0.95, toplocation, lumilabel.data());
      }
      latex.SetTextFont(62);
      latex.SetTextAlign(11);
      latex.DrawLatex(0.12, toplocation, "CMS");
      latex.SetTextSize(0.030);
      latex.SetTextFont(52);
      latex.SetTextAlign(11);

      latex.DrawLatex(0.2, toplocation, "Preliminary");

      // Save everything
      for (auto& suff : {".pdf", ".png", ".C"}) {
        auto output = filebase + suff;
        canv.SaveAs(output.data());
      }

    }

    void  Plot::dumpdatacard(const std::string& filename) {

    }

  }
}

#endif
