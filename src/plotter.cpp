#include <iostream>

#include "crombie/FileSystem.h"
#include "crombie/FileConfig.h"
#include "crombie/PlotConfig.h"
#include "crombie/Selection.h"
#include "crombie/Misc.h"
#include "crombie/Plotter.h"

#include "TStyle.h"

using namespace crombie;

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
                                Plotter::Merge(files));

  // Write plots
  std::string outdir = argv[2];
  FileSystem::mkdirs(outdir);

  gStyle->SetOptStat(0);

  for (auto& plot : outputs)
    plot.second.draw(outdir + "/" + plot.first);

  return 0;
}
