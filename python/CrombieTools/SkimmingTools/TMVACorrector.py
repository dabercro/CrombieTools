"""@package CrombieTools.SkimmingTools.TMVACorrector

Submodule of CrombieTools.SkimmingTools
Contains the constructor and default object for TMVACorrector.

@author Daniel Abercrombie <dabercro@mit.edu>
"""


from .. import Load

newTMVACorrector = Load('TMVACorrector')
"""TMVACorrector constructor"""

def MakeTMVACorrector(Name, Weights, Config, inCut='1', matchName=''):
    """Make a TMVACorrector object

    @param Name is the name of the branch for this Corrector if the CorrectorApplicator
                is saving all branches.
    @param Weights is the name of the weights file to read
    @param Config is the name of the file that contains the
                  variable names and branches for the reader to use
    @param inCut is the cut that a tree must pass in order for the Correction to be applied.
    @param matchName is the regex expression that must be matched 
                     for the correction to be applied to the file
    @returns a TMVACorrector object.
    """

    corrector = newTMVACorrector(Name, Weights)
    corrector.ReadVarConfig(Config)
    corrector.SetInCut(inCut)
    corrector.SetMatchFileName(matchName)

    return corrector
