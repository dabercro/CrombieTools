import ROOT
from CrombieVars import skimSrc

ROOT.gROOT.LoadMacro(skimSrc + 'GoodLumiFilter.cc+')
ROOT.gROOT.LoadMacro(skimSrc + 'FlatSlimmer.cc+')

newFlatSlimmer = ROOT.FlatSlimmer
flatSlimmer = newFlatSlimmer()
