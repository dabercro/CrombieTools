#ifndef CROMBIE_PLOTTER
#define CROMBIE_PLOTTER 1

#include <string>
#include <vector>
#include <utility>

struct Selection {
  std::string cut;
  std::string weight;
  std::string expr;
};

class Directory {

  enum class Type {
    data,
    signal,
    background
  };

 public:
  std::string path;
  Type type {data};
  double xs {0};
  struct process {
    std::string entry;
    std::string process;
    std::string cut;
  }
  std::vector<process> procs;        ///< Stores the processes that this directory goes into

}

#endif
