import ROOT
from CrombieVars import *
from copy import deepcopy
from multiprocessing import Process, Queue

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

applicator    = newApplicator()

def application(inQueue):
    thisApplicator = deepcopy(applicator)
    running = True
    while running:
        try:
            inFileName = inQueue.get(True,1)
            print "About to process " + inFileName
            if not os.path.isfile(inFileName):
                startTime = time()
                applicator.ApplyCorrections(inFileName)
                print "Finished " + inFileName + " ... Elapsed time: " + str(time() - startTime) + " seconds"
            ##
            else:
                print inFileName + " already processed!"
        except:
            print "Worker finished..."
            running = False
        ##
    ##
    del thisApplicator
##

def MakeApplicator(name,saveAll,inputTree,outputTree,reportFreq):
    applicator = newApplicator(name,saveAll)
    applicator.SetInputTreeName(inputTree)
    applicator.SetOutputTreeName(outputTree)
    applicator.SetReportFrequency(reportFreq)
##

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

def AddCorrector(corrector):
    applicator.AddCorrector(corrector)
##

def RunApplicator(directory,nCores):
    theQueue     = Queue()
    theProcesses = []

    for inFileName in sorted(os.listdir(directory)):
        if inFileName.endswith(".root"):
            theQueue.put(directory + "/" + inFileName)
        ##
    ##

    for worker in range(nCores):
        aProcess = Process(target=application, args=(theQueue,))
        aProcess.start()
        theProcesses.append(aProcess)
    ##

    for aProccess in theProcesses:
        aProccess.join()
    ##

    print "All done!"
##
