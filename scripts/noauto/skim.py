#!/usr/bin/env python

import os
import argparse

from CrombieTools.SkimmingTools.FlatSkimmer import newFlatSkimmer, MakeFilter
from CrombieTools.Parallelization import RunOnDirectory

parser = argparse.ArgumentParser(prog='crombie skim',
    description='Slims the contents of one directory into another one')

parser.add_argument('-n', '--numproc', metavar='NUM', type=int, dest='numMaxProcesses', default=1, help='Number of processes that FlatSkimmer will spawn.')
parser.add_argument('-i', '--indir', metavar='DIR', type=str, dest='inDir', default='.', help='Directory that contains input files to be slimmed.')
parser.add_argument('-o', '--outdir', metavar='DIR', type=str, dest='outDir', default='.', help='Directory where slimmed stuff will be placed.')
parser.add_argument('-j', '--json', metavar='FILE', type=str, dest='GoodRunsFile', default='', help='Good runs json file location to be used.')
parser.add_argument('-c', '--cut', metavar='CUT', type=str, dest='cut', default='1', help='Cut used in slimming.')
parser.add_argument('-t', '--tree', metavar='NAME', type=str, dest='treeName', default='events', help='Name of tree that will be slimmed.')
parser.add_argument('--copy', metavar='NAMES', nargs='*', dest='copyObjects', default=[], help='List other object names to copy into the slimmed file.')
parser.add_argument('-r', '--run', metavar='EXPR', dest='runExpr', default='runNum', help='Set the expression for Run Number.')
parser.add_argument('-l', '--lumi', metavar='EXPR', dest='lumiExpr', default='lumiNum', help='Set the expression for Lumi Number.')
parser.add_argument('-e', '--event', metavar='EXPR', dest='eventExpr', default='eventNum', help='Set the expression for Lumi Number.')
parser.add_argument('-f', '--freq', metavar='NUM', dest='reportFreq', type=int, default=100000, help='Set the reporting frequency.')
parser.add_argument('-d', '--duplicate', action='store_true', dest='doDuplicate', help='Turn on duplicate checking.')
parser.add_argument('--filters', metavar='FILE', nargs='*', dest='filters', default=[], help='Set the filter files.')
parser.add_argument('--disable', metavar='FILE', type=str, dest='disableFile', default='', help='File that will remove branches from the full tree')
parser.add_argument('--map', metavar='FILE', type=str, dest='mapfile', default='', help='File that will map input file names to output files')

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
skimmer.SetEventExpr(args.eventExpr)
skimmer.SetCut(args.cut)
skimmer.SetReportFrequency(args.reportFreq)
skimmer.SetCheckDuplicates(args.doDuplicate)
skimmer.SetDisableFile(args.disableFile)
for copyObject in args.copyObjects:
    skimmer.AddCopyObject(copyObject)
for eventFilter in args.filters:
    skimmer.AddEventFilter(eventFilter)

if args.mapfile:
    with open(args.mapfile, 'r') as mapfile:
        for line in mapfile:
            in_file, out_file = line.split()
            skimmer.SetInputOutputMap(in_file, out_file)

RunOnDirectory(skimmer, args.numMaxProcesses)
