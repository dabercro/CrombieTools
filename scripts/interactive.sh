#! /bin/bash

workdir=/scratch5/$USER/interactive/$(date +'%y%m%d_%H%M%S')

if [ -d $workdir ]
then
    echo "$workdir seems to exist"
    exit 1
fi

mkdir -p $workdir

cp $CMSSW_BASE/condor.tgz $workdir

echo "cd $workdir"
echo "$CROMBIEPATH/old/SubmitTools/condor/run.sh $1"
