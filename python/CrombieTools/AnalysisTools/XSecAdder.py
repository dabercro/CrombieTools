"""@package CrombieTools.SkimmingTools.XSecAdder
Holds constructor and default XSecAdder object along with convenience function.
@author Daniel Abercrombie <dabercro@mit.edu>
"""
from .. import Load

newXSecAdder = Load('XSecAdder')
xSecAdder    = newXSecAdder()

def RunXSecAdder(branchName='XSecWeight', exceptName = '', inTree='events', outTree='events', readSignal=False, adder=xSecAdder):
    """Add a branch filled with floats proportional to the cross section of a process to a tree

    @param branchName is the name of the branch to be added
    @param exceptName is the name of an exception config to also read in
    @param inTree is the name of the tree to get the number of events
    @param outTree is the tree to write the branch to
    @param readSignal causes the CrombieSignalConfig to be read too
    @param adder is the instances of XSecAdder that is used
    """

    from .. import LoadConfig
    import os

    adder.SetBranchName(branchName)
    adder.SetInTreeName(inTree)
    adder.SetOutTreeName(outTree)

    adder.SetInDirectory(os.environ['CrombieInFilesDir'])
    adder.ReadMCConfig(os.environ['CrombieMCConfig'])
    if exceptName != '':
        adder.ReadMCConfig(os.environ['CrombieExcept_' + exceptName])
        
    if readSignal:
        adder.ReadMCConfig(os.environ['CrombieSignalConfig'])

    for key in os.environ.keys():
        if key.startswith('CrombieExcept_'):
            adder.ReadMCConfig(os.environ[key])
            
    adder.AddXSecs()
