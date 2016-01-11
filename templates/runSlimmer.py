#! /usr/bin/python

import sys,os
import ROOT

ROOT.gROOT.LoadMacro('slimmer.cc+')

if sys.argv[1] == "compile":
    exit()
else:
    if not os.path.isfile(sys.argv[2]):
        ROOT.NeroSlimmer(sys.argv[1],
                         sys.argv[2])
    else:
        print sys.argv[2] + " already exists!! Skipping..."

