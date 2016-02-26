#! /usr/bin/python

import ROOT
import sys,os

NumberOfEvents = 0

if os.path.isfile(sys.argv[1]):
    testFile = ROOT.TFile(sys.argv[1])
    for testKey in testFile.GetListOfKeys():
        if testKey.GetClassName() == 'TTree':
            testTree = testFile.Get(testKey.GetName())
            NumberOfEvents = testTree.GetEntriesFast()
            if NumberOfEvents != 0:
                print 'Tree \'' + testKey.GetName() + '\' has ' + str(NumberOfEvents) + ' events!'
                exit(0)
else:
    print 'Error, file does not exist.'
    exit(1)

print 'Did not successfully find tree with events.'
exit(1)
