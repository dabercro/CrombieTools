#!/bin/bash

##
# @todo Clean up/comment Submission scripts <br>
#       Add Error checking for the config (Things should be filled) <br>
#       Check for running jobs and offer to kill them
#

export fresh=$1

if [ ! -f CrombieSlimmingConfig.sh ]
then
    echo "CrombieSlimmingConfig.sh isn't here."
    echo "You should probably go somewhere else."
    exit 1
fi


if [ "$CMSSW_BASE" = "" ]
then
    echo "Require you to be cmsenv somewhere"
    exit 1
fi

echo "=========================================================" >> `pwd`/LxbatchFileChecks.log
echo "Ran CrombieSubmitLxbatch at "`date -u` >> `pwd`/LxbatchFileChecks.log
echo "=========================================================" >> `pwd`/LxbatchFileChecks.log

if [ ! -d bout ]
then
    mkdir bout
fi

source CrombieSlimmingConfig.sh

export haddFile=$CrombieTempDir/myHadd.txt

if [ "$fresh" != "resub" ]
then
    crombie dumpeosfiles eos
    if [ $? -ne 0 ]
    then
        echo "Crashed while dumping file list."
        exit 1
    fi
fi

ranOnFile=0
    
if [ "$fresh" != "hadd" ]
then
    rootNames=`ls $CrombieTempDir/$CrombieFileBase\_*_*.txt | sed 's/.txt//'`
    for outFile in $rootNames
    do
        if [ ! -f $outFile ]
        then
            echo Making: $outFile
            command="bsub -q $CrombieQueue -n $CrombieNBatchProcs -o bout/out.%J crombie dojob $outFile"
            if [ "$fresh" = "test" ]
            then
                echo $command
            else
                $command
            fi
            ranOnFile=1
        fi
    done
else
    echo "Going directly to hadd step."
fi

if [ "$ranOnFile" -eq 0 ]
then
    cat $haddFile | xargs -n2 -P$CrombieNLocalProcs crombie hadd
    echo "All files merged!"
fi
