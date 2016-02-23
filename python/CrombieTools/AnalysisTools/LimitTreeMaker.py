import os
from .. import Load, DirFromEnv

newLimitTreeMaker = Load('LimitTreeMaker')

def SetupFromEnv(ltm):
    from .. import LoadConfig
    if os.path.exists('CrombieAnalysisConfig.sh'):
        DirFromEnv('CrombieOutLimitTreeDir')
        targets = [[ltm.SetLuminosity, 'CrombieLuminosity'],
                   [ltm.SetInDirectory, 'CrombieInFilesDir'],
                   [ltm.SetOutDirectory, 'CrombieOutLimitTreeDir']
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

def SetCuts(ltm,category):
    from .. import LoadConfig
    cuts = LoadConfig.cuts
    ltm.AddWeightBranch(cuts.defaultMCWeight)
    for region in cuts.regions:
        ltm.AddRegion(region,cuts.cut(category,region))
        if region in cuts.additionKeys:
            ltm.AddExceptionDataCut(region,cuts.additions[region][0])
            ltm.AddExceptionWeightBranch(region,cuts.additions[region][1])
