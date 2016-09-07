#!/usr/bin/env python

from CrombieTools.AnalysisTools.HistAnalysis import *
from CrombieTools.LoadConfig import cuts
import os

SetupFromEnv()

histAnalysis.AddDataFile('test_Data.root')
histAnalysis.SetBaseCut(cuts.cut('example', 'control'))
histAnalysis.SetMCWeight('allWeights')
histAnalysis.SetSignalName('MC1')
histAnalysis.SetNormalized(False)
histAnalysis.SetTreeName('test')

histAnalysis.MakeReweightHist('example_reweight.root', 'weight', 'example', 20, 0, 100)
