#!/bin/bash

fresh=$1

source CrombieSlimmingConfig.sh

if [ "$fresh" = "fresh" ]
then
    rm $CrombieSkimDir/*.root 2> /dev/null
fi

crombie skim --cut 'exampleDisc1 > 0.5' --tree 'test' --copy 'htotal' --run 'runNum' --lumi 'lumiNum' --freq 5000 --numproc $CrombieNLocalProcs --indir $CrombieFullDir --outdir $CrombieSkimDir --json $CrombieGoodRuns
