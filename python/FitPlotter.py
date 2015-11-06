import ROOT
from CrombieVars import packdir

ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotBase.cc+')
ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotFitParameters.cc+')

fitPlotter = ROOT.PlotFitParameters()
