import ROOT
import os
import subprocess

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

if os.path.exists('CrombieSlimmingConfig.sh'):
    configContents = subprocess.Popen(['bash','-c','source CrombieSlimmingConfig.sh; env'],stdout = subprocess.PIPE)
    for line in configContents.stdout:
        if type(line) == bytes:
            (key,sep,value) = line.decode('utf-8').partition('=')
        elif type(line) == str:
            (key,sep,value) = line.partition('=')
        else:
            print('Not sure how to handle subprocess output. Contact Dan.')
            break
        os.environ[key] = str(value).strip('\n')

    configContents.communicate()


__all__ = ['AnalysisTools','PlotTools','SkimmingTools','Parallelization']
