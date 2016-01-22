import ROOT
from CrombieVars import anaSrc

if not 'Corrector' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'Corrector.cc+')

newCorrector = ROOT.Corrector

def MakeCorrector(Name,inExpressions,inCut,correctionFile,correctionHist):
    corrector = newCorrector(Name)
    if type(inExpressions) is list:
        for inExpr in inExpressions:
            corrector.AddInExpression(inExpr)
        ##
    ##
    else:
        corrector.AddInExpression(inExpressions)
    ##
    corrector.SetInCut(inCut)
    corrector.SetCorrectionFile(correctionFile)
    corrector.SetCorrectionHist(correctionHist)
    return corrector
##
