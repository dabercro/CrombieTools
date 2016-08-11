"""@package CrombieTools.CommonTools.FileConfigReader
Holds centralized SetupConfigFromEnv functions.
@author Daniel Abercrombie <dabercro@mit.edu>
"""

import os


def SetFunctionFromEnv(targets):
    """Calls functions using environment variables

    @param targets is a list of tuples, each containing a function and
                   the environment variable to use as an argument
    """
    for func, envname in targets:
        if os.environ.get(envname) == None:
            print 'Cannot find ' + envname + ' in config'
        else:
            try:
                func(os.environ[envname])
            except:
                func(float(os.environ[envname]))


def SetupConfigFromEnv(object):
    """Sets up FileConfigReaders to read the config file automatically

    @param object is the python object that inherits from FileConfigReader.
    """

    from .. import LoadConfig

    if os.path.exists('CrombieAnalysisConfig.sh') or os.path.exists('CrombiePlotterConfig.sh'):
        def readMC(config):
            object.ReadMCConfig(config, object.kBackground)
        def readSignal(config):
            object.ReadMCConfig(config, object.kSignal)

        SetFunctionFromEnv([
                (object.SetInDirectory, 'CrombieInFilesDir'),
                (object.SetLuminosity, 'CrombieLuminosity'),
                (readMC, 'CrombieMCConfig'),
                (readSignal, 'CrombieSignalConfig')
                ])
    else:
        print 'Could not find CrombieAnalysisConfig.sh or CrombiePlotterConfig.sh'
        exit(10)
