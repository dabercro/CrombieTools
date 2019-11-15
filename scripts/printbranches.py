#! /usr/bin/env python

import os
import sys

sys.argv.append('-b')

import ROOT


if __name__ == '__main__':
    FILE = ROOT.TFile.Open(sys.argv[1])
    print '\n'.join([br.Print() if os.environ.get('print') else br.GetName()
                     for br in getattr(FILE, os.environ.get('tree', 'events')).GetListOfBranches()])
