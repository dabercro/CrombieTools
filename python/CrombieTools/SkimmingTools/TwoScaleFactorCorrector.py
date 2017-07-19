"""@package CrombieTools.SkimmingTools.Corrector

Submodule of CrombieTools.SkimmingTools
Contains the constructor and default object for TwoScaleFactorCorrector.

@author Daniel Abercrombie <dabercro@mit.edu>
"""


from .. import Load
from .Corrector import MakeCorrector

newTwoScaleFactorCorrector  = Load('TwoScaleFactorCorrector')
"""TwoScaleFactorCorrector constructor"""

def MakeTwoScaleFactorCorrector(Name, inCut,
                                leg1_tightcut, leg2_tightcut,
                                leg1_loose_expr, leg1_tight_expr,
                                leg2_loose_expr, leg2_tight_expr,
                                looseFile, tightFile, looseHist, tightHist):

    """Make a TwoScaleFactorCorrector object

    @param Name is the name of the branch for this Corrector if the CorrectorApplicator
                is saving all branches.
    @param inCut is the cut that a tree must pass in order for the Correction to be applied.
    @param leg*_tightcut is the cut that must be passed in order to read from the tight histogram for that leg
    @param leg*_*_expr is the expression to evaluate before reading from the Correction histogram.
                  If the histogram is multi-dimensional, a list can be passed to this parameter.
    @param looseFile is the name of the file holding the correction histogram
    @param tightFile is the name of the file holding the correction histogram
    @param looseHist is the name of the histograms to apply corrections with.
                     Two histograms from the same file can be divided by passing a list
                     of [numerator_name, denominator_name].
    @param tightHist is the name of the histograms to apply corrections with.
                     Two histograms from the same file can be divided by passing a list
                     of [numerator_name, denominator_name].
    @returns a filled Corrector object.
    """

    corrector = newTwoScaleFactorCorrector(
        Name,
        MakeCorrector('', leg1_loose_expr, '1', looseFile, looseHist),
        MakeCorrector('', leg1_tight_expr, leg1_tightcut, tightFile, tightHist),
        MakeCorrector('', leg2_loose_expr, '1', looseFile, looseHist),
        MakeCorrector('', leg2_tight_expr, leg2_tightcut, tightFile, tightHist)
    )

    corrector.SetInCut(inCut)

    return corrector
