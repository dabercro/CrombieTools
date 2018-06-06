#! /usr/bin/env python

import os
import sys
import glob

from CrombieTools import LoadConfig
from CrombieTools.ConfigTools import Line

if __name__ == '__main__':

    indir = os.environ['CrombieInFilesDir']

    inconfig = sys.argv[1]
    outconfig = sys.argv[2]

    lines = [Line(l) for l in open(inconfig, 'r') if l.strip()]

    expanded = []

    for line in lines:
        fulldir = os.path.join(indir, line.file)
        if os.path.isdir(fulldir):
            for f in glob.iglob(fulldir + '/*.root'):
                expanded.extend(['INGROUP',
                                 ('' if line.saved else '#') +
                                 ' '.join([line.treename, f, line.xs, line.entry, line.color])])
            expanded.append('ENDGROUP\n')
        else:
            expanded.append(line.raw.strip())

    with open(outconfig, 'w') as out:
        out.write('\n'.join(expanded))
