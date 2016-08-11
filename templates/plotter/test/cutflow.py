#!/usr/bin/env python

from CrombieTools.AnalysisTools.CutflowMaker import cutflowMaker
from ROOT import TFile

testFile = TFile('../FullOut/test_Data.root')
cutflowMaker.AddTree(testFile.Get('test'))

cutflowMaker.AddCut('example discriminator 1', 'exampleDisc1 > 0.5')
cutflowMaker.AddCut('example discriminator 2', 'exampleDisc2 > 0.5')
cutflowMaker.AddCut('example', 'example > 50')

cutflowMaker.MakePlot('plots/cutflow', cutflowMaker.kFractional)
