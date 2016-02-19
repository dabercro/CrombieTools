import ROOT
import os

ROOT.gROOT.SetBatch(True)

packdir = os.environ['CROMBIEPATH']

if packdir == '':
    print('#########################################################')
    print('#                                                       #')
    print('#   CROMBIEPATH variable is not set!                    #')
    print('#   You have not successfully installed CrombieTools.   #')
    print('#   Run CrombieTools/install.sh or read                 #')
    print('#   CrombieTools/README.md to troubleshoot.             #')
    print('#                                                       #')
    print('#########################################################')
    exit(1)

ROOT.gSystem.AddIncludePath('-I' + packdir + '/AnalysisTools/interface/')
ROOT.gSystem.AddIncludePath('-I' + packdir + '/PlotTools/interface/')
ROOT.gSystem.AddIncludePath('-I' + packdir + '/SkimmingTools/interface/')

anaSrc  = packdir + '/AnalysisTools/src/'
plotSrc = packdir + '/PlotTools/src/'
skimSrc = packdir + '/SkimmingTools/src/'

__all__ = ['AnalysisTools','PlotTools','SkimmingTools']
