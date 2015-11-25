import ROOT
from CrombieVars import skimSrc

if not 'GoodLumiFilter' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'GoodLumiFilter.cc+')
if not 'FlatSlimmer' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'FlatSlimmer.cc+')

newFlatSlimmer = ROOT.FlatSlimmer
flatSlimmer = newFlatSlimmer()
