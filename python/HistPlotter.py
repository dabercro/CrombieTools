import ROOT
from CrombieVars import plotSrc

ROOT.gROOT.LoadMacro(plotSrc + 'PlotBase.cc+')
ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')

newHistPlotter = ROOT.PlotHists
histPlotter    = newHistPlotter()
