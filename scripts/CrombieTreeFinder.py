#! /usr/bin/python

import ROOT
import sys,os

def GetNumEntries(fileName, className):
    NumberOfEvents = 0

    testFile = ROOT.TFile(fileName)
    for testKey in testFile.GetListOfKeys():
        if testKey.GetClassName() == className:
            testTree = testFile.Get(testKey.GetName())
            NumberOfEvents = testTree.GetEntries()
            if __name__ == '__main__':
                print(className + ' \'' + testKey.GetName() + '\' has ' + str(NumberOfEvents) + ' events!')
            
            break

    testFile.Close()
    return NumberOfEvents

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Wrong number of args!')
        exit(1)

    if os.path.isfile(sys.argv[1]):
        NumberOfEvents = GetNumEntries(sys.argv[1], 'TTree')
    else:
        print('Error, file does not exist.')
        exit(1)

    if NumberOfEvents != 0:
        exit(0)
    else:
        print('Did not successfully find tree with events.')
        exit(1)
