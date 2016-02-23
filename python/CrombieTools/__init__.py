import ROOT
import os
import subprocess

__all__ = ['AnalysisTools','CommonTools','PlotTools','SkimmingTools','Parallelization']

ROOT.gROOT.SetBatch(True)

crombieDir = os.environ['CROMBIEPATH']

if crombieDir == '':
    print('#########################################################')
    print('#                                                       #')
    print('#   CROMBIEPATH variable is not set!                    #')
    print('#   You have not successfully installed CrombieTools.   #')
    print('#   Run CrombieTools/install.sh or read                 #')
    print('#   CrombieTools/README.md to troubleshoot.             #')
    print('#                                                       #')
    print('#########################################################')
    exit(1)

for package in __all__:
    if os.path.exists(crombieDir + '/' + package):
        ROOT.gSystem.AddIncludePath('-I' + crombieDir + '/' + package + '/interface/')

dependencies = { 'FlatSkimmer' :         ['GoodLumiFilter'],
                 'PlotFitParameters' :   ['Plot2D'],
                 'PlotROC' :             ['PlotHists'],
                 'PlotStack' :           ['TreeContainer','PlotHists'],
                 'CorrectorApplicator' : ['Corrector'],
                 'Reweighter' :          ['PlotStack'],
                 'TmvaClassifier' :      ['TreeContainer','PlotHists']
                 }

def Load(className):
    if not className in dir(ROOT):
        if type(dependencies.get(className)) == list:
            for depend in dependencies[className]:
                Load(depend)

        toLoad = ''
        for package in __all__:
            checkFile = crombieDir + '/' + package + '/src/' + className + '.cc'
            if os.path.exists(checkFile):
                toLoad = checkFile
                break

        if toLoad == '':
            print('')
            print('Can\'t find class: ' + className)
            print('in CrombieTools... Exiting.')
            exit(1)

        ROOT.gROOT.LoadMacro(toLoad + '+')

    if className in ['PlotUtils']:
        return 0

    return getattr(ROOT,className)


CrombieConfigs = ['CrombieSlimmingConfig.sh','CrombieAnalysisConfig.sh']

for config in CrombieConfigs:
    if os.path.exists(config):
        configContents = subprocess.Popen(['bash','-c','source ' + config + '; env'],stdout = subprocess.PIPE)
        for line in configContents.stdout:
            if type(line) == bytes:
                (key,sep,value) = line.decode('utf-8').partition('=')
            elif type(line) == str:
                (key,sep,value) = line.partition('=')
            else:
                print('Not sure how to handle subprocess output. Contact Dan.')
                break
            os.environ[str(key)] = str(value).strip('\n')

        configContents.communicate()

possibleImports = ['CrombieCutsFile']
for toImport in possibleImports:
    if not os.environ.get(toImport) == None:
        cuts = __import__(os.environ[toImport].strip('.py'), globals(), locals(), [], -1)
