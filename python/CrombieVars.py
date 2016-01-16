import ROOT
import os
import tdrStyle

tdrStyle.setTDRStyle()
ROOT.gROOT.SetBatch(True)

packdir = os.path.dirname(os.path.realpath(__file__))
packdir = "/".join(packdir.split('/')[:-1])

ROOT.gSystem.AddIncludePath('-I' + packdir + '/AnalysisTools/interface/')
ROOT.gSystem.AddIncludePath('-I' + packdir + '/PlotTools/interface/')
ROOT.gSystem.AddIncludePath('-I' + packdir + '/SkimmingTools/interface/')

anaSrc  = packdir + '/AnalysisTools/src/'
plotSrc = packdir + '/PlotTools/src/'
skimSrc = packdir + '/SkimmingTools/src/'
