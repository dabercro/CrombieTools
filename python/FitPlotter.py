import ROOT
from CrombieVars import packdir

ROOT.gROOT.LoadMacro(packdir + '/PlotTools/PlotBase.cc+')
ROOT.gROOT.LoadMacro(packdir + '/PlotTools/PlotFitParameters.cc+')

fitPlotter = ROOT.PlotFitParameters()
