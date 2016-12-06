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


def SetupConfigFromEnv(obj):
    """Sets up FileConfigReaders to read the config file automatically.
    It uses the environment variable $DEBUG to set the verbosity level of a class.
    The default is 1, with a maximum of 3 for real debugging output.

    @param obj is the python object that inherits from FileConfigReader.
    """

    from .. import LoadConfig

    obj.SetDebugLevel(int(os.environ.get('DEBUG', 1)))

    if os.path.exists('CrombieAnalysisConfig.sh') or os.path.exists('CrombiePlotterConfig.sh'):
        def readMC(config):
            obj.ReadMCConfig(config, obj.kBackground)
        def readSignal(config):
            obj.ReadMCConfig(config, obj.kSignal)

        SetFunctionFromEnv([
                (obj.SetInDirectory, 'CrombieInFilesDir'),
                (obj.SetLuminosity, 'CrombieLuminosity'),
                (readMC, 'CrombieMCConfig'),
                (readSignal, 'CrombieSignalConfig')
                ])

        obj.SetFileType(obj.kBackground)

    else:
        print 'Could not find CrombieAnalysisConfig.sh or CrombiePlotterConfig.sh'
        exit(10)
