import ROOT
from CrombieVars import plotSrc

ROOT.gROOT.LoadMacro(plotSrc + 'PlotBase.cc+')
ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
ROOT.gROOT.LoadMacro(plotSrc + 'PlotStack.cc+')

newStackPlotter = ROOT.PlotStack
stackPlotter    = newStackPlotter()
