from .. import Load

newCorrector  = Load('Corrector')
newApplicator = Load('CorrectorApplicator')

def MakeApplicator(name,saveAll,inputTree,outputTree,reportFreq):
    applicator = newApplicator(name,saveAll)
    applicator.SetInputTreeName(inputTree)
    applicator.SetOutputTreeName(outputTree)
    applicator.SetReportFrequency(reportFreq)
    return applicator

def MakeCorrector(Name,inExpressions,inCut,correctionFile,correctionHist):
    corrector = newCorrector(Name)
    if type(inExpressions) is list:
        for inExpr in inExpressions:
            corrector.AddInExpression(inExpr)
    else:
        corrector.AddInExpression(inExpressions)

    corrector.SetInCut(inCut)
    corrector.SetCorrectionFile(correctionFile)
    if type(correctionHist) is list:
        corrector.SetCorrectionHist(correctionHist[0],correctionHist[1])
    else:
        corrector.SetCorrectionHist(correctionHist)

    return corrector
