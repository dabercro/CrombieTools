#!/usr/bin/env python

import os, argparse

from CrombieTools.SkimmingTools.FlatSkimmer import newFlatSkimmer, MakeFilter
from CrombieTools.Parallelization import RunOnDirectory

parser = argparse.ArgumentParser(prog='crombie skim',
    description = 'Slims the contents of one directory into another one')

parser.add_argument('--numproc', '-n', metavar = 'NUM', type = int, dest = 'numMaxProcesses', default = 1, help = 'Number of processes that FlatSkimmer will spawn.')
parser.add_argument('--indir', '-i', metavar = 'DIR', type = str, dest = 'inDir', default = '.', help = 'Directory that contains input files to be slimmed.')
parser.add_argument('--outdir', '-o', metavar = 'DIR', type = str, dest = 'outDir', default = '.', help = 'Directory where slimmed stuff will be placed.')
parser.add_argument('--json','-j', metavar = 'FILE', type = str, dest = 'GoodRunsFile', default = '', help = 'Good runs json file location to be used.')
parser.add_argument('--cut','-c', metavar = 'CUT', type = str, dest = 'cut', default = '1', help = 'Cut used in slimming.')
parser.add_argument('--tree','-t', metavar = 'NAME', type = str, dest = 'treeName', default = 'events', help = 'Name of tree that will be slimmed.')
parser.add_argument('--copy', metavar = 'NAMES', nargs = '*', dest = 'copyObjects', default = [], help = 'List other object names to copy into the slimmed file.')
parser.add_argument('--run', '-r', metavar = 'EXPR', dest = 'runExpr', default = 'runNum', help = 'Set the expression for Run Number.')
parser.add_argument('--lumi', '-l', metavar = 'EXPR', dest = 'lumiExpr', default = 'lumiNum', help = 'Set the expression for Lumi Number.')
parser.add_argument('--freq', '-f', metavar = 'NUM', dest = 'reportFreq', type = int, default = 100000, help = 'Set the reporting frequency.')
parser.add_argument('--filters', '-e', metavar = 'FILE', nargs = '*', dest = 'filters', default = [], help = 'Set the filter files.')
parser.add_argument('--duplicate','-d', action = 'store_true', dest = 'doDuplicate', help = 'Turn on duplicate checking.')

args = parser.parse_args()

if args.inDir == args.outDir:
    print ""
    print " ################################################################"
    print " #                                                              #"
    print " #  You have to specify at least an input or output directory!  #"
    print " #                                                              #"
    print " ################################################################"
    print ""
    parser.print_help()
    print ""
    exit(-1)

if not os.path.exists(args.outDir):
    os.makedirs(args.outDir)

goodRunFilter = MakeFilter(args.GoodRunsFile)
skimmer = newFlatSkimmer()
skimmer.SetInDirectory(args.inDir)
skimmer.SetOutDirectory(args.outDir)
skimmer.SetGoodLumiFilter(goodRunFilter)
skimmer.SetTreeName(args.treeName)
skimmer.SetRunExpr(args.runExpr)
skimmer.SetLumiExpr(args.lumiExpr)
skimmer.SetCut(args.cut)
skimmer.SetReportFrequency(args.reportFreq)
skimmer.SetCheckDuplicates(args.doDuplicate)
for copyObject in args.copyObjects:
    skimmer.AddCopyObject(copyObject)
for eventFilter in args.filters:
    skimmer.AddEventFilter(eventFilter)

RunOnDirectory(skimmer, args.numMaxProcesses)
