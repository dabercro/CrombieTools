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
  for (int iarg = 2; iarg < argc; ++iarg)
    cuts.push_back(argv[iarg]);

  auto output = fileconfig.runfiles(Cutflow::SingleFile(cuts),
                                    Cutflow::Merge);

  for (unsigned i = 0; i < cuts.size(); ++i)
    std::cout << std::setw(30) << cuts[i]
              << std::setw(30) << output[i]
              << std::endl;

  return 0;

}
