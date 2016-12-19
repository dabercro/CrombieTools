#! /usr/bin/env python

import os

from CrombieTools.PlotTools.PlotBrazil import plotter
from CrombieTools.PlotTools import AddOutDir

plotter.ReadConfig('brazil.txt')

plotter.MakePlot(AddOutDir('brazil'), 'Mass [GeV]', 'Test limit')
