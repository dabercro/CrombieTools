#! /usr/bin/env python

import sys

import ROOT

if __name__ == '__main__':
    for filename in sys.argv[2:]:
        rootfile = ROOT.TFile(filename)
        nevents = rootfile.events.GetEntries(sys.argv[1])
        rootfile.Close()
        if nevents:
            print '%s: %i' % (filename, nevents)
