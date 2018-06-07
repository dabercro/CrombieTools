#ifndef CROMBIE_MISC_H
#define CROMBIE_MISC_H

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>

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

    std::vector<std::string> Tokenize(const std::string str) {
      std::istringstream ss {str};
      std::vector<std::string> output {
        std::istream_iterator<std::string>{ss},
        std::istream_iterator<std::string>{}
      };
      return output;
    }

  }
}

#endif
