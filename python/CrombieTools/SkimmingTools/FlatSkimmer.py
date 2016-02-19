import ROOT
from .. import skimSrc

if not 'GoodLumiFilter' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'GoodLumiFilter.cc+')
if not 'FlatSkimmer' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'FlatSkimmer.cc+')

newFlatSkimmer = ROOT.FlatSkimmer
flatSkimmer    = newFlatSkimmer()
