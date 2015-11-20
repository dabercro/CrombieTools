import ROOT
from CrombieVars import plotSrc

ROOT.gROOT.LoadMacro(plotSrc + 'Plot2D.cc+')

newFit2DPlotter = ROOT.Plot2D
fit2DPlotter    = newFit2DPlotter()
