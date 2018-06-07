#ifndef CROMBIE_FILESYSTEM_H
#define CROMBIE_FILESYSTEM_H

#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>


namespace Crombie {
  namespace FileSystem {

    /// Check if path exists (either directory or file)
    bool exists(const char* path) {
      struct stat buffer;
      return stat(path, &buffer) == 0;
    }
    bool exists(const std::string path) {
      return exists(path.data());
    }

    /// Get the size of a file
    unsigned get_size(const char* name) {
      struct stat file_stat;
      stat(name, &file_stat);
      return file_stat.st_size;
    }
    unsigned get_size(const std::string path) {
      return get_size(path.data());
    }

    /// Create directories, recursively if needed
    void mkdirs(const std::string path) {
      char path_array[512];
      // Stick a slash on the end to trick our character flipping
      auto addslash = path + "/";
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
    std::vector<std::string> list(std::string directory) {
      std::vector<std::string> output;

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
