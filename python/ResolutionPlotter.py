import ROOT
from FitPlotter import fitPlotter

resolutionPlotter = fitPlotter

resolutionPlotter.SetLooseFit('[2] * TMath::Gaus(x,[0],[1])')
