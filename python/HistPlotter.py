import ROOT
from CrombieVars import plotSrc

if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')

newHistPlotter = ROOT.PlotHists
histPlotter    = newHistPlotter()
