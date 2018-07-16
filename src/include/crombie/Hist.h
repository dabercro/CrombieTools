#ifndef CROMBIE_HIST_H
#define CROMBIE_HIST_H

// I really don't like ROOT's global hash tables and confusing lifetimes
// Here, I make a simple data structure that can make a histogram

#include <cmath>
#include <exception>
#include <vector>
#include <list>
#include <limits>
#include <tuple>

#include "crombie/Types.h"
#include "crombie/Debug.h"

#include "TH1D.h"

namespace crombie {
  namespace Hist {

    class Hist {
    public:
    /**
       Constructor of custom Hist class
       @param label Is the label to go on the x-axis
       @param nbins The number of bins to show in the plot.
              There is also one overflow and one underflow bin stored internally.
       @param min The minimum value shown on the x-axis
       @param max The maximum value on the x-axis
       @param w2 If true, store the squared sum of weights, otherwise don't bother
       @param total_events Is the total weight of events in the file(s) filling this histogram
     */
    Hist(const std::string label = "",
         unsigned nbins = 0, double min = 0, double max = 0,
         bool w2 = true, double total_events = 0)
      : label{label}, nbins{nbins}, min{min}, max{max},
        contents(nbins + 2), sumw2((nbins + 2) * w2),
        total{total_events} {}

      /// Fills this histogram with some value and weight
      void fill (double val, double weight = 1.0);
      /// Get a reference to a histogram representing an uncertainty
      Hist& get_unc_hist(const std::string& sys);
      /// Create and return a histogram inside an envelope
      Hist& get_env_hist(const std::string& sys);

      /// Get the uncertainty histogram, either up or down. Helpful for dumping datacards.
      const Hist& fetch_unc (const std::string& key, const std::string& direction) const;

      void add  (const Hist& other);
      /// Scale this histogram by a direct scale factor
      void scale (const double scale);
      /**
         Scale this histogram to a luminosity and cross section. 
         The result will be invalid if this scale function is called
         after any other call to Hist::scale.
      */
      void scale (const double lumi, const double xs);

      /// Returns a Hist that is a ratio between this and another Hist
      Hist ratio (const Hist& other) const;

      /**
         Returns a pointer to a histogram that is owned by global list.
         This list will handle the deletion when the program is done running.
         Not thread-safe.
         @param allunc If this is false, only the stat uncertainties will be included in the histogram.
      */
      TH1D* roothist (bool allunc = true) const;

      /**
         Get all of the uncertainty histograms.
         The keys are the name of the uncertainty including "Up" or "Down" at the end.
      */
      Types::map<TH1D*> unchists () const;

      /**
         Get the maximum bin and the total number of bins.
         Does not include overflow bins.
      */
      std::pair<unsigned, unsigned> get_maxbin_outof () const;

      /// Get the maximum value including uncertainties (for plotting)
      double max_w_unc () const;
      /// Get the minimum value including uncertainties, but not less than 0.0 (for plotting)
      double min_w_unc (const bool includezeros = true) const;

      /// Sets the value of the total number of events, throws exception if total is already set.
      void set_total (double newtotal);

    private:
      std::string label {};
      unsigned nbins {};
      double min {};
      double max {};

      std::vector<double> contents {};
      std::vector<double> sumw2 {};

      double total {};                                           ///< Stores the total weights of files filling this

      double get_unc (unsigned bin, bool allunc = true) const;   ///< Find the full uncertainty from uncs hists and sumw2
      void doscale (const double scale);                         ///< Scales histogram without scaling uncertainties
      void doscale (const double lumi, const double xs);         ///< Scales histogram without scaling uncertainties
      Types::map<Hist> uncs;                                     ///< Store of alternate histograms for uncertainties

      /// Apply some function to this histogram and all its uncertainties
      template<typename F> void doall (const F& func);

      using Envelope = std::tuple<Hist, Hist, std::list<Hist>>;
      mutable Types::map<Envelope> envs;                         ///< Envelope information
      mutable bool envs_set{false};                              ///< Determine whether envelope minmax has been set or not
      void set_env_min_max () const;                             ///< Sets the min and max histograms for each envelope
    };


    namespace {
      // Put this here so we don't have to move around with the Hist objects
      std::list<TH1D> histstore;
    }


    void Hist::fill(double val, double weight) {
      unsigned bin {std::min(nbins + 1, static_cast<unsigned>(std::ceil(((val - min) * nbins/(max - min)))))};
      if (bin >= contents.size()) {
        std::cerr << bin << " " << val << " " << nbins << " " << min << " " << max << std::endl;
        throw std::runtime_error{"bin too big"};
      }
      contents[bin] += weight;
      if (sumw2.size())
        sumw2[bin] += std::pow(weight, 2);
    }


