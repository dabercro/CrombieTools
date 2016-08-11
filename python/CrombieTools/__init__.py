""" @package CrombieTools
Base package module of everything used through python.

This contains a useful functions for compiling objects while loading 
them into ROOT and for creating directories from environment variables 
loaded by submodules. There are also several submodules contained in
the package.
@author Daniel Abercrombie <dabercro@mit.edu>
"""

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import os
import re

__all__ = ['AnalysisTools','CommonTools','PlotTools','SkimmingTools','Parallelization']

ROOT.gROOT.SetBatch(True)

"""Location of CrombieTools package, based on environment."""
CrombieDir = os.environ['CROMBIEPATH']

if CrombieDir == '':
    print('#########################################################')
    print('#                                                       #')
    print('#   CROMBIEPATH variable is not set!                    #')
    print('#   You have not successfully installed CrombieTools.   #')
    print('#   Run CrombieTools/install.sh or read                 #')
    print('#   CrombieTools/README.md to troubleshoot.             #')
    print('#                                                       #')
    print('#########################################################')
    exit(1)

for package in __all__:
    if os.path.exists(CrombieDir + '/' + package + '/interface'):
        ROOT.gSystem.AddIncludePath('-I' + CrombieDir + '/' + package + '/interface/')

ROOT.gROOT.LoadMacro(CrombieDir + '/PlotTools/interface/KinematicFunctions.h')

"""Key -- Class to load : Value -- List of classes that must be loaded first."""
dependencies = { 'FlatSkimmer' :         ['GoodLumiFilter'],
                 'PlotFitParameters' :   ['Plot2D'],
                 'CorrectorApplicator' : ['Corrector'],
                 'TmvaClassifier' :      ['TreeContainer','PlotHists'],
                 'PlotROC' :             ['PlotHists'],
                 'PlotStack' :           ['PlotHists'],
                 'HistAnalysis' :        ['PlotHists'],
                 'XSecAdder' :           ['PlotHists'],
                 }

def Load(className):
    """ Loads a class from Crombie Tools into ROOT.

    @param className is the name of a class in the Crombie Tools package.
    @returns the function pointer for the constructor, except in the case of PlotUtils.
    In that case, 0 is returned. It would be much better to import what
    you need from CrombieTools.PlotTools.PlotUtils.
    """
    if not className in dir(ROOT):
        if type(dependencies.get(className)) is list:
            for depend in dependencies[className]:
                Load(depend)

        toLoad = ''
        for package in __all__:
            checkFile = CrombieDir + '/' + package + '/src/' + className + '.cc'
            if os.path.exists(checkFile):
                toLoad = checkFile
                break

        if toLoad == '':
            print('')
            print('Can\'t find class: ' + className)
            print('in CrombieTools... Exiting.')
            exit(1)

        ROOT.gROOT.LoadMacro(toLoad + '+')

    if className in ['PlotUtils']:
        return 0

    return getattr(ROOT,className)

def DirFromEnv(envVar):
    """ Creates a directory stored in an environment variable.

    Generally does not need to be called by the user. Multiple submodules make use of this command.
    @param envVar is the name of the environment variable containing a directory name.
    """
    if type(os.environ.get(envVar)) == str:
        if not os.path.exists(os.environ[envVar]):
            os.makedirs(os.environ[envVar])

def Nminus1Cut(inCut,varToRemove,returnCuts=False):
    """ A function for getting N - 1 plots.

    Given a cutstring and a variable name, all comparison expressions with that variable are removed.
    @param inCut is the full cutstring
    @param varToRemove is the variable to remove from the cutstring (usually a variable being plotted).
    @param returnCuts sets whether to return the cuts values (True) or the new cuts string (False).
    @returns a version of the cutstring without any comparisons to varToRemove or the cuts values.
    @todo I want this to ignore cuts next to an '||'? Or I have the weird '&&' veto thing... That's trickier to fix
    """
    holdCut = str(inCut)
    matches = re.findall(r'[0-9\.]*\s*[=<>]*\s*(?<!\w)' + varToRemove + '(?!\w)\s*[=<>]*\s*[0-9\.]*',holdCut)
    cutList = []
    for match in matches:
        cutMatch = re.search(r'(?<!\w)[\d\.]+(?!\w)', match)
        if cutMatch:
            cutList.append(float(cutMatch.group()))
        holdCut = holdCut.replace(match,'(1)',1)

    if returnCuts:
        return cutList
    else:
        return holdCut
