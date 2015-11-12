import ROOT
import os

packdir = os.path.dirname(os.path.realpath(__file__))
packdir = "/".join(packdir.split('/')[:-1])

ROOT.gSystem.AddIncludePath('-I' + packdir + '/PlotTools/interface/')
ROOT.gSystem.AddIncludePath('-I' + packdir + '/SkimmingTools/interface/')

plotSrc = packdir + '/PlotTools/src/'
skimSrc = packdir + '/SkimmingTools/src/'
