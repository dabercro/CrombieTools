import ROOT
from CrombieVars import plotSrc

ROOT.gROOT.LoadMacro(plotSrc + 'PlotBase.cc+')

GetRatioToPoint = ROOT.GetRatioToPoint
