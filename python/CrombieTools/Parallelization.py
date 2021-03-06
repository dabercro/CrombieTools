""" @file Parallelization.py
Defines the CrombieTools.Parallelization package.
@author Daniel Abercrombie <dabercro@mit.edu>

@package CrombieTools.Parallelization
Package for running processes in parallel.
Submodule of CrombieTools.
"""

import os
from multiprocessing import Process, Queue
from Queue import Empty
from time import time

"""Number of processors from environment"""
DefaultNumProcs = int(os.environ.get('CrombieNLocalProcs') or 1)

def RunParallel(objectToRun, functionName, parametersLists, procs=DefaultNumProcs, printing=True, doCopy=True):
    """ Starts parallel processes.

    @param objectToRun is an objectToRun that can be copied and run independently when
    it's own Copy() function is called.
    @param functionName is the str name of the function that will be run in multiple instances.
    @param parametersLists is a list of lists. Each sublist contains the parameters for the functionName.
    @param procs is the maximum number of processors that will be used.
    @param printing tells which files are about to be processed and how long they took if True.
    @param doCopy copies the object that the function should be run over
    """

    totStartTime = time()

    if 'Copy' not in dir(objectToRun) and doCopy:
        print('Object not copyable.')
        print('Exiting...')
        exit(1)

    if not functionName in dir(objectToRun):
        print('You gave an invalid function name!')
        exit(1)

    if 'GetOutDirectory' in dir(objectToRun):
        outDir = str(objectToRun.GetOutDirectory())
        if not os.path.exists(outDir):
            os.makedirs(outDir)

    def runOnQueue(inQueue):
        running = True

        objCopy = objectToRun.Copy() if doCopy else objectToRun
        functionToRun = getattr(objCopy, functionName)

        while running:
            try:
                parameters = inQueue.get(True, 1)
                if printing:
                    print('About to process ' + str(parameters))

                startTime = time()
                functionToRun(*parameters)
                if printing:
                    print('Finished ' + str(parameters) + ' ... Elapsed time: ' + str(time() - startTime) + ' seconds')

            except Empty:
                if printing:
                    print('Worker finished...')

                running = False

        if (doCopy):
            del objCopy

    if printing:
        print('About to use {0} processers.'.format(procs))

    theQueue     = Queue()
    theProcesses = []

    for parameters in parametersLists:
        theQueue.put(parameters)

    for worker in range(procs):
        aProcess = Process(target=runOnQueue, args=(theQueue,))
        aProcess.start()
        theProcesses.append(aProcess)

    for aProccess in theProcesses:
        aProccess.join()

    print('All done!\n')
    print('Total jobs: ' + str(len(parametersLists)))
    print('Total time: ' + str(time() - totStartTime) + ' seconds\n')


def RunOnDirectory(objectToRun, procs=DefaultNumProcs, printing=True):
    """ Runs an objectToRun over a directory.

    @param objectToRun has GetInDirectory() and RunOnFile() function members.
    This function then runs the objectToRun's over all the files in that directory.
    @param procs is the maximum number of processes to start.
    @param printing tells which files are about to be processed and how long they took if True.
    """
    theFiles = []

    if not 'GetInDirectory' in dir(objectToRun):
        print('##########################################')
        print('# Missing GetInDirectory in this object. #')
        print('##########################################')
        exit(1)

    inDir = str(objectToRun.GetInDirectory())

    outDir = None
    if 'GetOutDirectory' in dir(objectToRun):
        outDir = str(objectToRun.GetOutDirectory())

    def GetSize(name):
        return os.path.getsize(inDir + name)

    for inFileName in sorted(os.listdir(inDir), key=GetSize, reverse=True):
        if inFileName.endswith('.root'):
            if outDir and os.path.exists(outDir + inFileName):
                print(outDir + inFileName + ' already exists !!! ... Skipping.')
                continue
            theFiles.append([inFileName])

    RunParallel(objectToRun, 'RunOnFile', theFiles, procs, printing)
