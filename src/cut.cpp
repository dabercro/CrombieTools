#include <iostream>

#include "crombie/Selection.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " CONFIG SELECTION" << std::endl;
    return 1;
  }

  auto regions = Selection::read("", argv[1]);

  std::cout << regions.selections.at(argv[2]).cut << std::endl;

  for (int iarg = 3; iarg < argc; ++iarg)
    std::cout << std::endl << argv[iarg] << ":" << std::endl
              << Selection::nminus1(argv[iarg], regions.selections.at(argv[2]).cut)
              << std::endl;

  return 0;

}
