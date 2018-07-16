#include <iostream>
#include <fstream>

#include "crombie/FileSystem.h"
#include "crombie/FileConfig.h"
#include "crombie/Lumi.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " INDIR [OUTFILE]" << std::endl;
    return 1;
  }

  if (argc >= 3 and not FileSystem::confirm_overwrite(argv[2]))
    return 1;

  // Input directory
  FileConfig::FileConfig fileconfig {argv[1]};

  auto output = fileconfig.runfiles(Lumi::SingleFile, Lumi::Merge);

  if (argc >= 3) {
    std::ofstream outfile {argv[2]};
    outfile << output;
    outfile.close();
  }
  else
    std::cout << output;

  return 0;
}
