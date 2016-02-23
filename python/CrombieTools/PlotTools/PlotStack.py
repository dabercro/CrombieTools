import os
from .. import Load

newStackPlotter = Load('PlotStack')
plotter         = newStackPlotter()

def SetupFromEnv(aPlotter = plotter):
    if os.path.exists('CrombieAnalysisConfig.sh'):
        targets = [[aPlotter.SetLuminosity, 'CrombieLuminosity'],
                   [aPlotter.SetInDirectory, 'CrombieInFilesDir'],
                   [aPlotter.SetOutDirectory, 'CrombieOutPlotDir'],
                   [aPlotter.SetLimitTreeDir, 'CrombieOutLimitTreeDir'],
                   ]
        for target in targets:
            if os.environ.get(target[1]) == None:
                print 'Cannot find ' + target[1] + ' in config'
            else:
                target[0](os.environ[target[1]]

    else:
        print 'Could not find CrombieAnalysisConfig.sh'
