#ifndef CROMBIE_MISC_H
#define CROMBIE_MISC_H

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <array>
#include <set>
#include <functional>

#include "TROOT.h"

namespace Crombie {
  namespace Misc {

    void draw_progress(unsigned progress, unsigned max) {

      // Draw shitty progress bar
      std::cout << '\r' << '[';
      for (unsigned i = 0; i < max; i++) {
        if (i < progress)
          std::cout << '=';
        else if (i == progress)
          std::cout << '>';
        else
          std::cout << ' ';
      }
      std::cout << "] " << progress * 100/max << '%';
      std::flush(std::cout);
    }

    std::vector<std::string> tokenize(const std::string str) {
      std::istringstream ss {str};
      std::vector<std::string> output {
        std::istream_iterator<std::string>{ss},
        std::istream_iterator<std::string>{}
      };
      return output;
    }

    /// Get the output of a shell command
    std::string shell(std::string cmd) {
      std::array<char, 128> buffer;
      std::string output;
      std::shared_ptr<FILE> pipe(popen(cmd.data(), "r"), pclose);
      if (not pipe)
        throw std::runtime_error("popen() failed");
      while(not feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()))
          output += buffer.data();
      }
      return output;
    }

    /// Get an environment variable by name, with an optional fallback value
    std::string env(const std::string& variable, const std::string& fallback = "") {
      if (variable == "nthreads")
        ROOT::EnableThreadSafety();

      auto* output = getenv(variable.data());
      if (output)
        return std::string(output);
      if (not fallback.size())
        throw std::runtime_error(std::string("Requesting non-existent variable ") + variable + " with no fallback");
      return std::string(fallback);
    }

    template<typename O, typename C, typename F>
      std::vector<O> comprehension (const C& container, const F& func) {
      std::vector<O> output {};
      for (auto& iter : container)
        output.push_back(func(iter));
      return output;
    }

  }
}

#endif
