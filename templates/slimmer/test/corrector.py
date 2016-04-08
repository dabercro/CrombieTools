#!/usr/bin/env python

from CrombieTools.SkimmingTools.Corrector import *
import CrombieTools.LoadConfig
import os

applicator = MakeApplicator('allWeights',True,'test','test',5000)
corrector = MakeCorrector('fixWeight','example','0','testHist.root','hist')
applicator.AddCorrector(corrector)
applicator.AddFactorToMerge('weight')

# I really have to fix this bit
for fileName in os.listdir(os.environ['CrombieFullDir']):
    if not '.root' in fileName:
        continue
    # Though setting this cut in my general parallelization tool will be tricky
    if fileName == 'test_MC2.root':
        corrector.SetInCut('1')
    else:
        corrector.SetInCut('0')
    applicator.ApplyCorrections(os.environ['CrombieFullDir'] + '/' + fileName)
