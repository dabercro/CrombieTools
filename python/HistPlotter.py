import ROOT
from CrombieVars import plotSrc

ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')

newHistPlotter = ROOT.PlotHists
histPlotter    = newHistPlotter()