    Hist& Hist::get_unc_hist(const std::string& sys) {
      uncs.insert({sys, {label, nbins, min, max, false}});
      return uncs[sys];
    }


    Hist& Hist::get_env_hist(const std::string& sys) {
      auto& mysys = std::get<2>(envs[sys]);
      mysys.push_front({label, nbins, min, max, false});
      return mysys.front();
    }


    void Hist::add(const Hist& other) {
      Debug::Debug(__PRETTY_FUNCTION__, "Start add", this, nbins);
      if (nbins == 0)
        *this = other;               // If this not set yet, just simple assignment
      else {
        total += other.total;        // Increase the total events count
        if (other.nbins != nbins) {  // Check for binning error
          std::cerr << "Num bins other: " << other.nbins << " me: " << nbins << std::endl;
          throw std::runtime_error{"Hists don't have same number of bins"};
        }

        // Add these histograms together
        for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
          contents[ibin] += other.contents[ibin];
          if (sumw2.size())
            sumw2[ibin] += other.sumw2[ibin];
        }

        // Add the up/down uncertainties
        for (auto& unc : uncs)
          unc.second.add(other.uncs.at(unc.first));
        
        // Sum the envelope histograms together
        for (auto& env : envs) {
          // If envelopes evaluted, just do those
          if (envs_set) {
            if (not other.envs_set)
              throw std::logic_error{"Trying to add one histogram with envelope set to another one without"};

            std::get<0>(env.second).add(std::get<0>(other.envs.at(env.first)));
            std::get<1>(env.second).add(std::get<1>(other.envs.at(env.first)));
          }
          else {
            // Get the other list
            const auto& otherenv = std::get<2>(other.envs.at(env.first));
            auto& meenv = std::get<2>(env.second);
            if (otherenv.size() != meenv.size()) {   // Error checking
              std::cerr << "Num envs other: " << otherenv.size() << " me: " << meenv.size() << std::endl;
              throw std::runtime_error{"Hists don't have same number of envelope histograms"};
            }
            // Loop through both lists at the same time, and add them together
            auto iother = otherenv.cbegin();
            for (auto& me : meenv) {
              me.add(*iother);
              ++iother;
            }
          }
        }
      }
    }


