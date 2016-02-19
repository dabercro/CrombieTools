import ROOT
from .. import plotSrc

if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')

newHistPlotter = ROOT.PlotHists
plotter        = newHistPlotter()
