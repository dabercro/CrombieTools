#!/usr/bin/env python

from CrombieTools.PlotTools.PlotStack import *
from CrombieTools.LoadConfig import cuts
import os

SetupFromEnv()

plotter.AddDataFile(os.environ['CrombieFileBase'] + '_Data.root')
plotter.SetTreeName('test')
plotter.SetEventsPer(1.0)

plotter.SetRatioMinMax(0.0, 2.0)

def SetupArgs():
    return [
        ['example', 22, -5.0, 105.0, 'X Label', 'Y Label'],
        ['exampleDisc1', 22, -0.05, 1.05, 'X Label', 'Y Label'],
        ['exampleDisc2', 22, -0.05, 1.05, 'X Label', 'Y Label'],
        ]

MakePlots(cuts.categories, cuts.regions, SetupArgs(), False)
