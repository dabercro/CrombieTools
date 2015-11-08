import ROOT
from CrombieVars import packdir

ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotBase.cc+')
ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotHists.cc+')
ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotStack.cc+')

newStackPlotter = ROOT.PlotStack
stackPlotter    = newStackPlotter()
