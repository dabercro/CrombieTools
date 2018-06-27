#ifndef CROMBIE_TYPES_H
#define CROMBIE_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <list>

/**
   Defines a few types that are frequently used
*/

namespace crombie {
  namespace Types {

    using strings = std::vector<std::string>;

    /**
       Map with strings as the keys
       @param V The mapped type
     */
    template<typename V> using map = std::map<const std::string, V>;

  }
}

#endif
