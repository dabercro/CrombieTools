#! /usr/bin/python

from CrombieTools.AnalysisTools.LimitTreeMaker import *
import CrombieTools.LoadConfig
import os

ltm = newLimitTreeMaker()

ltm.AddKeepBranch('example')
ltm.SetOutputWeightBranch('weight')

SetupFromEnv(ltm)

if __name__ == '__main__':
    ltm.SetOutFileName('LimitTree.root')
    SetCuts(ltm,category='example')
    ltm.MakeTrees()
