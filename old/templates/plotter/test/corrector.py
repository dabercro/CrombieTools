#!/usr/bin/env python

from CrombieTools.SkimmingTools.Corrector import *
import CrombieTools.LoadConfig
from CrombieTools.Parallelization import RunOnDirectory
import os

applicator = MakeApplicator('', True, 'test', 'test', 100000)
corrector = MakeCorrector('reweight', 'example', '0', 'example_reweight.root', 'weight')
applicator.AddCorrector(corrector)

applicator.SetInDirectory(os.environ['CrombieFullDir'])

corrector.SetInCut('1')
applicator.ApplyCorrections('test_MC1.root')

print('Now running in parallel!')

corrector.SetInCut('0')
RunOnDirectory(applicator)
