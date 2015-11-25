import ROOT
from CrombieVars import plotSrc

if not 'Plot2D' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'Plot2D.cc+')

newFit2DPlotter = ROOT.Plot2D
fit2DPlotter    = newFit2DPlotter()
