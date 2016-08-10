"""@package CrombieTools.CommonTools.FileConfigReader
Holds centralized SetupConfigFromEnv functions.
@author Daniel Abercrombie <dabercro@mit.edu>
"""

import os
from ..LoadConfig import SetFunctionFromEnv


def SetupConfigFromEnv(object):
    if os.path.exists('CrombieAnalysisConfig.sh') or os.path.exists('CrombiePlotterConfig.sh'):
        def readMC(config):
            object.ReadMCConfig(config, object.kBackground)
        def readSignal(config):
            object.ReadMCConfig(config, object.kSignal)

        SetFunctionFromEnv([
                (object.SetInDirectory, 'CrombieInFilesDir'),
                (readMC, 'CrombieMCConfig'),
                (readSignal, 'CrombieSignalConfig')
                ])
    else:
        print 'Could not find CrombieAnalysisConfig.sh or CrombiePlotterConfig.sh'
        exit(10)
