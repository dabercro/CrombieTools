#include <iostream>

#include "FileSystem.h"
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
                                 argv[5]);

  // Run threads

  // Output is a map of vectors of Plot objects
  // The map key is the selection, vector is parallel to the ``plots`` variable above
  // Plot objects can make each plot
  auto outputs = files.runfiles(Plotter::SingleFile(plots, regions),
                                Plotter::Merge);

  // Write plots
  std::string outdir = argv[2];
  FileSystem::mkdirs(outdir);
  auto lumi = std::stod(Misc::env("lumi"));

  for (auto& sel : outputs) {
    for (unsigned iplot = 0; iplot < plots.size(); ++iplot) {
      auto filename = outdir + "/" + sel.first + "_" + plots[iplot].name;
      sel.second[iplot].draw(filename, lumi);
    }
  }

  return 0;
}
