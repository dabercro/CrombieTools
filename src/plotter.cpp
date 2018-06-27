#include <iostream>
#include <fstream>

#include "crombie/FileSystem.h"
#include "crombie/FileConfig.h"
#include "crombie/PlotConfig.h"
#include "crombie/Selection.h"
#include "crombie/Misc.h"
#include "crombie/Plotter.h"
#include "crombie/Uncertainty.h"

#include "TStyle.h"
#include "TError.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 6) {
    std::cout << "Usage: " << argv[0] << " INDIR OUTDIR FILECONFIG PLOTCONFIG REGIONCONFIG [UNCCONFIG]" << std::endl;
    return 1;
  }

  // Read the configuration files
  auto files = FileConfig::read(argv[1], argv[3]);
  auto plots = PlotConfig::read(argv[4]);
  auto regions = Selection::read(Misc::env("normhist", "htotal"),
                                 argv[5]);

  Uncertainty::UncertaintyInfo unc {};
  if (argc > 6)
    std::ifstream{argv[6]} >> unc;

  // Run threads

  // Output is a map of vectors of Plot objects
  // The map key is the selection, vector is parallel to the ``plots`` variable above
  // Plot objects can make each plot
  auto outputs = files.runfiles(Plotter::SingleFile(plots, regions, unc),
                                Plotter::Merge(files));

  // Write plots
  std::string outdir = argv[2];
  FileSystem::mkdirs(outdir);

  // Some style stuff
  gStyle->SetOptStat(0);
  gStyle->SetGridStyle(3);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadTopMargin(0.07);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetHistLineWidth(2);

  gErrorIgnoreLevel = kWarning;

  for (auto& plot : outputs)
    plot.second.draw(outdir + "/" + plot.first);

  std::cout << "Created " << outputs.size() << " plots." << std::endl;

  return 0;
}
