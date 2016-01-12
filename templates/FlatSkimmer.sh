#! /bin/bash

source CrombieSlimmingConfig.sh

CrombieFlatSkimmer --cut '1' --tree 'events' --copy 'htotal' --run 'runNum' --lumi 'lumiNum' --freq 100000 --numproc $CrombieNLocalProcs --indir $CrombieFullDir --outdir $CrombieSkimDir
