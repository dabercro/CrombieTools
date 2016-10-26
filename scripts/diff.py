#!/usr/bin/env python

import argparse
import os
import sys
sys.path.insert(0, os.environ['CROMBIEPATH'] + '/scripts')

from findtree import GetNumEntries
from CrombieTools.Parallelization import RunOnDirectory

class CrombieDiffDoer:
    def __init__(self, dir0, dir1, treeName, skipBranches, verbose, checkbranch):
        self.dir0 = dir0.rstrip('/')
        self.dir1 = dir1.rstrip('/')
        self.treeName = treeName
        self.skipBranches = skipBranches
        self.verbose = verbose
        self.checkbranch = checkbranch

    def GetInDirectory(self):
        return self.dir0 + '/'

    def Copy(self):
        return self

    def RunOnFile(self, fileName):
        if os.path.exists(self.dir1 + '/' + fileName):
            hist0 = GetNumEntries(self.dir0 + '/' + fileName, 'TH1')
            hist1 = GetNumEntries(self.dir1 + '/' + fileName, 'TH1')
            if hist0 != hist1:
                print(fileName + ' has different number of events in hists: ' + str(hist0) + '     ' + str(hist1))
            elif self.verbose:
                print(fileName + ' has same number of events in hists: ' + str(hist0))

            tree0 = GetNumEntries(self.dir0 + '/' + fileName, 'TTree')
            tree1 = GetNumEntries(self.dir1 + '/' + fileName, 'TTree')
            if tree0 != tree1:
                print(fileName + ' has different number of events in trees: ' + str(tree0) + '     ' + str(tree1))
            else:
                if self.verbose:
                    print(fileName + ' has same number of events in trees: ' + str(tree0))

                if self.checkbranch:
                    from ROOT import TFile
                    file0 = TFile(self.dir0 + '/' + fileName)
                    tree0 = getattr(file0,self.treeName)

                    tree0.AddFriend('tree1_thefriend=' + self.treeName, self.dir1 + '/' + fileName)

                    for branch in tree0.GetListOfBranches():
                        if branch.GetName() in self.skipBranches:
                            continue

                        if self.verbose:
                            print('Checking branch ' + branch.GetName())

                        numDiff = tree0.GetEntries(branch.GetName() + ' != tree1_thefriend.' + branch.GetName())
                        if numDiff != 0:
                            print(fileName + ' has different values in: ' + branch.GetName() + ' (' + str(numDiff) + '/' + str(tree1) + ')')

                    file0.Close()
        elif self.verbose:
            print('WARNING: ' + fileName + ' is in ' + self.dir0 + ', but not in ' + self.dir1)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(prog='crombie diff',
        description = 'Checks two directories to make sure they have the same number of files' +
                      ' and number of events in the tree and hist for each file')

    parser.add_argument('dirs', metavar='DIRECTORY', nargs=2, help='The names of the two directories to compare.')
    parser.add_argument('--numproc', '-n', metavar = 'NUM', type=int, dest = 'numMaxProcesses', default = 1, help = 'Number of processes that FlatSkimmer will spawn.')
    parser.add_argument('--treename', '-t', metavar='TREENAME', type=str, dest='treename', default='events',
                        help='The name of the trees to friend.')
    parser.add_argument('--skip-branches', '-s', metavar = 'BRANCHES', nargs = '*', dest = 'skipbranches', default = [],
                        help = 'Set branches to skip comparison.')
    parser.add_argument('--verbose', '-v', dest='verbose', action='store_true', help = 'Give a verbose checker to watch progress.')
    parser.add_argument('--check-branch', '-b', dest='checkbranch', action='store_true', help = 'Checks if two branches have the same entries.')

    args = parser.parse_args()

    for fileName in os.listdir(args.dirs[1]):
        if '.root' not in fileName:
            continue

        if not os.path.exists(args.dirs[0] + '/' + fileName):
            print('WARNING: ' + fileName + ' is in ' + args.dirs[1] + ', but not in ' + args.dirs[0])

    print('\nGoing on to compare number of events:         ' + args.dirs[0] + ' --to-- ' + args.dirs[1] + '\n')

    differ = CrombieDiffDoer(args.dirs[0], args.dirs[1], args.treename, args.skipbranches, args.verbose, args.checkbranch)

    RunOnDirectory(differ, args.numMaxProcesses, printing=False)

    if not args.checkbranch:
        print('Did not check individual branches. Enable with -b, if desired.\n')
