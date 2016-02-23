import os
from .. import Load

newStackPlotter = Load('PlotStack')
plotter         = newStackPlotter()

def SetupFromEnv(aPlotter = plotter):
    from .. import LoadConfig
    if os.path.exists('CrombieAnalysisConfig.sh'):
        def readMC(config):
            aPlotter.ReadMCConfig(config,aPlotter.kBackground)
        def readSignal(config):
            aPlotter.ReadMCConfig(config,aPlotter.kSignal)

        targets = [[aPlotter.SetLuminosity, 'CrombieLuminosity'],
                   [aPlotter.SetInDirectory, 'CrombieInFilesDir'],
                   [aPlotter.SetOutDirectory, 'CrombieOutPlotDir'],
                   [aPlotter.SetLimitTreeDir, 'CrombieOutLimitTreeDir'],
                   [readMC,'CrombieMCConfig']
                   ]
        for target in targets:
            if os.environ.get(target[1]) == None:
                print 'Cannot find ' + target[1] + ' in config'
            else:
                try:
                    target[0](os.environ[target[1]])
                except:
                    target[0](float(os.environ[target[1]]))

    else:
        print 'Could not find CrombieAnalysisConfig.sh'

def SetCuts(category,region,aPlotter = plotter):
    from .. import LoadConfig
    cuts = LoadConfig.cuts
    aPlotter.SetDefaultWeight(cuts.cut(category,region))
    aPlotter.SetMCWeights(cuts.dataMCCuts(region,False))
    aPlotter.SetDataWeights(cuts.dataMCCuts(region,True))

def ReadExceptConfig(region,aPlotter = plotter):
    from .. import LoadConfig
    if os.environ.get('CrombieExcept_' + region) == None:
        print 'Region ' + region + ' does not have an except config set!'
    else:
        plotter.ReadMCConfig(os.environ['CrombieExcept_' + region])
