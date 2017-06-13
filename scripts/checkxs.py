#! /usr/bin/env python

"""
Usage: %s FILE [FILE ...]

Checks MC Configuration file cross sections with the central database.
If there are mismatches, a new suggested configuration file is generated.
This file has the same name as the original file with the suffix '.new' appended.

Author: Daniel Abercrombie <dabercro@mit.edu>
"""

import sys

from CrossSecDB.reader import get_xsec

if __name__ == '__main__':

    if len(sys.argv) == 1:
        print __doc__
        exit(0)

    for file_name in sys.argv[1:]:
        print '\nChecking %s\n' % file_name

        suggest_new = False

        with open(file_name, 'r') as config_file:
            lines = list(config_file)

        line_nums = []
        samples = []
        xs = []
        for num, line in enumerate(lines):
            split = line.split()
            if len(split) < 5:
                continue

            line_nums.append(num)
            samples.append(split[1].split('.')[0].split('/')[-1])
            xs.append(float(split[2]))

        stored = get_xsec(samples)

        for index, stored_xs in enumerate(stored):
            if stored_xs != xs[index]:
                suggest_new = True

                print '%s cross section does not match: %s ----> %s' % \
                    (samples[index], xs[index], stored_xs)

                lines[line_nums[index]] = lines[line_nums[index]].replace(str(xs[index]), str(stored_xs))

        if suggest_new:
            print '\nWriting suggested config: %s.new\n' % file_name

            with open('%s.new' % file_name, 'w') as output_file:
                output_file.write(''.join(lines))
