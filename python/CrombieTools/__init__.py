""" @package CrombieTools
Base package module of everything used through python.

This contains a useful functions for compiling objects while loading
them into ROOT and for creating directories from environment variables
loaded by submodules. There are also several submodules contained in
the package.
@todo Set up a central FileConfigReader python class that other config readers inherit from
@author Daniel Abercrombie <dabercro@mit.edu>
"""

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import os
import re

__all__ = ['AnalysisTools', 'CommonTools', 'PlotTools', 'SkimmingTools', 'Parallelization']

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
                 'TmvaClassifier' :      ['TreeContainer'],
                 'FitTools' :            ['HistAnalysis'],
                 'TwoScaleFactorCorrector' : ['Corrector'],
                 'FormulaCorrector' :    ['Corrector'],
                 'CorrectorApplicator' : ['Corrector'],
                 'TMVACorrector' :       ['Corrector']
                 }

loadAddOn = '+'
"""Add characters after LoadMacro command. Modified if CrombieTools.debug is imported"""

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

        ROOT.gROOT.LoadMacro(toLoad + loadAddOn)

    if className in ['PlotUtils']:
        return 0

    return getattr(ROOT, className)

def DirFromEnv(envVar, default='tmp_dir'):
    """Creates a directory stored in an environment variable.

    Generally does not need to be called by the user. Multiple submodules make use of this command.
    @param envVar is the name of the environment variable containing a directory name.
    @param default is the default value of the directory location if the environment variable is not filled.
    """

    import LoadConfig

    directory = os.environ.get(envVar, default)

    if not os.path.exists(directory):
        os.makedirs(directory)

def Nminus1Cut(inCut, varToRemove, returnCuts=False):
    """ A function for getting N - 1 plots.

    Given a cutstring and a variable name, all comparison expressions with that variable are removed.
    @param inCut is the full cutstring
    @param varToRemove is the variable to remove from the cutstring (usually a variable being plotted).
    @param returnCuts sets whether to return the cuts values (True) or the new cuts string (False).
    @returns a version of the cutstring without any comparisons to varToRemove or the cuts values.
    @todo I want this to ignore cuts next to an '||'? Or I have the weird '&&' veto thing... That's trickier to fix
    """
    holdCut = str(inCut)
    # First match the expression with the variable. It has to be to the left of the comparison operator.
    # (This is a feature, not a bug. We want to be immune to the automatic n-1 cut sometimes.)
    matches = re.findall(r'(?<!\w)' + re.escape(varToRemove) + r'(?!\w)\s*[=<>]+\s*-?[0-9\.]+', holdCut)
    cutList = []
    for match in matches:
        # Get the number out of the matched expression so that we can draw lines where the cuts should go
        cutMatch = re.search(r'(?<!\w)-?[\d\.]+(?!\w)$', match)
        if cutMatch:
            cutList.append(float(cutMatch.group()))
        holdCut = holdCut.replace(match, '(1)', 1)

    if returnCuts:
        return cutList
    else:
        return holdCut
