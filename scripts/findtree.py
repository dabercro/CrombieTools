#!/usr/bin/env python

import ROOT
import sys, os

def GetNumEntries(fileName, className):
    NumberOfEvents = 0

    testFile = ROOT.TFile(fileName)

    if len(testFile.GetListOfKeys()) == 0:
        return -1

    for testKey in testFile.GetListOfKeys():
        if className in testKey.GetClassName():
            testTree = testFile.Get(testKey.GetName())
            NumberOfEvents = testTree.GetEntries()
            if __name__ == '__main__':
                print className + ' \'' + testKey.GetName() + '\' has ' + str(NumberOfEvents) + ' events!'

            break

    testFile.Close()
    return NumberOfEvents

if __name__ == '__main__':
    if len(sys.argv) == 1:
        print 'Wrong number of args!'
        exit(1)

    exitcode = 0

    for checkfile in sys.argv[1:]:
        print 'Searching', checkfile

        if os.path.isfile(sys.argv[1]):
            NumberOfEvents = GetNumEntries(sys.argv[1], 'TTree')
        else:
            print 'Error, file does not exist.'
            exitcode += 1

        if NumberOfEvents < 0:
            # This means that no keys were found in the file
            print 'No keys were found in the file'
            exitcode += 100
        elif NumberOfEvents == 0:
            print 'Did not successfully find tree with events.'
            exitcode += 1

    print 'Exiting with exit code:', exitcode

    exit(exitcode)
