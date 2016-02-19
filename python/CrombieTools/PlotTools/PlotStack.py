import ROOT
from .. import plotSrc, skimSrc

if not 'TreeContainer' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'TreeContainer.cc+')
if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
if not 'PlotStack' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotStack.cc+')

newStackPlotter = ROOT.PlotStack
plotter         = newStackPlotter()
