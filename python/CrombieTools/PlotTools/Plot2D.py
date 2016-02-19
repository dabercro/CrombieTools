import ROOT
from .. import plotSrc

if not 'Plot2D' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'Plot2D.cc+')

newFit2DPlotter = ROOT.Plot2D
plotter         = newFit2DPlotter()
