import ROOT
from CrombieVars import skimSrc

ROOT.gROOT.LoadMacro(skimSrc + 'TreeContainer.cc+')

newTreeContainer = ROOT.TreeContainer
treeContainer    = newTreeContainer()
