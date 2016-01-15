import ROOT
from CrombieVars import plotSrc

if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
if not 'PlotROC' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotROC.cc+')

newRocPlotter = ROOT.PlotROC
rocPlotter = newRocPlotter()
