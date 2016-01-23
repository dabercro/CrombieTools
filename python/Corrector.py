import ROOT
from CrombieVars import anaSrc

if not 'TreeContainer' in dir(ROOT):
    ROOT.gROOT.LoadMacro(skimSrc + 'TreeContainer.cc+')
if not 'PlotHists' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotHists.cc+')
if not 'PlotStack' in dir(ROOT):
    ROOT.gROOT.LoadMacro(plotSrc + 'PlotStack.cc+')
if not 'Corrector' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'Corrector.cc+')
if not 'Reweighter' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'Reweighter.cc+')
if not 'CorrectorApplicator' in dir(ROOT):
    ROOT.gROOT.LoadMacro(anaSrc + 'CorrectorApplicator.cc+')

newCorrector  = ROOT.Corrector
newReweighter = ROOT.Reweighter
newApplicator = ROOT.CorrectorApplicator

def MakeCorrector(Name,inExpressions,inCut,correctionFile,correctionHist):
    corrector = newCorrector(Name)
    if type(inExpressions) is list:
        for inExpr in inExpressions:
            corrector.AddInExpression(inExpr)
        ##
    else:
        corrector.AddInExpression(inExpressions)
    ##
    corrector.SetInCut(inCut)
    corrector.SetCorrectionFile(correctionFile)
    if type(correctionHist) is list:
        corrector.SetCorrectionHist(correctionHist[0],correctionHist[1])
    else:
        corrector.SetCorrectionHist(correctionHist)
    ##
    return corrector
##
