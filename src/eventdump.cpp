#include <iostream>
#include <iomanip>
#include <fstream>

#include "crombie/Types.h"
#include "crombie/FileConfig.h"
#include "crombie/EventDump.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 4) {
    std::cout << "Usage: " << argv[0] << " INDIR OUTFILE CUT [BRANCH [BRANCH ...]]" << std::endl;
    return 1;
  }

  // If variables not given, get a default list
  const Types::strings vars = [=] () {
    if (argc == 4) {
      Types::strings output {"runNumber", "lumiNumber", "eventNumber"};
      return output;
    }
    Types::strings output;
    for (int i = 4; i < argc; ++i)
      output.push_back(argv[i]);
    return output;
  } ();

  // Input directory
  FileConfig::FileConfig fileconfig {argv[1], true};

  auto output = fileconfig.runfiles(EventDump::SingleFile(argv[3], vars), EventDump::Merge);

  std::ofstream outfile {argv[2]};

  for (auto& var : vars)
    outfile << " * " << std::setw(15) << var;
  outfile << " *\n";

  for (auto& line : output) {
    for (auto out : line)
      outfile << " * " << std::setw(15) << std::setprecision(10) << out;
    outfile << " *\n";
  }

  outfile.close();

  return 0;
}
