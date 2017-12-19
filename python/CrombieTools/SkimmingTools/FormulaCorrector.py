"""@package CrombieTools.SkimmingTools.Corrector

Submodule of CrombieTools.SkimmingTools
Contains the constructor and default object for FormulaCorrector.

@author Daniel Abercrombie <dabercro@mit.edu>
"""


from .. import Load
from .Corrector import MakeCorrector

newFormulaCorrector  = Load('FormulaCorrector')
"""FormulaCorrector constructor"""

def MakeFormulaCorrector(Name, Formula, inCut):
    """Make a FormulaCorrector object

    @param Name is the name of the branch for this Corrector if the CorrectorApplicator
                is saving all branches.
    @param Formula is the formula to set the branch to
    @param inCut is the cut that a tree must pass in order for the Correction to be applied.
    @returns a FormulaCorrector object.
    """

    corrector = newFormulaCorrector(Name, Formula)
    corrector.SetInCut(inCut)

    return corrector