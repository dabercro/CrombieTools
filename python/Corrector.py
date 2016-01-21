import ROOT
from CrombieVars import anaSrc

if not 'Corrector' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'Corrector.cc+')

newCorrector = ROOT.Corrector
