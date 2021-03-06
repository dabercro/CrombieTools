#!/usr/bin/env python

import os
import sys
import argparse

import ROOT

# This dumb hack makes ROOT abort when it has to recover keys for a TFile
ROOT.gErrorAbortLevel = ROOT.kInfo

def GetNumEntries(fileName, className, branches=[]):
    NumberOfEvents = 0

    testFile = ROOT.TFile(fileName)

    if testFile.IsZombie():
        return -2

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


def main(argv):
    parser = argparse.ArgumentParser(prog='crombie findtree',
                                     description='Finds if there is an empty tree or not. Can also search for a branch.')

    parser.add_argument('files', metavar='FILE', nargs='+',
                        help='List of files to check.')
    parser.add_argument('-b', '--branches', metavar='BRANCH', dest='branches', nargs='*', default=[],
                        help='Branches to search for in each file.')
    parser.add_argument('-c', '--class', metavar='CLASS', dest='classname', default='TTree',
                        help='Class name to search for entries.')
    parser.add_argument('-v', '--verify', metavar='NUM', dest='num_events', default=0,
                       help='Optionally verify that the tree has the proper number of events')

    args = parser.parse_args(argv)

    total = 0

    exitcode = 0

    for checkfile in args.files:
        print 'Searching', checkfile

        if not os.stat(checkfile).st_size:
            print 'Error, file has zero size'
            exitcode += 50
        elif os.path.isfile(checkfile):
            NumberOfEvents = GetNumEntries(checkfile, args.classname, args.branches)

            if NumberOfEvents == -1:
                # This means that no keys were found in the file
                print 'No keys were found in the file'
                exitcode += 100
            elif NumberOfEvents == -2:
                # The file is a zombie
                print 'The file is a zombie'
                exitcode += 10
            elif NumberOfEvents == 0:
                print 'Did not successfully find tree with events.'
                exitcode += 1
            else:
                total += NumberOfEvents

        else:
            print 'Error, file does not exist.'
            exitcode += 1

    print 'Total number of events:', total
    if args.num_events and int(args.num_events) != int(total):
        exitcode += 1000

    print 'Exiting with exit code:', exitcode

    exit(exitcode)

if __name__ == '__main__':
    main(sys.argv[1:])
