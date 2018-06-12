#include <iostream>

#include "FileConfig.h"
#include "PlotConfig.h"
#include "Selection.h"
#include "Misc.h"
#include "Plotter.h"

using namespace Crombie;

int main(int argc, char* argv[]) {

  if (argc < 6) {
    std::cout << "Usage: " << argv[0] << " INDIR OUTDIR FILECONFIG PLOTCONFIG REGIONCONFIG" << std::endl;
    return 1;
  }

  // Read the configuration files
  auto files = FileConfig::read(argv[1], argv[3]);
  auto plots = PlotConfig::read(argv[4]);
  auto regions = Selection::read(Misc::env("normhist", "htotal"),
                                 std::stod(Misc::env("lumi")),
                                 argv[5]);

  // Run threads

  auto ouputs = files.runfiles(Plotter::SingleFile(plots, regions),
                               Plotter::Merge);

  // Write plots

  return 0;
}
