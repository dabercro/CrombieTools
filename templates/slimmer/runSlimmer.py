#!/usr/bin/env python

import sys,os
import ROOT

OutTreeName = 'OutTree'

os.system('crombie maketree ' + OutTreeName)
ROOT.gROOT.LoadMacro(OutTreeName + '.cc+')
ROOT.gROOT.LoadMacro('slimmer.cc+')

# Load any other needed macros here

if len(sys.argv) < 3:
    exit()
else:
    if not os.path.isfile(sys.argv[2]):
        ROOT.slimmer(sys.argv[1],
                     sys.argv[2])
    else:
        print(sys.argv[2] + ' already exists!! Skipping...')
