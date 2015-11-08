import ROOT
from CrombieVars import packdir

ROOT.gROOT.LoadMacro(packdir + '/SkimmingTools/src/TreeContainer.cc+')

newTreeContainer = ROOT.TreeContainer
treeContainer    = newTreeContainer()
