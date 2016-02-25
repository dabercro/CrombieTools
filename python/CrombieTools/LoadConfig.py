import os
import subprocess

CrombieConfigs = ['CrombieSlimmingConfig.sh','CrombieAnalysisConfig.sh']

for config in CrombieConfigs:
    if os.path.exists(config):
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

possibleImports = ['CrombieCutsFile']
for toImport in possibleImports:
    if not os.environ.get(toImport) == None:
        if os.path.exists(os.environ[toImport]):
            cuts = __import__(os.environ[toImport].strip('.py'), globals(), locals(), [], -1)
