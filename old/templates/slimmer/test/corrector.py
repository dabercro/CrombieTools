#!/usr/bin/env python

from CrombieTools.SkimmingTools.Corrector import *
import CrombieTools.LoadConfig
from CrombieTools.Parallelization import RunOnDirectory
import os

applicator = MakeApplicator('allWeights', True, 'test', 'test', 100000)
corrector = MakeCorrector('fixWeight', 'example', '0', 'testHist.root', 'hist')
applicator.AddCorrector(corrector)
applicator.AddFactorToMerge('weight')

applicator.SetInDirectory(os.environ['CrombieFullDir'])

corrector.SetInCut('1')
applicator.ApplyCorrections('test_MC2.root')

print('Now running in parallel!')

corrector.SetInCut('0')
RunOnDirectory(applicator)
