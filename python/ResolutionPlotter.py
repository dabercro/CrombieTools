import ROOT
from FitPlotter import fitPlotter

resolutionPlotter = fitPlotter

resolutionPlotter.SetLooseFit('[2] * TMath::Gaus(x,[0],[1])')
resolutionPlotter.SetFunction('[4]*([3]/[1]*TMath::Gaus(x,[0],[1]) + (1 - [3])/[2]*TMath::Gaus(x,[0],[2]))')
