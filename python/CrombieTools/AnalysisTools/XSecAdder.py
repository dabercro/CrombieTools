"""@package CrombieTools.SkimmingTools.XSecAdder
Holds constructor and default XSecAdder object along with convenience function.
@author Daniel Abercrombie <dabercro@mit.edu>
"""
from .. import Load

newXSecAdder = Load('XSecAdder')
xSecAdder    = newXSecAdder()

def RunXSecAdder(branchName='XSecWeight', inTree='events', outTree='events', readSignal=False, adder=xSecAdder):
    from .. import LoadConfig
    import os

    adder.SetBranchName(branchName)
    adder.SetInTreeName(inTree)
    adder.SetOutTreeName(outTree)

    adder.SetInDirectory(os.environ['CrombieInFilesDir'])
    adder.ReadMCConfig(os.environ['CrombieMCConfig'])

    if readSignal:
        adder.ReadMCConfig(os.environ['CrombieSignalConfig'])

    for key in os.environ.keys():
        if key.startswith('CrombieExcept_'):
            adder.ReadMCConfig(os.environ[key])
            
    adder.AddXSecs()
