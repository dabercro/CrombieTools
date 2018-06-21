#ifndef CROMBIE_DEBUG_H
#define CROMBIE_DEBUG_H

#include <cstdlib>
#include <iostream>

namespace crombie {
  namespace Debug {

    namespace {
      static bool _printed = false;

      template<typename T> void OneMessage (T message) {
        if (not _printed)
          std::cout << "[";
        std::cout << message;
        if (not _printed)
          std::cout << "]\n";
        std::cout << " ";
        _printed = true;
      }

      void Debug() {
        _printed = false;
        std::cout << std::endl;
      }
    }

    const bool debug = getenv("debug");

    template<typename T, typename... V> void Debug(T message, V... more) {
      if (debug) {
        OneMessage(message);
        Debug(more...);
      }
    }

  }
}

#endif
