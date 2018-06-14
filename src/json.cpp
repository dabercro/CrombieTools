#include <iostream>
#include <sstream>
#include <fstream>

#include "FileConfig.h"
#include "Lumi.h"

using namespace Crombie;

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " INDIR [OUTFILE]" << std::endl;
    return 1;
  }

  // Input directory
  std::stringstream input {std::string(argv[1]) + " {}"};
  FileConfig::FileConfig fileconfig {""};
  input >> fileconfig;

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
