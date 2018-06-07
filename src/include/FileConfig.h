#ifndef CROMBIE_CONFIGREADER_H
#define CROMBIE_CONFIGREADER_H


/**
   @file ConfigReader.h

   Defines functions for reading configuration files
*/

#include <functional>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <map>
#include <mutex>
#include <algorithm>
#include <thread>

#include <sys/stat.h>

#include "Misc.h"
#include "FileSystem.h"

namespace Crombie {
  namespace FileConfig {

    /**
       @class FileInfo
       Hold the necessary information for running over a single file.
     */
    struct FileInfo {
    FileInfo() {}
    FileInfo(const std::string dirname, const std::string filename, const std::vector<std::string>& cuts = {})
      : dirname{dirname}, name{filename}, size{FileSystem::get_size(name.data())}, cuts{cuts} {};
      std::string dirname {};
      std::string name {};
      unsigned size {};
      std::vector<std::string> cuts {};
    };


    /// The processes that a directory can be divided between
    struct Process {
    Process(const std::string treename,
            const std::string legendentry,
            const std::string cut,
            const unsigned style)
    : treename{treename}, legendentry{legendentry}, cut{cut}, style{style} {}
      const std::string treename;
      const std::string legendentry;
      const std::string cut;
      const unsigned style;
    };


    /**
       @class DirectoryInfo
       Holds information for the files to run over as well as relevant processes
     */
    class DirectoryInfo {
    public:
      /// The kinds of processes
      enum class Type {
        Data,
        Background,
        Signal
      };

      DirectoryInfo (const std::string line, const Type type, const std::vector<Process>& processes)
        : name{getname(line)}, xs{getxs(line)}, type{type}, processes{processes} {
        fillfiles();
      }

      const std::string name;  ///< The name of the directory
      const double xs;         ///< The cross section of this sample
      const Type type;         ///< The kind of process this object points to

      std::vector<FileInfo> files {};         ///< The file infos
      const std::vector<Process> processes;

    private:
      /// Helper function to extract directory name from config line and check for existence
      static std::string getname(const std::string line) {
        return line.substr(0, line.find(' '));
      }
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
    FileConfig(std::string inputdir)
      : inputdir{inputdir} {}

      template <typename M, typename R>
        auto runfiles (unsigned ncores, std::function<M(const FileInfo&)> map, R reduce);

    private:
      std::vector<DirectoryInfo> dirinfos;
      std::string inputdir;   ///< The directory containing the files

      friend std::istream& operator>>(std::istream& is, FileConfig& config);
    };

    std::istream& operator>>(std::istream& is, FileConfig& config);

    /// Reads a configuration file for file info
    FileConfig read (const char* indir, const char* config) {
      FileConfig output {indir};
      std::ifstream input {config};
      input >> output;
      return output;
    }


    // IMPLEMENTATIONS BELOW HERE //


    void DirectoryInfo::fillfiles() {
      if (FileSystem::exists(name)) {
        std::vector<std::string> cuts;
        for (auto& proc : processes)
          cuts.push_back(proc.cut);

        for (auto& file : FileSystem::list(name))
          files.push_back({name, name + "/" + file, cuts});
      }
      else {
        throw std::runtime_error{"Directory " + name + " does not exist"};
      }
    }

    std::istream& operator>>(std::istream& is, FileConfig& config) {
      DirectoryInfo::Type current_type = DirectoryInfo::Type::Data;
      std::vector<Process> current_procs {{"data_obs", "Data", "", 0}};

      std::string entry;  // Hold these things as
      std::string cut;    // we go along in case there's
      unsigned style;     // a less complete line

      bool in_dirs = false;

      for (std::string raw; std::getline(is, raw); ) {
        // Strip out comments
        std::string line {raw.substr(0, raw.find("! "))};

        if (line.size()) {
          if (line == "SIGNAL") {                                // If signal delimiter, then we set that
            current_type = DirectoryInfo::Type::Signal;
            continue;
          }

          if (line.find('{') == std::string::npos) {             // If not a line about directory info
            if (in_dirs) {                                       // Time to reset processes if new
              in_dirs = false;
              current_procs.clear();
              if (current_type == DirectoryInfo::Type::Data)
                current_type = DirectoryInfo::Type::Background;
            }
            // Read the line cuts
            auto tokens = Misc::Tokenize(line);
            // Update these values if needed
            if (tokens.size() > 1) {
              entry = tokens[1];
              cut = tokens.size() == 4 ? tokens[2] : "";
              style = std::stoi(tokens.back());
            }
            current_procs.push_back({tokens[0], entry, cut, style});
          }
          else { // Otherwise add a DirectoryInfo
            in_dirs = true;
            config.dirinfos.push_back({config.inputdir + "/" + line, current_type, current_procs});
          }
        }
      }
      return is;
    }

    // Use this to set priority for files
    bool operator<(const FileInfo& a, const FileInfo& b) {
      return a.size < b.size;
    }

    template <typename M, typename R>
      auto FileConfig::runfiles (unsigned ncores, std::function<M(const FileInfo&)> map, R reduce) {

      std::map<std::string, std::vector<M>> outputs; // This is fed into reduce, in addition to the directory infos
      std::priority_queue<FileInfo> queue;
      for (const auto& dirinfo : dirinfos) {
        outputs[dirinfo.name].reserve(dirinfo.files.size());
        for (const auto& fileinfo : dirinfo.files)
          queue.push(fileinfo);
      }

      // Launch threads
      std::mutex inlock;
      std::mutex outlock;

      std::vector<std::thread> threads;
      ncores = std::min(ncores, 1u);

      for (unsigned i = 0; i < ncores; ++i) {
        threads.push_back(std::thread([&] () {
              bool running = true;
              while(true) {
                FileInfo info;
                inlock.lock();
                running = !queue.empty();
                if (running) {
                  info = queue.top();
                  queue.pop();
                }
                inlock.unlock();
                if (not running)
                  break;
                auto fileoutput = map(info);
                outlock.lock();
                outputs.at(info.dirname).push_back(fileoutput);
                outlock.unlock();
              }
            })
          );
      }

      for (auto& thread : threads)
        thread.join();

      return reduce(dirinfos, outputs);
    }

  }
}


#endif
