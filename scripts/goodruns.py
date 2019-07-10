#! /usr/bin/env python

"""
Makes a checkrun header file with the specified GoodRun JSONs hardcoded in for speed.
"""

import sys
import json
from collections import defaultdict

if __name__ == '__main__':
    output_base = sys.argv.pop()

    if len(sys.argv) < 2:
        print 'Not enough arguments'
        exit()

    input_files = sys.argv[1:]
    goodlumis = defaultdict(lambda: set())

    for in_name in input_files:
        with open(in_name, 'r') as in_file:
            inputs = json.load(in_file)

        for run, lumisecs in inputs.items():
            for lumisec in lumisecs:
                for lumi in range(lumisec[0], lumisec[1]+1):
                    goodlumis[int(run)].add(int(lumi))


    with open(output_base + '.h', 'w') as output:
        output.write("""#ifndef CROMBIE_GOODRUN_H
#define CROMBIE_GOODRUN_H 1

#include <limits.h>
#include <unordered_set>

template <typename R, typename L>
bool checkrun(R run, L lumi) {
  if (run == 1)
    return true;

  using key_t = unsigned long long;
  static_assert(sizeof(key_t) >= sizeof(R) + sizeof(L), "Key type is too small");

  static key_t last_key = 0;
  static bool last_result = false;

  key_t key = (static_cast<key_t>(run) << (sizeof(R) * CHAR_BIT)) + lumi;
  if (key == last_key)
    return last_result;
  last_key = key;

  static const std::unordered_set<key_t> goodruns {
""")

        output.write('    ' + ',\n    '.join(
            sum([['(%sl << (sizeof(R) * CHAR_BIT)) + %s' % (run, lumi) for lumi in goodlumis[run]] for run in goodlumis], [])
        ))

        output.write("""
  };

  return last_result = (goodruns.find(key) != goodruns.end());
}

#endif
""")
