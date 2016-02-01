import ROOT
from CrombieVars import anaSrc

if not 'HistWriter' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'HistWriter.cc+')

newHistWriter = ROOT.HistWriter
histWriter = newHistWriter()
