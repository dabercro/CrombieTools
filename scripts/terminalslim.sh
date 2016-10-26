#!/bin/bash

export fresh=$1

source CrombieSlimmingConfig.sh

export haddFile=$CrombieTempDir/myHadd.txt

if [ "$fresh" = "fresh" ]
then
    if [ -d $CrombieTempDir ]
    then
        rm $CrombieTempDir/*.root 2> /dev/null
        if [ -d $CrombieTempDir/TerminalRunning ]
        then
            rm $CrombieTempDir/TerminalRunning/* 2> /dev/null
        fi
    fi
fi

running=0

if [ "$fresh" != "hadd" ]
then
    countFiles=`ls $CrombieTempDir/TerminalRunning/*.txt.running  2> /dev/null | wc -l`

    if [ ! -d $CrombieTempDir/TerminalRunning ]
    then
        mkdir -p $CrombieTempDir/TerminalRunning
    fi

    if [ "$countFiles" -eq "0" -a "$fresh" != "resub" ]
    then
        crombie dumpfilelist
        if [ "$?" -ne "0" ]
        then
            echo "Something went wrong with File Dump. Exiting..."
            exit 1
        fi
        ./$CrombieSlimmerScript compile
    fi

    for inFile in `ls $CrombieTempDir/$CrombieFileBase\_*.txt 2> /dev/null`
    do
        inFile="${inFile##*/}"
        inRoot="${inFile%%.txt}"
        if [ -f $CrombieTempDir/$inRoot.root ]
        then
            continue
        elif [ ! -f $CrombieTempDir/TerminalRunning/$inFile.running ]
        then
            touch $CrombieTempDir/TerminalRunning/$inFile.running
            echo "Running on "$inFile
            cat $CrombieTempDir/$inFile | xargs -n2 -P$CrombieNLocalProcs ./$CrombieSlimmerScript
            ERRORFOUND=0
            for file in `ls $CrombieTempDir/TerminalRunning/$inRoot\_*.root`
            do
                $CrombieCheckerScript $file
                if [ "$?" -eq "1" ]
                then
                    echo "Error found in $file"
                    ERRORFOUND=1
                fi
            done
            if [ "$ERRORFOUND" -eq "0" ]
            then
                hadd $CrombieTempDir/$inRoot.root $CrombieTempDir/TerminalRunning/$inRoot\_*.root
            else
                echo "Not hadding $CrombieTempDir/$inRoot.root"
            fi
            rm $CrombieTempDir/TerminalRunning/$inRoot\_*.root
            rm $CrombieTempDir/TerminalRunning/$inFile.running
        fi
        running=1
    done
else
    echo "Going directly to hadd step."
fi

if [ "$running" -eq 0 ]
then
    if [ -d $CrombieTempDir/TerminalRunning ]
    then
        rmdir $CrombieTempDir/TerminalRunning
    fi
    cat $haddFile | xargs -n2 -P$CrombieNLocalProcs crombie hadd
fi
