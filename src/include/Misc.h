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
    }

    std::vector<std::string> tokenize(const std::string str) {
      std::istringstream ss {str};
      std::vector<std::string> output {
        std::istream_iterator<std::string>{ss},
        std::istream_iterator<std::string>{}
      };
      return output;
    }

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

    std::string env(std::string variable, std::string fallback) {
      auto* output = getenv(variable.data());
      if (output)
        return std::string(output);
      return fallback;
    }

  }
}

#endif
