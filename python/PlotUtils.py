import ROOT
from CrombieVars import packdir

ROOT.gROOT.LoadMacro(packdir + '/PlotTools/src/PlotUtils.cc+')

GetRatioToPoint = ROOT.GetRatioToPoint
GetRatioToLine  = ROOT.GetRatioToLine
GetRatioToLines = ROOT.GetRatioToLines
