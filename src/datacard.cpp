#include <vector>
#include <utility>

#include "crombie/Misc.h"
#include "crombie/FileSystem.h"
#include "crombie/FileConfig.h"
#include "crombie/PlotConfig.h"
#include "crombie/Selection.h"
#include "crombie/Plotter.h"
#include "crombie/Uncertainty.h"
#include "crombie/Datacard.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 8) {
    std::cout << "Usage: " << argv[0] << " INDIR OUTDIR FILECONFIG PLOTCONFIG REGIONCONFIG UNCCONFIG SELECTION:PLOT [SELECTION:PLOT ...]" << std::endl;
    return 1;
  }

  if (not FileSystem::confirm_overwrite(argv[2]))
    return 1;

  // Read the configuration files
  auto files = FileConfig::read(argv[1], argv[3]);
  auto plots = PlotConfig::read(argv[4]);
  auto regions = Selection::read(Misc::env("normhist", "hSumW"),
                                 argv[5]);
  auto unc = Uncertainty::read(argv[6]);

  // Track selections used
  std::set<std::string> selections;
  // Only do the plots required
  std::vector<std::pair<std::string, std::string>> requested;
  for (int iarg = 7; iarg < argc; ++iarg) {
    auto elems = Misc::split(argv[iarg], ":");
    if (elems.size() != 2)
      throw std::runtime_error{std::string("Poorly formatted argument: ") + argv[iarg]};
    auto inserted = selections.insert(elems[0]);
    if (not inserted.second)
      throw std::runtime_error{elems[0] + " seems to be used twice in this datacard"};
    requested.push_back(std::make_pair(elems[0], elems[1]));
  }

  auto outputs = files.runfiles(Plotter::SingleFile(plots, regions, unc, true, requested),
                                Plotter::Merge(files));

  Datacard::dumpplots(argv[2], selections, unc, outputs);

  return 0;
}
