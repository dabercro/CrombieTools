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
    template<typename V> using map = std::map<const std::string, V>;

    /**
       The parameter passed to the FileConfig::runfiles reduce function
       @param M is the type given by a SingleOut mapping function
    */
    template<typename M> using ToMerge = Types::map<std::list<M>>;

  }
}

#endif
