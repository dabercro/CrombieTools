#!/usr/bin/env python

import os
import sys
import argparse

import ROOT

def GetNumEntries(fileName, className, branches=[]):
    NumberOfEvents = 0

    testFile = ROOT.TFile(fileName)

    if len(testFile.GetListOfKeys()) == 0:
        return -1

    for testKey in testFile.GetListOfKeys():
        if className in testKey.GetClassName():
            testTree = testFile.Get(testKey.GetName())
            NumberOfEvents = testTree.GetEntries()

            if className == 'TTree':
                for branch in branches:
                    if not testTree.GetBranch(branch):
                        print 'Cannot find branch', branch
                        exit(5)

            if __name__ == '__main__':
                print className + ' \'' + testKey.GetName() + '\' has ' + str(NumberOfEvents) + ' events!'

            break

    testFile.Close()
    return NumberOfEvents

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='crombie findtree',
                                     description='Finds if there is an empty tree or not. Can also search for a branch.')

    parser.add_argument('files', metavar='FILE', nargs='+',
                        help='List of files to check.')
    parser.add_argument('-b', '--branches', metavar='BRANCH', dest='branches', nargs='*', default=[],
                        help='Branches to search for in each file.')
    parser.add_argument('-c', '--class', metavar='CLASS', dest='classname', default='TTree',
                        help='Class name to search for entries.')

    args = parser.parse_args()

    exitcode = 0

    for checkfile in args.files:
        print 'Searching', checkfile

        if os.path.isfile(checkfile):
            NumberOfEvents = GetNumEntries(checkfile, args.classname, args.branches)
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
