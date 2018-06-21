#ifndef CROMBIE_CONFIGREADER_H
#define CROMBIE_CONFIGREADER_H


/**
   @file ConfigReader.h

   Defines functions for reading configuration files
*/

#include <unistd.h>

#include <functional>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <map>
#include <mutex>
#include <algorithm>
#include <thread>
#include <chrono>
#include <list>

#include <sys/stat.h>

#include "crombie/Misc.h"
#include "crombie/FileSystem.h"

namespace crombie {
  namespace FileConfig {

    namespace {
      std::string dirclean (const std::string& dir) {
        if (dir.size() and dir.back() != '/')
          return dir + "/";
        return dir;
      }
    }

    /// The kinds of processes
    enum class Type {
      Data,
      Background,
      Signal
    };

    /**
       @class FileInfo
       Hold the necessary information for running over a single file.
     */
    struct FileInfo {
    FileInfo() {}
    FileInfo(const Type type, const std::string dirname, const std::string filename, const std::vector<std::string>& cuts = {"1.0"})
    : type{type}, dirname{dirname}, name{filename}, size{FileSystem::get_size(name.data())}, cuts{cuts} {
      Debug::Debug(__PRETTY_FUNCTION__, dirname, filename, size, cuts.size());
    }
      Type type {};
      std::string dirname {};
      std::string name {};
      unsigned size {};
      std::vector<std::string> cuts {};
    };


    /// The processes that a directory can be divided between
    struct Process {
    Process(const std::string treename,
            const std::string entry,
            const std::string cut,
            const short style)
    : treename{treename}, legendentry{entry}, cut{cut}, style{style} {
      std::replace(legendentry.begin(), legendentry.end(), '_', ' ');
    }
      const std::string treename;
      std::string legendentry;
      const std::string cut;
      const short style;
    };


    /**
       @class DirectoryInfo
       Holds information for the files to run over as well as relevant processes
     */
    class DirectoryInfo {
    public:

      DirectoryInfo (const std::string line, const Type type, const std::vector<Process>& processes)
        : name{getname(line)}, xs{getxs(line)}, type{type}, processes{processes} {
        Debug::Debug(__PRETTY_FUNCTION__, line, name, xs, processes.size());
        fillfiles();
      }

      const std::string name;  ///< The name of the directory
      const double xs;         ///< The cross section of this sample
      const Type type;         ///< The kind of process this object points to

      std::vector<FileInfo> files {};         ///< The file infos
      const std::vector<Process> processes;

    private:
      /**
         Helper function to extract directory name from config line.
         If the config line is actually a file, then it returns the unchanged file name.
      */
      static std::string getname(const std::string line);

      /// Helper function to extract cross section
      static double getxs(const std::string line) {
        auto begin = line.find('{') + 1;
        auto val = line.substr(begin, line.find('}') - begin);
        return val.size() ? std::stod(val) : 0;
      }

      /// Fill the file info for this object
      void fillfiles();
    };


    class FileConfig {
    public:
    FileConfig(const std::string& inputdir)
      : inputdir{dirclean(inputdir)} {}

      template <typename M, typename R>
        auto runfiles (std::function<M(const FileInfo&)> map, R reduce);

      /// Read the directory infos
      const std::vector<DirectoryInfo>& get_dirs () const { return dirinfos; }

      const bool has_mc () const { return _has_mc; }
      const bool has_data () const { return _has_data; }

    private:
      std::vector<DirectoryInfo> dirinfos;
      const std::string inputdir;   ///< The directory containing the files

      bool _has_mc {false};
      bool _has_data {false};

      friend std::istream& operator>>(std::istream& is, FileConfig& config);
    };

    std::istream& operator>>(std::istream& is, FileConfig& config);

    /// Reads a configuration file for file info
    FileConfig read(const char* indir, const char* config) {
      FileConfig output {indir};
      std::ifstream input {config};
      input >> output;
      return output;
    }


    // IMPLEMENTATIONS BELOW HERE //


    std::string DirectoryInfo::getname(const std::string line) {
      std::string dir {line.substr(0, line.find(' '))};
      // Just return the file name if a .root file.
      return (dir.substr(dir.size() - 5, 5) == ".root") ? dir : dirclean(dir);
    }

