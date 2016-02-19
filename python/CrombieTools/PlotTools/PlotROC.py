import ROOT
from .. import plotSrc

if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
if not 'PlotROC' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotROC.cc+')

newRocPlotter = ROOT.PlotROC
plotter       = newRocPlotter()
