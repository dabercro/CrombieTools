#!/bin/bash

outFile=$1

macroDir=$LS_SUBCWD

source $macroDir/CrombieSlimmingConfig.sh

env

eosServer=eoscms
if [ "$CrombieUseCernBox" = "1" ]
then
    eosServer=eosuser
fi

if [ "$CMSSW_BASE" != "" ]
then
    cd $CMSSW_BASE/src
    eval `scram runtime -sh`
    cd -
fi

for file in `cat $macroDir/$CrombieJobScriptList`
do
    if [ "$file" != "${file/\//}" ]
    then
        making=${file%%/${file##*/}}
        if [ ! -d $making ]
        then
            mkdir -p $making
        fi
    fi
    cp $macroDir/$file $file
done

cp "${outFile%.*}".txt . 

echo "Trying to make $outFile"
echo ""

RUNNING=0
NUM=0

$CrombieSlimmerScript

OutputBase="lxbatchTmpOutput"
CommandList="ArgsForThisJob.txt"
echo "" > $CommandList

for file in `cat "${outFile%.*}".txt`
do
    if [ "${file:0:7}" = "root://" ]
    then
        echoCommand="echo $file $OutputBase\_$NUM.root"
    else
        echoCommand="echo root://$eosServer/$file $OutputBase\_$NUM.root"
    fi

    $echoCommand
    $echoCommand >> $CommandList
    NUM=$((NUM + 1))
done

cat $CommandList | xargs -n2 -P$LSB_DJOB_NUMPROC $CrombieSlimmerScript

hadd $OutputBase.root $OutputBase\_*.root

ERRORLOG=$macroDir/LxbatchFileChecks.log
ERRORFOUND=0
FATALERROR=0
for file in `ls $OutputBase*.root`
do
    "$CrombieCheckerScript" $file
    if [ "$?" -ne "0" ]
    then
        if [ "$?" -eq "5" -o "$file" = "$OutputBase.root" ]
        then
            FATALERROR=1
        fi
        ERRORFOUND=1
        echo "Could not find acceptable output in $file" >> $ERRORLOG
        echo "Check output in job bout/out.$LSB_JOBID" >> $ERRORLOG
        echo "" >> $ERRORLOG
    fi
done

if [ "$FATALERROR" -eq "1" ]
then
    echo "FATAL ERROR in bout/out.$LSB_JOBID" >> $ERRORLOG
    echo "" >> $ERRORLOG
    exit 0
fi

echo ""
echo "Copying to $outFile"

cp $OutputBase.root $outFile
