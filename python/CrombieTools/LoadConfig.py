""" @file LoadConfig.py
Defines the CrombieTools.LoadConfig package
@author Daniel Abercrombie <dabercro@mit.edu>

@package CrombieTools.LoadConfig
Package that loads local [configuration files](@ref envconfig) into environment.
"""

import os
import subprocess


def LoadEnv(configs):
    """Sources bash files and loads the resulting environment into os.environ

    @param configs is a list of file names that should be searched for.
    A string for a single configuration file is also accepted.
    """
    if type(configs) == str:
        LoadEnv([configs])
    elif type(configs) == list:
        for config in configs:
            if os.path.exists(config):
                configContents = subprocess.Popen(['bash','-c','source ' + config + '; env'],
                                                  stdout = subprocess.PIPE)
                for line in configContents.stdout:
                    if type(line) == bytes:
                        (key,sep,value) = line.decode('utf-8').partition('=')
                    elif type(line) == str:
                        (key,sep,value) = line.partition('=')
                    else:
                        print('Not sure how to handle subprocess output. Contact Dan.')
                        break
                    os.environ[str(key)] = str(value).strip('\n')

                configContents.communicate()
    else:
        print 'You passed an invalid argument type to CrombieTools.LoadConfig.LoadEnv()'
        exit()


def LoadModuleFromEnv(EnvVarName):
    """Loads and returns a python file named in the environment as a module.

    @param EnvVarName is an environment variable containing the name of a local .py file.
    @returns the module handler from importing the .py file.
    """
    if not os.environ.get(EnvVarName) == None:
        if os.path.exists(os.environ[EnvVarName]):
            return __import__(os.environ[EnvVarName].strip('.py'), globals(), locals(), [], -1)
    return None


"""List of configuration files that this module tries to load automatically."""
CrombieConfigs = ['CrombieAnalysisConfig.sh','CrombiePlotterConfig.sh','CrombieSlimmingConfig.sh']

LoadEnv(CrombieConfigs)

""" Sub module set by the [$CrombieCutsFile](@ref envconfig) environment variable."""
cuts = LoadModuleFromEnv('CrombieCutsFile')
