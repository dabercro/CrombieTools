#include <cmath>
#include <iostream>
#include <iomanip>

#include "crombie/FileConfig.h"
#include "crombie/Types.h"
#include "crombie/Cutflow.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " INDIR CUT [CUT [CUT ...]]" << std::endl;
    return 1;
  }

  // Input directory
  FileConfig::FileConfig fileconfig {std::string(argv[1])};

  Types::strings cuts {};
  long unsigned maxwidth = 0;
  for (int iarg = 2; iarg < argc; ++iarg) {
    cuts.push_back(argv[iarg]);
    maxwidth = std::max(maxwidth, cuts.back().size());
  }

  auto output = fileconfig.runfiles(Cutflow::SingleFile(cuts),
                                    Cutflow::Merge);

  for (unsigned icut = 0; icut < cuts.size(); ++icut)
    std::cout << std::setw(maxwidth) << cuts[icut]
              << std::setw(15) << output[icut]
              << std::endl;

  return 0;

}