    void DirectoryInfo::fillfiles() {
      if (FileSystem::exists(name)) {
        std::vector<std::string> cuts;
        for (auto& proc : processes)
          cuts.push_back(proc.cut);

        if (name.back() != '/')  // If the directory info is actually a file, just push back one file
          files.push_back({type, name, name, cuts});
        else {                   // Otherwise, push back all of the files
          for (auto& file : FileSystem::list(name))
            files.push_back({type, name, name + file, cuts});
        }

      }
      else {
        throw std::runtime_error{"Path " + name + " does not exist"};
      }
    }

    std::istream& operator>>(std::istream& is, FileConfig& config) {
      Type current_type = Type::Data;
      std::vector<Process> current_procs {{"data_obs", "Data", "1.0", 1}};

      // This is the type we change to when we see process lines
      Type default_type = Type::Background;

      std::string entry;  // Hold these things as
      std::string cut;    // we go along in case there's
      short style;        // a less complete line

      bool in_dirs = true;

      for (std::string raw; std::getline(is, raw); ) {
        // Strip out comments
        std::string line {raw.substr(0, raw.find("! "))};

        if (line.size()) {
          Debug::Debug(__PRETTY_FUNCTION__, line);
          // Set the default type, if needed
          const std::map<std::string, Type> default_lines {
            {"DATA", Type::Data},
            {"SIGNAL", Type::Signal},
            {"MC", Type::Background}
          };
          if (default_lines.find(line) != default_lines.end()) {
            default_type = default_lines.at(line);
            continue;
          }

          if (line.find('{') == std::string::npos) {             // If not a line about directory info
            if (in_dirs) {                                       // Time to reset processes if new
              in_dirs = false;
              current_procs.clear();
              current_type = default_type;
            }
            // Read the line cuts
            auto tokens = Misc::tokenize(line);
            // Update these values if needed
            if (tokens.size() > 1) {
              entry = tokens[1];
              cut = tokens.size() == 4 ? tokens[2] : "1.0";
              style = std::stoi(tokens.back());
            }
            current_procs.push_back({tokens[0], entry, cut, style});
          }
          else { // Otherwise add a DirectoryInfo
            if (current_type != Type::Data)
              config._has_mc = true;
            else
              config._has_data = true;
            in_dirs = true;
            config.dirinfos.push_back({config.inputdir + line, current_type, current_procs});
          }
        }
      }
      return is;
    }

    // Use this to set priority for files
    bool operator<(const FileInfo& a, const FileInfo& b) {
      return a.size < b.size;
    }

    /// The parameter passed to the FileConfig::runfiles reduce function
    template<typename M>
      using ToMerge = std::map<std::string, std::list<M>>;

    template <typename M, typename R>
      auto FileConfig::runfiles (std::function<M(const FileInfo&)> map, R reduce) {
      unsigned nthreads = std::stoi(Misc::env("nthreads", "1"));
      ToMerge<M> outputs; // This is fed into reduce, in addition to the directory infos
      std::priority_queue<FileInfo> queue;
      for (const auto& dirinfo : dirinfos) {
        for (const auto& fileinfo : dirinfo.files)
          queue.push(fileinfo);
      }

      // Launch threads
      std::mutex inlock;
      std::mutex outlock;

      std::vector<std::thread> threads;
      nthreads = std::max(nthreads, 1u);

      std::cout << "Using " << nthreads << " threads" << std::endl;

      auto total = queue.size();
      auto divisor = total/100 + 1;
      auto progress = total/divisor;
      Misc::draw_progress(0, progress);

      std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < nthreads; ++i) {
        threads.push_back(std::thread([&] () {
              bool running = true;
              while(true) {
                Debug::Debug(__PRETTY_FUNCTION__, "Start thread loop");
                FileInfo info;
                inlock.lock();
                running = !queue.empty();
                if (running) {
                  info = queue.top();
                  auto done = total - queue.size();
                  if (done % divisor == 0) {
                    Misc::draw_progress(done/divisor, progress);
                  }
                  queue.pop();
                }
                inlock.unlock();
                if (not running)
                  break;
                auto fileoutput = map(info);
                outlock.lock();
                outputs[info.dirname].push_back(std::move(fileoutput));
                outlock.unlock();
              }
            })
          );
      }

      for (auto& thread : threads)
        thread.join();

      Misc::draw_progress(progress, progress);
      std::cout << std::endl; // New line after progress bar

      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

      std::cout << "Ran over " << total << " files in "
                << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
                << " seconds" << std::endl;

      return reduce(outputs);
    }

  }
}


#endif
