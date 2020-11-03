#! /usr/bin/env python

import sys

sys.argv.append('-b')

import ROOT


if __name__ == '__main__':
    f = ROOT.TFile.Open(sys.argv[1])
    print '\n'.join(['%s/%s' % (k.GetName(), h.GetName()) \
                         for k in f.GetListOfKeys() \
                         for h in f.Get(k.GetName()).GetListOfKeys()])
