#include <iostream>

#include "FileConfig.h"
#include "Selection.h"

using namespace Crombie;

int main(int argc, char* argv[]) {

  if (argc < 6) {
    std::cout << "Usage: " << argv[0] << " INDIR OUTDIR FILECONFIG PLOTCONFIG REGIONCONFIG" << std::endl;
    return 1;
  }

  // Read the configuration files
  auto files = FileConfig::read(argv[1], argv[3]);
  std::cout << files.runfiles(2,
                 std::function<unsigned long (const FileConfig::FileInfo&)> ([] (const FileConfig::FileInfo& info) {
                     return info.size;
                   }),
                 [] (const auto& dirinfos, const auto& maps) {
                   unsigned long output = 0;
                   for (auto& m : maps) {
                     unsigned long here = 0;
                     for (auto& out : m.second)
                       here += out;
                     std::cout << here << " " << m.first << std::endl;
                     output += here;
                   }
                   return output;
                              }) << std::endl;
  // auto plots = ConfigReader::readplots(argv[4]);
  auto regions = Selection::read(argv[5]);

  // Run threads

  // Merge files

  // Write plots

  return 0;
}
