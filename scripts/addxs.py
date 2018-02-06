#! /usr/bin/env python

import sys

from CrombieTools.AnalysisTools.XSecAdder import RunXSecAdder, xSecAdder
from CrombieTools.CommonTools.FileConfigReader import SetupConfigFromEnv, SetFunctionFromEnv

if __name__ == '__main__':

    if 'skip' in sys.argv:
        xSecAdder.SetKeepAllFiles(False)

    SetupConfigFromEnv(xSecAdder)
    SetFunctionFromEnv([(xSecAdder.SetNumThreads, 'CrombieNLocalProcs')])

    RunXSecAdder()
