import ROOT
from .. import skimSrc

if not 'TreeContainer' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'TreeContainer.cc+')

newTreeContainer = ROOT.TreeContainer
treeContainer    = newTreeContainer()
