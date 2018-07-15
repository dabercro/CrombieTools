#ifndef CROMBIE_FILESYSTEM_H
#define CROMBIE_FILESYSTEM_H

#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>

#include "crombie/Types.h"
#include "crombie/Misc.h"

namespace crombie {
  namespace FileSystem {

    std::string basename(const std::string& name) {
      auto output = Misc::split(name, "/").back();
      Debug::Debug(__PRETTY_FUNCTION__, name, output);
      return output;
    }

    std::string dirname(const std::string& name) {
      std::string modname = name;
      while (modname.back() == '/')
        modname.pop_back();

      auto output = modname.substr(0, modname.size() - basename(modname).size());
      Debug::Debug(__PRETTY_FUNCTION__, name, output);
      return output;
    }

    namespace {
      // Is the path for xrd
      bool is_xrd(const std::string& path) {
        return path.find("root://") == 0;
      }

      // Do the xrdfs call
      Types::strings xrd_ls(const std::string& path) {
        // Cache for listed directories to prevent too many calls
        static Types::map<Types::strings> xrd_dir_contents;

        auto iter = xrd_dir_contents.find(path);
        if (iter != xrd_dir_contents.end())
          return iter->second;

        // Door will be between two "//" sequences when user is sane
        auto parts = Misc::split(path, "//");
        if (parts.size() != 3)
          throw std::runtime_error{std::string("xrd path (") + path + ") doesn't seem to have a good pattern"};

        // Set the cache to the split long listing of xrdfs and return it
        return xrd_dir_contents[path] = 
          Misc::split(Misc::shell(std::string("xrdfs root://") + parts[1] + "/ ls -l /" + parts[2]));
      }

      // Get the xrd_ls call and return the file portion of the line
      Types::strings xrd_list(const std::string& path) {
        return Misc::comprehension<std::string>
          (xrd_ls(path),
           [] (auto& line) {
            return basename(Misc::tokenize(line)[4]);
          });
      }

      // Get the listing of the directory, and then extract the size
      unsigned long xrd_get_size(const std::string& name) {
        // There is some tricky business about the "//" in ls outputs, so just look for the file
        std::string filename = basename(name);
        for (auto& line : xrd_ls(dirname(name))) {
          if (line.find(filename) != std::string::npos)
            return std::stoul(Misc::tokenize(line)[3]);
        }
        return 0;
      }

      // Hacky way to see if path is there
      bool xrd_exists(const std::string& path) {
        return xrd_get_size(path);
      }

    }

    /// Check if path exists (either directory or file)
    bool exists(const std::string& path) {
      if (is_xrd(path))
        return xrd_exists(path);
      struct stat buffer;
      return stat(path.data(), &buffer) == 0;
    }

    /// Get the size of a file
    unsigned long get_size(const std::string name) {
      if (is_xrd(name))
        return xrd_get_size(name);
      struct stat file_stat;
      stat(name.data(), &file_stat);
      return file_stat.st_size;
    }

    /// Create directories, recursively if needed
    void mkdirs(const std::string path) {
      char path_array[512];
      // Stick a slash on the end to trick our character flipping
      auto addslash = path;
      if (addslash.back() != '/')
        addslash += '/';

      strncpy(path_array, addslash.data(), sizeof(path_array) - 1);
      auto num_chars = strlen(path_array);

      for (unsigned i_char = 1; i_char < num_chars; i_char++) {

        if (path_array[i_char] != '/')
          continue;

        // Flip character to null
        path_array[i_char] = '\0';
        if (!exists(path_array)) {
          std::cout << "Making: " << path_array << std::endl;
          mkdir(path_array, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        }
        // Flip back
        path_array[i_char] = '/';
      }
    }

    /// The name of files inside of the directory
    Types::strings list(std::string directory) {
      if (is_xrd(directory))
        return xrd_list(directory);

      Types::strings output;

      auto* indir = opendir(directory.data());
      while (auto* dir_ent = readdir(indir)) {
        if (dir_ent->d_name[0] != '.')
          output.emplace_back(dir_ent->d_name);
      }
      closedir(indir);

      return output;
    }
  }
}


#endif
