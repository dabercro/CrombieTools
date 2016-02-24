import os
from multiprocessing import Process, Queue
from time import time

configProcs = os.environ.get('CrombieNLocalProcs')
configProcs = configProcs or 1

def runParallel(object, functionName, parametersLists, procs=configProcs):
    totStartTime = time()

    if not functionName in dir(object):
        print('You gave an invalid function name!')
        exit(1)

    if not 'Copy' in dir(object):
        print('#########################################')
        print('# Missing copy function in this object. #')
        print('# Cannot run in parallel.               #')
        print('#########################################')
        exit(1)

    if 'GetOutDirectory' in dir(object):
        if not os.path.exists(object.GetOutDirectory()):
            os.makedirs(object.GetOutDirectory())

    def skim(inQueue):
        running = True

        objCopy = object.Copy()
        functionToRun = getattr(objCopy,functionName)

        while running:
            try:
                parameters = inQueue.get(True,1)
                print('About to process ' + str(parameters))
                startTime = time()
                functionToRun(*inFileName)
                print('Finished ' + str(parameters) + ' ... Elapsed time: ' + str(time() - startTime) + ' seconds')
            except Queue.Empty:
                print('Worker finished...')
                running = False

        del objCopy

    theQueue     = Queue()
    theProcesses = []

    for parameters in parametersLists:
        theQueue.put(parameters)

    for worker in range(numMaxProcesses):
        aProcess = Process(target=skim, args=(theQueue,))
        aProcess.start()
        theProcesses.append(aProcess)

    for aProccess in theProcesses:
        aProccess.join()

    print('All done!')
    print()
    print('Total time: ' + str(time() - totStartTime) + ' seconds')
    print()
    

def runOnDirectory(object, procs=configProcs):
    theFiles = []

    if not 'GetInDirectory' in dir(object):
        print('##########################################')
        print('# Missing GetInDirectory in this object. #')
        print('##########################################')
        exit(1)

    inDir = object.GetInDirectory()

    def GetSize(name):
        return os.path.getsize(inDir + '/' + name)

    for inFileName in sorted(os.listdir(inDir), key=GetSize, reverse=True):
        if inFileName.endswith('.root'):
            theFiles.append([inFileName])

    runParallel(object,'RunOnFile',theFiles,procs)
