import ROOT
from .. import plotSrc

if not 'PlotUtils' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotUtils.cc+')

SetZeroError = ROOT.SetZeroError
Divison = ROOT.Division
GetRatioToLines = ROOT.GetRatioToLines
GetRatioToLine = ROOT.GetRatioToLine
GetRatioToPoint = ROOT.GetRatioToPoint
