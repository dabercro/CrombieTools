#include <libgen.h>

#include "crombie/FileConfig.h"
#include "crombie/PlotConfig.h"
#include "crombie/Selection.h"
#include "crombie/Plotter.h"
#include "crombie/Uncertainty.h"

using namespace crombie;

int main(int argc, char* argv[]) {

  if (argc < 8) {
    std::cout << "Usage: " << argv[0] << " INDIR OUTBASE FILECONFIG PLOTCONFIG REGIONCONFIG UNCCONFIG SELECTION:PLOT [SELECTION:PLOT ...]" << std::endl;
    return 1;
  }

}
