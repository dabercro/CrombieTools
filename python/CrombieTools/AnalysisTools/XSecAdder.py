"""@package CrombieTools.AnalysisTools.XSecAdder
Holds constructor and default XSecAdder object along with convenience function.
@author Daniel Abercrombie <dabercro@mit.edu>
"""
from .. import Load

newXSecAdder = Load('XSecAdder')
xSecAdder    = newXSecAdder()

def RunXSecAdder(branchName='XSecWeight', inTree='events', outTree='events', adder=xSecAdder):
    """Add a branch filled with floats proportional to the cross section of a process to a tree

    @param branchName is the name of the branch to be added
    @param inTree is the name of the tree to get the number of events
    @param outTree is the tree to write the branch to
    @param adder is the instances of XSecAdder that is used
    """

    from .. import LoadConfig
    import os

    adder.SetBranchName(branchName)
    adder.SetInTreeName(inTree)
    adder.SetOutTreeName(outTree)

    adder.AddXSecs()
