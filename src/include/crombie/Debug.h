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
          _printed = true;
        std::cout << message << " ";
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
