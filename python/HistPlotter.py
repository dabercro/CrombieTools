import ROOT
from CrombieVars import packdir

ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotBase.cc+')
ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotHists.cc+')

newHistPlotter = ROOT.PlotHists
histPlotter    = newHistPlotter()
