import ROOT
from CrombieVars import plotSrc

ROOT.gROOT.LoadMacro(plotSrc + 'Plot2D.cc+')
ROOT.gROOT.LoadMacro(plotSrc + 'PlotFitParameters.cc+')

newFitPlotter = ROOT.PlotFitParameters
fitPlotter    = newFitPlotter()
