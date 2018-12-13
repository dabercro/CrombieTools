#ifndef CROMBIE_CONFIGREADER_H
#define CROMBIE_CONFIGREADER_H


/**
   @file ConfigReader.h

   Defines functions for reading configuration files
*/

#include <unistd.h>

#include <iostream>
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

#include "crombie/Types.h"
#include "crombie/Misc.h"
#include "crombie/Parse.h"
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
    /// Default constructor
    FileInfo() {}
    /// Set values for everything in the structure
    FileInfo(const Type type, const std::string& dirname, const std::string& filename, const Types::strings& cuts = {"1.0"})
    : type{type}, dirname{dirname}, name{filename}, size{FileSystem::get_size(name.data())}, cuts{cuts} {
      Debug::Debug(__PRETTY_FUNCTION__, dirname, filename, size, cuts.size());
    }
      Type type {};            ///< Type of process this file is
      std::string dirname {};  ///< Directory this file is in
      std::string name {};     ///< The full path to the file
      unsigned long size {};   ///< The size of the file. Used for priority
      Types::strings cuts {};  ///< Cuts that split this file into different processes or legend entries
    };


    /// The processes that a directory can be divided between
    struct Process {
    /**
       @param treename The name that this process will have in datacards
       @param entry The entry in plot legends for this process. `"_"` is replaced with `" "`.
       @param cut Is the cut applied to the file to create this process
       @param style Some style number that is used to make plots
     */
    Process(const std::string& treename,
            const std::string& entry,
            const std::string& cut,
            const short style)
    : treename{treename}, legendentry{entry}, cut{cut}, style{style} {
      std::replace(legendentry.begin(), legendentry.end(), '_', ' ');
    }
      const std::string& treename;
      std::string legendentry;
      const std::string& cut;
      const short style;
    };


    /**
       @class DirectoryInfo
       Holds information for the files to run over as well as relevant processes
     */
    class DirectoryInfo {
    public:

      DirectoryInfo (const std::string& line, const Type type, const std::vector<Process>& processes)
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
      static std::string getname(const std::string& line);

      /// Helper function to extract cross section
      static double getxs(const std::string& line) {
        auto begin = line.find('{') + 1;
        auto val = line.substr(begin, line.find('}') - begin);
        return val.size() ? std::stod(val) : 0;
      }

      /// Fill the file info for this object
      void fillfiles();
    };

    // These types are used for runfiles

    /**
       The type that is used for the mapping function
    */
    template<typename M> using MapFunc = std::function<M(const FileInfo&)>;

    /**
       The parameter passed to the FileConfig::runfiles reduce function
       @param M is the type given by a SingleOut mapping function
    */
    template<typename M> using ToMerge = Types::map<std::list<M>>;

    /**
       A functional type that is not necessary, but might be a useful shortcut
       @param R The output of the reduction formula
       @param M The output of the map formula
     */
    template<typename R, typename M> using MergeFunc = std::function<R(const ToMerge<M>&)>;


    /**
       @class FileConfig
       A single object that holds all of the information about the files to run on.
       This is the result of reading a single file configuration file.
     */
    class FileConfig {
    public:
      FileConfig(const std::string& inputdir, const bool onedir = true);

      /**
         This will perform a map-reduce operation on all of the files in this configuration.
         Takes two functions. One to map files to outputs and one to reduce these outputs.
         @param M The type of output of the map function
         @param R The type of reduce
         @param map Is a function that takes a FileInfo as input, and outputs some object of type M
         @param reduce is fed ToMerge<M> as input. Whatever reduce returns is returned by runfiles.
       */
      template <typename M, typename R>
        auto runfiles (MapFunc<M> map, R reduce);

      /// Read the directory infos
      const std::vector<DirectoryInfo>& get_dirs () const { return dirinfos; }

      /// Read a single directory info
      const DirectoryInfo& get_dir (const std::string& dirname) const;

      /// Say if this object has MC directories stored in it
      const bool has_mc () const { return _has_mc; }
      /// Say if this object has data directories stored in it
      const bool has_data () const { return _has_data; }

    private:
      std::vector<DirectoryInfo> dirinfos;   ///< Internal store of DirectoryInfo objects
      const std::string inputdir;            ///< The directory containing the files

      bool _has_mc {false};                  ///< Tracks when MC files are stored
      bool _has_data {false};                ///< Tracks when data files are stored

      friend std::istream& operator>>(std::istream& is, FileConfig& config);
    };

    std::istream& operator>>(std::istream& is, FileConfig& config);

    /// Reads a configuration file for file info
    FileConfig read(const std::string& indir, const std::string& config) {
      FileConfig output {indir, false};
      std::ifstream input {config};
      input >> output;
      return output;
    }


    // IMPLEMENTATIONS BELOW HERE //

    FileConfig::FileConfig(const std::string& inputdir, const bool onedir)
      : inputdir{onedir ? "" : dirclean(inputdir)} {
      if (onedir) {
        std::stringstream input {inputdir + " {}"};
        input >> *this;
      }
    }


    const DirectoryInfo& FileConfig::get_dir(const std::string& dirname) const {
      for (auto& dir : dirinfos)
        if (dir.name == dirname)
          return dir;

      throw std::logic_error{dirname + " does not seem to be a valid directory name"};
    }


    std::string DirectoryInfo::getname(const std::string& line) {
      std::string dir {line.substr(0, line.find(' '))};
      // Just return the file name if a .root file.
      return (dir.substr(dir.size() - 5, 5) == ".root") ? dir : dirclean(dir);
    }


    void DirectoryInfo::fillfiles() {
      if (FileSystem::exists(name)) {
        Types::strings cuts;
        for (auto& proc : processes)
          cuts.push_back(proc.cut);

        if (name.back() != '/')  // If the directory info is actually a file, just push back one file
          files.push_back({type, name, name, cuts});
        else {                   // Otherwise, push back all of the files
          for (auto& file : FileSystem::list(name)) {
            // Only loading .root files
            if (file.find(".root") != std::string::npos)
              files.push_back({type, name, name + file, cuts});
          }
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

      for (auto& line : Parse::parse(is)) {
        // Set the default type, if needed
        const Types::map<Type> default_lines {
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
      return is;
    }


    /// Compares FileInfo objects based on the size of the files; used for queue priority
    bool operator<(const FileInfo& a, const FileInfo& b) {
      return a.size < b.size;
    }


    template <typename M, typename R>
      auto FileConfig::runfiles (MapFunc<M> map, R reduce) {
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

      auto total = queue.size();
      auto divisor = total/100 + 1;
      auto progress = total/divisor;

      std::cout << "Using " << nthreads << " threads to run over " << total << " files" << std::endl;

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
                    Misc::draw_progress(std::min(done/divisor, progress - 1), progress);
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
