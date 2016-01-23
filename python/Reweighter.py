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

newReweighter = ROOT.Reweighter

def MakeReweighter(Name,inExpressions,inCut,correctionFile,correctionHist):
    reweighter = newReweighter(Name)
    if type(inExpressions) is list:
        for inExpr in inExpressions:
            reweighter.AddInExpression(inExpr)
        ##
    ##
    else:
        reweighter.AddInExpression(inExpressions)
    ##
    reweighter.SetInCut(inCut)
    reweighter.SetCorrectionFile(correctionFile)
    reweighter.SetCorrectionHist(correctionHist)
    return reweighter
##
