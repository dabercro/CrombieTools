import ROOT
from CrombieVars import anaSrc, skimSrc

if not 'LimitTreeMaker' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'LimitTreeMaker.cc+')

newLimitTreeMaker = ROOT.LimitTreeMaker
