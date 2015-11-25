import ROOT
from CrombieVars import plotSrc

if not 'Plot2D' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'Plot2D.cc+')
if not 'PlotFitParameters' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotFitParameters.cc+')

newFitPlotter = ROOT.PlotFitParameters
fitPlotter    = newFitPlotter()
