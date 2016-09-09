"""@package CrombieTools.SkimmingTools.Corrector

Submodule of CrombieTools.SkimmingTools
Contains the constructor and default object for Corrector and CorrectorApplicator.
A couple of convenience functions are also used to create these objects with 
some parameters filled.

@author Daniel Abercrombie <dabercro@mit.edu>
"""


from .. import Load


newCorrector  = Load('Corrector')
"""Corrector constructor"""
newApplicator = Load('CorrectorApplicator')
"""CorrectorApplicator constructor"""


def MakeApplicator(name, saveAll, reportFreq=100000, inputTree='events', outputTree='events'):
    """Make a CorrectorApplicator with parameters filled

    @param name is a string that names the branch to merge all of the correction factors into.
    @param saveAll is a bool to determine whether or not to save each correction factor
                   in their own branch.
    @param reportFreq is the number of events to pass between reporting progress.
    @param inputTree is the name of the tree to read as input.
    @param outputTree is the name of the tree to place the output branches.
    @returns a CorrectorApplicator object.
    """
    applicator = newApplicator(name, saveAll)
    applicator.SetInputTreeName(inputTree)
    applicator.SetOutputTreeName(outputTree)
    applicator.SetReportFrequency(reportFreq)
    return applicator


def MakeCorrector(Name, inExpressions, inCut, correctionFile, correctionHist):
    """Make a Corrector object

    @param Name is the name of the branch for this Corrector if the CorrectorApplicator 
                is saving all branches.
    @param inExpressions is the expression to evaluate before reading from the Correction histogram.
                         If the histogram is multi-dimensional, a list can be passed to this parameter.
    @param inCut is the cut that a tree must pass in order for the Correction to be applied.
    @param correctionFile is the name of the file holding the correction histogram
    @param correctionHist is the name of the histograms to apply corrections with.
                          Two histograms from the same file can be divided by passing a list
                          of [numerator_name, denominator_name].
    @returns a filled Corrector object.
    """
    corrector = newCorrector(Name)
    if type(inExpressions) is list:
        for inExpr in inExpressions:
            corrector.AddInExpression(inExpr)
    else:
        corrector.AddInExpression(inExpressions)

    corrector.SetInCut(inCut)
    corrector.SetCorrectionFile(correctionFile)
    if type(correctionHist) is list:
        corrector.SetCorrectionHist(correctionHist[0], correctionHist[1])
    else:
        corrector.SetCorrectionHist(correctionHist)

    return corrector
