""" @file LoadConfig.py
Defines the CrombieTools.LoadConfig package
@author Daniel Abercrombie <dabercro@mit.edu>

@package CrombieTools.LoadConfig
Package for running processes in parallel.
Submodule of CrombieTools.
"""

import os
import subprocess

## List of possible (environment configuration)[@ref md_docs_ENVCONFIG] files to look for.
CrombieConfigs = ['CrombieSlimmingConfig.sh','CrombieAnalysisConfig.sh']

for config in CrombieConfigs:
    if os.path.exists(config):
        ## @private
        configContents = subprocess.Popen(['bash','-c','source ' + config + '; env'],stdout = subprocess.PIPE)
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

## Sub module set by the (CrombieCutsFile)[@ref md_docs_ENVCONFIG] environment variable.
cuts = None
## @private List of environment variables containing file names to load into submodules.
possibleImports = [['CrombieCutsFile',cuts]]
for toImport in possibleImports:
    if not os.environ.get(toImport[0]) == None:
        if os.path.exists(os.environ[toImport[0]]):
            toImport[1] = __import__(os.environ[toImport[0]].strip('.py'), globals(), locals(), [], -1)
