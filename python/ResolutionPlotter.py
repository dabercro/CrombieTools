import ROOT
from FitPlotter import *

def setupResolution(aPlotter):
    aPlotter.SetLooseFit('[2] * TMath::Gaus(x,[0],[1])')
    aPlotter.SetFunction('[4]*([3]/[1]*TMath::Gaus(x,[0],[1]) + (1 - [3])/[2]*TMath::Gaus(x,[0],[2]))')

    aPlotter.AddMapping(0,0)
    aPlotter.AddMapping(1,1)
    aPlotter.AddMapping(1,2)

    aPlotter.AddFunctionComponent('[4]*[3]/[1]*TMath::Gaus(x,[0],[1])')
    aPlotter.AddFunctionComponent('[4]*(1 - [3])/[2]*TMath::Gaus(x,[0],[2])')

    return aPlotter
##

def newResolutionPlotter():
    aPlotter = newFitPlotter()
    return setupResolution(aPlotter)
##

resolutionPlotter = setupResolution(fitPlotter)
