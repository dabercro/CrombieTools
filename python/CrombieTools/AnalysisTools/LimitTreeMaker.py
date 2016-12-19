"""
@todo Clean up the LimitTreeMaker python file to not depend on these extra variables in cuts.py
"""

import os
from .. import Load, DirFromEnv

newLimitTreeMaker = Load('LimitTreeMaker')


def SetupFromEnv(ltm):
    """A function that sets up the LimitTreeMaker after sourcing a config file

    @param ltm The LimitTreeMaker object to set up
    """
    from ..CommonTools.FileConfigReader import SetupConfigFromEnv, SetFunctionFromEnv

    SetupConfigFromEnv(ltm)

    DirFromEnv('CrombieOutLimitTreeDir')
    SetFunctionFromEnv([
            (ltm.SetOutDirectory, 'CrombieOutLimitTreeDir'),
            ])


def SetCuts(ltm, category):
    from .. import LoadConfig
    cuts = LoadConfig.cuts
    for region in cuts.regions:
        ltm.AddRegion(region,cuts.cut(category, region))
        if region in cuts.additionKeys:
            ltm.AddExceptionDataCut(region, cuts.additions[region][0])
            ltm.AddExceptionWeightBranch(region, cuts.additions[region][1])
