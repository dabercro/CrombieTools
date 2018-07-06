#include <iostream>

#include "crombie/Selection.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " CONFIG [SELECTION ...]" << std::endl;
    return 1;
  }

  auto regions = Selection::read("", argv[1]);

  if (argc == 2) {
    std::cout << std::endl << "OR of all:" << std::endl;
    bool notfirst = false;
    for (auto& sel : regions.selections) {
      if (notfirst)
        std::cout << " || ";
      std::cout << "(" << sel.second.cut << ")";
      notfirst = true;
    }
    std::cout << std::endl;
  }
  else {
    for (int iarg = 2; iarg < argc; ++iarg)
      std::cout << std::endl << argv[iarg] << ":" << std::endl
                << Selection::nminus1(argv[iarg], regions.selections.at(argv[2]).cut)
                << std::endl;
  }

  return 0;

}
