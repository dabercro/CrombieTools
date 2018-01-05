"""@package CrombieTools.CommonTools.FileConfigReader
Holds centralized SetupConfigFromEnv functions.
@author Daniel Abercrombie <dabercro@mit.edu>
"""

import os

from .. import LoadConfig

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
                try:
                    func(float(os.environ[envname]))
                except:
                    func(int(os.environ[envname]))


def SetupConfigFromEnv(obj):
    """Sets up FileConfigReaders to read the config file automatically.
    It uses the environment variable $DEBUG to set the verbosity level of a class.
    The default is 1, with a maximum of 3 for real debugging output.

    @param obj is the python object that inherits from FileConfigReader.
    """

    level = int(os.environ.get('DEBUG', 1))
    if os.environ.get('DEBUG'):
        print 'Setting debug level to %i' % level

    obj.SetDebugLevel(level)

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
