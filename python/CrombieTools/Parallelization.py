import os
from multiprocessing import Process, Queue
from time import time

configProcs = os.environ.get('CrombieNLocalProcs')
configProcs = configProcs or 1

def runParallel(object, procs=configProcs):
    totStartTime = time()

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

        while running:
            try:
                inFileName = inQueue.get(True,1)
                print('About to process ' + inFileName)
                startTime = time()
                skimmer.Slim(inFileName)
                print('Finished ' + inFileName + ' ... Elapsed time: ' + str(time() - startTime) + ' seconds')
            except:
                print('Worker finished...')
                running = False

        del objCopy

    theQueue     = Queue()
    theProcesses = []

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
            theQueue.put(inFileName)

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
