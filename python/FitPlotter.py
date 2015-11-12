import ROOT
from CrombieVars import plotSrc

ROOT.gROOT.LoadMacro(plotSrc + 'PlotBase.cc+')
ROOT.gROOT.LoadMacro(plotSrc + 'PlotFitParameters.cc+')

newFitPlotter = ROOT.PlotFitParameters
fitPlotter    = newFitPlotter()
