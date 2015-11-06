import ROOT
from CrombieVars import packdir

ROOT.gROOT.LoadMacro(packdir + '/PlotTools/PlotUtils.cc+')

GetRatioToPoint = ROOT.GetRatioToPoint
GetRatioToLine  = ROOT.GetRatioToLine
GetRatioToLines = ROOT.GetRatioToLines
