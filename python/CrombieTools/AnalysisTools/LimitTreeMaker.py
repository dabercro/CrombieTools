import os
from .. import Load, DirFromEnv

newLimitTreeMaker = Load('LimitTreeMaker')

def SetupFromEnv(ltm):
    from .. import LoadConfig
    if os.path.exists('CrombieAnalysisConfig.sh'):
        DirFromEnv('CrombieOutLimitTreeDir')
        def readMC(config):
            ltm.ReadMCConfig(config,ltm.kBackground)
        def readSignal(config):
            ltm.ReadMCConfig(config,ltm.kSignal)

        targets = [[ltm.SetLuminosity, 'CrombieLuminosity'],
                   [ltm.SetInDirectory, 'CrombieInFilesDir'],
                   [ltm.SetOutDirectory, 'CrombieOutLimitTreeDir'],
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

        for region in LoadConfig.cuts.regions:
            if os.environ.get('CrombieExcept_' + region) != None:
                ltm.ReadExceptionFile(os.environ['CrombieExcept_' + region],region)

    else:
        print 'Could not find CrombieAnalysisConfig.sh'

def SetCuts(ltm,category):
    from .. import LoadConfig
    cuts = LoadConfig.cuts
    ltm.AddWeightBranch(cuts.defaultMCWeight)
    for region in cuts.regions:
        ltm.AddRegion(region,cuts.cut(category,region))
        if region in cuts.additionKeys:
            ltm.AddExceptionDataCut(region,cuts.additions[region][0])
            ltm.AddExceptionWeightBranch(region,cuts.additions[region][1])
