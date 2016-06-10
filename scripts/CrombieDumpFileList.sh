#!/bin/bash

isEOS=$1

source CrombieSlimmingConfig.sh                # Get the slimming configuration

if [ ! -d $CrombieFullDir ]                    # Create directories to hold output
then                                           # of the slimming
    mkdir -p $CrombieFullDir
fi

if [ ! -d $CrombieTempDir ]                    # Create also a temporary directory
then                                           # to hold the lxplus output directly
    mkdir -p $CrombieTempDir
else
    rm $CrombieTempDir/*.txt 2> /dev/null
    if [ "$fresh" = "fresh" ]                  # User can specify a fresh lxbatch run
    then                                       # which clears out temp .root files
        rm $CrombieTempDir/*.root 2> /dev/null # from before
    fi
fi

# Check to see if we will be using multi EOS

if [ -f $CrombieEosDir ]
then
    usingMultiEOS=1
fi

# There are now two instances of EOS that we can store Nero ntuples on

eosCMS=/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select
eosUSER=/afs/cern.ch/project/eos/installation/0.3.84-aquamarine.user/bin/eos.select

eosCommand=$cmsCMS                             # Old behavior was to use a flag to determine which to use
if [ "$CrombieUseCernBox" = "1" ]              # This is unnecessary when specifying multiple EOS directories to run on
then
    eosCommand=$eosUSER
fi

logDir=bout                                    # Make a log directory
if [  ! -d $logDir -a "$isEOS" = "eos" ]       # if running on EOS
then
    echo "Making log output directory as $logDir"
    mkdir $logDir
fi 

if [ "$CrombieDirList" = "" ]                                 # User can specify a list of directories
then                                                          # (samples) to run on
    CrombieDirList=$CrombieTempDir/CrombieDirList.txt         # But if unspecified, we dump all of them
    if [ "$isEOS" = "eos" ]
    then
        if [ "$usingMultiEOS" = "1" ]                         # If using MultiEOS, prepare to read Dir List
        then
            > $CrombieDirList
            eoshost=eoscms.cern.ch
            for line in `cat $CrombieEosDir`
            do
                if [ "${line%%=*}" = "eoshost" ]
                then
                    eoshost=${line##*=}
                    echo "eoshost="$eoshost >> $CrombieDirList
                else
                    echo "eosdir="$line >> $CrombieDirList
                    if [ "${eoshost%%.*}" = "eoscms" ]
                    then
                        eosCommand=$eosCMS
                    elif [ "${eoshost%%.*}" = "eosuser" ]
                    then
                        eosCommand=$eosUSER
                    else
                        echo "Bad host found in config: $eoshost"
                        echo "Please check that."
                        exit 1
                    fi
                    $eosCommand ls $line >> $CrombieDirList
                fi
            done
        else
            $eosCommand ls $CrombieEosDir > $CrombieDirList
        fi
    else
        if [ ! -d $CrombieRegDir ]
        then
            echo "$CrombieRegDir does not seem to exist. Maybe needs mounting."
            exit 1
        fi
        ls $CrombieRegDir > $CrombieDirList
    fi
fi

> $haddFile

ranOnFile=0

lastDir=''

count=0

eoshost=eoscms.cern.ch
if [ "$usingMultiEOS" = "1" ]
then
    eosCommand=$eosCMS
fi

for dir in `cat $CrombieDirList`
do
    if [ "${dir:0:1}" = "#" ]
    then
        continue
    fi

    # Shitty redundant code needs to be fixed

    if [ "${dir%%=*}" = "eoshost" ]
    then
        eoshost=${dir##*=}
        if [ "${eoshost%%.*}" = "eoscms" ]
        then
            eosCommand=$eosCMS
        elif [ "${eoshost%%.*}" = "eosuser" ]
        then
            eosCommand=$eosUSER
        else
            echo "Bad host found in config: $eoshost"
            echo "Please check that."
            exit 1
        fi
        continue
    elif [ "${dir%%=*}" = "eosdir" ]
    then
        CrombieEosDir=${dir##*=}
        continue
    fi

    fileInCount=$CrombieFilesPerJob

    reasonableName="${dir%%/*}"
    reasonableName="${reasonableName%%_Tune*}"
    reasonableName="${reasonableName%%-madgraph*}"

    if [ "$reasonableName" != "$lastDir" ]
    then 
        count=0
        lastDir=$reasonableName
        echo $CrombieFullDir/$CrombieFileBase\_$reasonableName.root $CrombieTempDir/$CrombieFileBase\_$reasonableName"_*.root" >> $haddFile
    fi

    if [ "$isEOS" = "eos" ]
    then
        for inFile in `$eosCommand find $CrombieEosDir/$dir`
        do
            if [ "${inFile##*_}" = "pilot.root" -o "${inFile##*.}" != "root" ]
            then
                continue
            elif echo $inFile | grep "/failed/"
            then
                echo "Found a failed job. Resubmit that. I'm skipping for now."
                continue
            fi
            
            if [ "$fileInCount" -eq "$CrombieFilesPerJob" ]
            then
                fileInCount=0
                count=$((count + 1))
                currentConfig=$CrombieTempDir/$CrombieFileBase\_$reasonableName\_$count.txt
                > $currentConfig
            fi
            if [ "$usingMultiEOS" -eq 1 ]
            then
                # Will eventually make this standard, I think
                echo root://$eoshost/$inFile >> $currentConfig
            else
                echo $inFile >> $currentConfig
            fi
            fileInCount=$((fileInCount + 1))
        done
    else
        for inFile in `find $CrombieRegDir/$dir -name '*.root'`; do
            if [ "${inFile##*_}" = "pilot.root" ]; then
                continue
            fi
            if [ "$fileInCount" -eq "$CrombieFilesPerJob" ]
            then
                fileInCount=0
                count=$((count + 1))
                currentConfig=$CrombieTempDir/$CrombieFileBase\_$reasonableName\_$count.txt
                > $currentConfig
            fi
            echo $inFile $CrombieTempDir/TerminalRunning/$CrombieFileBase\_$reasonableName\_$count\_$indexFiles.root >> $currentConfig
            fileInCount=$((fileInCount + 1))
            indexFiles=$((indexFiles + 1))
        done
    fi
done

exit 0
