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

    /**
       This generates and stores all of the edges needed in a histogram.
       There are two types of Binning, same width, and variable width.
       This is determined by the constructor.
    */
    class Binning {
    public:
      /**
         @brief Initializes the binning for a histogram.

         If the initializer list is three numbers,
         these are assumed to be the number of bins, minimum, and maximum,
         unless the first number is not an integer.
         (You can end the number with `'.'` or something to avoid this.)

         An initialization with one value is invalid.
       */
      Binning(std::initializer_list<std::string> bins);

      /**
         @param value The value on the binned axis to make an insertion.
         @returns `0` for an underflow bin, and `nbins + 1` for overflow bin.
       */
      unsigned find_bin(double value);

    private:
      bool same_width {};           ///< Tracks if this Binning is same width, or variable width
      unsigned nbins {};            ///< Number of bins
      double min {};                ///< Minimum value
      double max {};                ///< Maximum value
      std::vector<double> edges {}; ///< If variable width, store all of the edges

    };

  }
}

#endif