    void Hist::doscale(const double scale) {
      Debug::Debug(__PRETTY_FUNCTION__, "Scaling", scale);
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        contents[ibin] *= scale;
        if (sumw2.size())
          sumw2[ibin] *= std::pow(scale, 2);
      }
    }


    void Hist::doscale(const double lumi, const double xs) {
      Debug::Debug(__PRETTY_FUNCTION__, "Scaling", lumi, xs);
      doscale(lumi * xs / total);
    }


    template<typename F> void Hist::doall(const F& func) {
      func(*this);
      for (auto& unc : uncs)
        func(unc.second);

      set_env_min_max();
      for (auto& env : envs) {
        for (auto& hist : std::get<2>(env.second))
          func(hist);
        if (envs_set) {
          func(std::get<0>(env.second));
          func(std::get<1>(env.second));
        }
      }
    }


    void Hist::scale(const double scale) {
      doall([scale] (auto& hist) { hist.doscale(scale); });
    }


    void Hist::scale(const double lumi, const double xs) {
      doall([lumi, xs] (auto& hist) { hist.doscale(lumi, xs); });
    }


    TH1D* Hist::roothist(bool allunc) const {
      static unsigned plot = 0;
      auto title = std::string(";") + label + ";Events";
      Debug::Debug(__PRETTY_FUNCTION__, "Generating root hist with", title, nbins, min, max, "from", this);
      histstore.emplace_back(std::to_string(plot++).data(), title.data(), static_cast<int>(nbins), min, max);
      auto& hist = histstore.back();
      for (unsigned ibin = 0; ibin < contents.size(); ++ibin) {
        hist.SetBinContent(ibin, contents[ibin]);
        hist.SetBinError(ibin, get_unc(ibin, allunc));
      }
      Debug::Debug(__PRETTY_FUNCTION__, "hist with", hist.Integral());
      return &hist;
    }


    std::pair<unsigned, unsigned> Hist::get_maxbin_outof () const {
      double max = 0;
      unsigned maxbin = 0;
      for (unsigned ibin = 1; ibin <= nbins; ++ibin) {
        if (contents[ibin] > max) {
          max = contents[ibin];
          maxbin = ibin;
        }
      }
      return std::make_pair(maxbin, std::max(nbins, 1u));
    }


    double Hist::max_w_unc () const {
      double output = 0;
      for (unsigned ibin = 1; ibin <= nbins; ++ibin)
        output = std::max(contents[ibin] + get_unc(ibin), output);
      return output;
    }


    double Hist::min_w_unc (const bool includezeros) const {
      double output = std::numeric_limits<double>::max();
      for (unsigned ibin = 1; ibin <= nbins; ++ibin) {
        if (contents[ibin] or includezeros)
          output = std::min(contents[ibin] - get_unc(ibin), output);
      }
      return std::max(output, 0.0);
    }


    Hist Hist::ratio(const Hist& other) const {
      Debug::Debug(__PRETTY_FUNCTION__, "ratio", nbins, min, max);
      Hist output{*this};

      auto change = [&other] (Hist& tochange) {
        for (unsigned ibin = 0; ibin < other.contents.size(); ++ibin) {
          auto bincontent = other.contents.at(ibin);
          if (bincontent) {
            tochange.contents[ibin] /= other.contents.at(ibin);
            if (tochange.sumw2.size())
              tochange.sumw2[ibin] /= std::pow(other.contents.at(ibin), 2);
          }
          else {
            tochange.contents[ibin] = 1.0;
            if (tochange.sumw2.size())
              tochange.sumw2[ibin] = 0;
          }
        }
      };

      output.doall(change);

      Debug::Debug(__PRETTY_FUNCTION__, "output", output.nbins, output.min, output.max);
      return output;
    }


    void Hist::set_total (double newtotal) {
      if (total)
        throw std::logic_error{"Attempted to set total value for histogram twice. Probably a bug."};
      total = newtotal;
    }


    double Hist::get_unc(unsigned bin, bool allunc) const {
      double w2 = sumw2.size() ? sumw2.at(bin) : 0;
      if (allunc) {
        // Divide the uncertainty from each histogram by two to not double count Up and Down
        for (auto& unc : uncs)
          w2 += std::pow(contents.at(bin) - unc.second.contents.at(bin), 2)/2;

        set_env_min_max();
        // Do the same thing with the min/max envelope uncertainties
        for (auto& env : envs) {
          w2 += std::pow(contents.at(bin) - std::get<0>(env.second).contents.at(bin), 2)/2;
          w2 += std::pow(contents.at(bin) - std::get<1>(env.second).contents.at(bin), 2)/2;
        }
      }

      return std::sqrt(w2);
    }


    void Hist::set_env_min_max() const {
      // If we did this before, do nothing
      if (envs_set)
        return;

      // First time we do this
      envs_set = true;
      for (auto& env : envs) {
        // Only want a histogram without uncertianties
        auto& hmin = std::get<0>(env.second) = Hist(label, nbins, min, max, false);
        hmin.contents = contents;   // Start with current contents
        auto& hmax = std::get<1>(env.second) = hmin;
        // Loop through all the histograms in the envelope
        auto& envs_list = std::get<2>(env.second);
        for (auto& part : envs_list) {
          for (unsigned ibin = 0; ibin != contents.size(); ++ibin) {
            hmin.contents[ibin] = std::min(hmin.contents[ibin], part.contents[ibin]);
            hmax.contents[ibin] = std::max(hmax.contents[ibin], part.contents[ibin]);
          }
        }
        envs_list.clear();
      }
    }


    const Hist& Hist::fetch_unc(const std::string& key, const std::string& direction) const {
      if (direction != "Up" and direction != "Down")
        throw std::logic_error{"Direction must be 'Up' or 'Down'."};
      // Check if envelope
      if (envs.find(key) != envs.end()) {
        set_env_min_max();
        if (direction == "Up")
          return std::get<1>(envs.at(key));
        return std::get<0>(envs.at(key));
      }
      else
        return uncs.at(key + direction);
    }


    Types::map<TH1D*> Hist::unchists () const {
      Types::map<TH1D*> output;
      for (auto& unc : uncs)
        output.insert({unc.first, unc.second.roothist(false)});
      // Use the fetch_unc function to ensure that the min-max is correct
      for (auto& env : envs) {
        for (const std::string& dir : {"Up", "Down"})
          output.insert({env.first + dir, fetch_unc(env.first, dir).roothist(false)});
      }
      return output;
    }

  }
}


#endif
