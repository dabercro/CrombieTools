import ROOT
from CrombieVars import plotSrc, skimSrc

ROOT.gROOT.LoadMacro(skimSrc + 'TreeContainer.cc+')
ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
ROOT.gROOT.LoadMacro(plotSrc + 'PlotStack.cc+')

newStackPlotter = ROOT.PlotStack
stackPlotter    = newStackPlotter()
