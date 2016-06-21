from .. import Load

newHistAnalysis = Load('HistAnalysis')
histAnalysis = newHistAnalysis()

def SetupFromEnv(analyzer=histAnalysis):
    """Sets up a HistAnalysis for the user by reading the CrombieAnalysisConfig.
    @param analyzer is the HistAnalysis instance that will be modified.
    """
    
    from .. import LoadConfig
    import os

    ## @todo place the FileConfigReader functions into its own setup function for other classes to call
    if os.path.exists('CrombieAnalysisConfig.sh'):
        def readMC(config):
            analyzer.ReadMCConfig(config,analyzer.kBackground)
        def readSignal(config):
            analyzer.ReadMCConfig(config,analyzer.kSignal)

        targets = [[analyzer.SetLuminosity, 'CrombieLuminosity'],
                   [analyzer.SetInDirectory, 'CrombieInFilesDir'],
                   [readMC,'CrombieMCConfig'],
                   [readSignal,'CrombieSignalConfig']
                   ]
        for target in targets:
            if os.environ.get(target[1]) == None:
                print 'No ' + target[1] + ' in config'
            else:
                try:
                    target[0](os.environ[target[1]])
                except:
                    target[0](float(os.environ[target[1]]))

