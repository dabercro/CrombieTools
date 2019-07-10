#! /usr/bin/env python

import sys

sys.argv.append('-b')

import ROOT


if __name__ == '__main__':
    FILE = ROOT.TFile(sys.argv[1])
    print '\n'.join([br.GetName() for br in FILE.events.GetListOfBranches()])
