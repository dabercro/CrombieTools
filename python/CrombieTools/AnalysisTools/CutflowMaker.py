import ROOT
from .. import anaSrc

if not 'CutflowMaker' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'CutflowMaker.cc+')

newCutflowMaker = ROOT.CutflowMaker
cutflowMaker = newCutflowMaker()
