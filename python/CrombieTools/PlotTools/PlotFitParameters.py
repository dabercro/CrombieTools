import ROOT
from .. import plotSrc

if not 'Plot2D' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'Plot2D.cc+')
if not 'PlotFitParameters' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotFitParameters.cc+')

def setupResolution(aPlotter):
    aPlotter.SetLooseFit('[2] * TMath::Gaus(x,[0],[1])')
    aPlotter.SetFunction('[4]*([3]/[1]*TMath::Gaus(x,[0],[1]) + (1 - [3])/[2]*TMath::Gaus(x,[0],[2]))')

    aPlotter.AddMapping(0,0)
    aPlotter.AddMapping(1,1)
    aPlotter.AddMapping(1,2)

    aPlotter.AddFunctionComponent('[4]*[3]/[1]*TMath::Gaus(x,[0],[1])')
    aPlotter.AddFunctionComponent('[4]*(1 - [3])/[2]*TMath::Gaus(x,[0],[2])')

def newResolutionPlotter():
    aPlotter = newFitPlotter()
    return setupResolution(aPlotter)

newFitPlotter = ROOT.PlotFitParameters
plotter       = newFitPlotter()
