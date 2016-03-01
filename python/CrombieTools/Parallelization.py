""" @file Parallelization.py
Defines the CrombieTools.Parallelization package.
@author Daniel Abercrombie <dabercro@mit.edu>

@package CrombieTools.Parallelization
Package for running processes in parallel.
Submodule of CrombieTools.
"""

import os
from multiprocessing import Process, Queue
from time import time
from copy import copy

"""Number of processors from environment"""
DefaultNumProcs = os.environ.get('CrombieNLocalProcs') or 1

def RunParallel(object, functionName, parametersLists, procs=DefaultNumProcs):
    """ Starts parallel processes.

    @param object is an object that can be copied and run independently when
    Python's shallow copy.copy() is called on it.
    @param functionName is the str name of the function that will be run in multiple instances.
    @param parametersLists is a list of lists. Each sublist contains the parameters for the functionName.
    @param procs is the maximum number of processors that will be used.
    """
    totStartTime = time()

    if not functionName in dir(object):
        print('You gave an invalid function name!')
        exit(1)

    if 'GetOutDirectory' in dir(object):
        if not os.path.exists(object.GetOutDirectory()):
            os.makedirs(object.GetOutDirectory())

    def skim(inQueue):
        running = True

        ## @todo Make sure all classes I'm interested in running parallel work as shallow copies. This should be done in tests.
        objCopy = copy(object)
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

    
def RunOnDirectory(object, procs=DefaultNumProcs):
    """ Runs an object over a directory.

    @param object has GetInDirectory() and RunOnFile() function members. 
    This function then runs the object's over all the files in that directory.
    @param procs is the maximum number of processes to start.
    """
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
