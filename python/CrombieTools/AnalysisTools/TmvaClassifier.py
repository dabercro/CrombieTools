import ROOT
from .. import anaSrc, plotSrc, skimSrc

if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
if not 'TreeContainer' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'TreeContainer.cc+')
if not 'TmvaClassifier' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'TmvaClassifier.cc+')

newClassifier = ROOT.TmvaClassifier
classifier    = newClassifier()
