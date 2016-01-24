import ROOT
from CrombieVars import *

if not 'TreeContainer' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'TreeContainer.cc+')
if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
if not 'PlotStack' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotStack.cc+')
if not 'Reweighter' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'Reweighter.cc+')

newReweighter = ROOT.Reweighter
