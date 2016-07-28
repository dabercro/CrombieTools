#!/bin/bash

## @file dumpeosfiles.sh
#  This is the file that creates the input files for running slimmers over EOS
#  or other directories.
#  @todo Clean me and give user more power for picking directories when multiple present
#  @author Daniel Abercrombie <dabercro@mit.edu>

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
    CrombieEosDir=`cat $CrombieEosDir`         # Set list of EOS directories
    usingMultiEOS=1
fi

# There are now two instances of EOS that we can store Nero ntuples on

eosCMS=/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select
eosUSER=/afs/cern.ch/project/eos/installation/0.3.84-aquamarine.user/bin/eos.select

logDir=bout                                    # Make a log directory
if [  ! -d $logDir -a "$isEOS" = "eos" ]       # if running on EOS
then
    echo "Making log output directory as $logDir"
    mkdir $logDir
fi 

trackEOS=0                                     # % 2 == 0 will be CMS, % 2 == 1 will be USER
eosCommand=$eosCMS

RunOnList=$CrombieTempDir/RunOnList.txt
if [ "$isEOS" = "eos" ]
then
    > $RunOnList
    eosarr=(`echo $CrombieEosDir`)
    for line in ${eosarr[@]}
    do
        dirContent=(`$eosCommand ls $line`)
        if [ "$dirContent" = "" ]
        then
            trackEOS=$((trackEOS + 1))
            if [ $((trackEOS % 2)) -eq 1 ]
            then
                eosCommand=$eosUSER
                echo "eoshost=eosuser.cern.ch" >> $RunOnList
            else
                eosCommand=$eosCMS
                echo "eoshost=eoscms.cern.ch" >> $RunOnList
            fi
            dirContent=`$eosCommand ls $line`
            if [ "$dirContent" = "" ]
            then
                echo "Can't find $line in either instance of EOS..."
                echo "Check configuration."
                exit 1
            fi
        fi

        echo "eosdir="$line >> $RunOnList

        for dir in ${dirContent[@]}
        do

            if [ "$CrombieDirList" != "" ]  # If a DirList (sample list) is set, make sure the proposed directory is present
            then
                foundInList=0
                for inList in `cat $CrombieDirList`
                do
                    if [ "$dir" = "$inList" ]
                    then
                        foundInList=1
                        break
                    fi
                done
                if [ $foundInList -eq 0 ]   # If not present, don't append it to the RunOnList
                then
                    continue
                fi
            fi

            foundInRun=0                    # Do a quick check to make sure we didn't pick up a sample in another location
            for check in `cat $RunOnList`   # Check samples found
            do
                if [ "$dir" = "$check" ]
                then
                    foundInRun=1            # Pass flag if sample already will be run on
                    break
                fi
            done
            
            if [ $foundInRun -eq 0 ]
            then
                echo $dir >> $RunOnList     # Add to the RunOnList
            fi

        done
    done

else                                        # If we're not using EOS
    usingMultiEOS=0

    if [ ! -d $CrombieRegDir ]
    then
        echo "$CrombieRegDir does not seem to exist. Maybe needs mounting."
        exit 1
    fi

    if [ "$CrombieDirList" = "" ]
    then
        ls $CrombieRegDir > $RunOnList      # Dump the directory contents
    else
        cat $CrombieDirList > $RunOnList    # Or just use the set directories
    fi
fi

> $haddFile

ranOnFile=0

lastDir=''

count=0

eoshost=eoscms.cern.ch
if [ "$usingMultiEOS" -eq 1 ]
then
    eosCommand=$eosCMS
fi

for dir in `cat $RunOnList`
do
    if [ "${dir:0:1}" = "#" ]
    then
        continue
    fi

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

    if [ "$reasonableName" != "$lastDir" ]
    then 
        count=0
        lastDir=$reasonableName
        echo $CrombieFullDir/$CrombieFileBase\_$reasonableName.root $CrombieTempDir/$CrombieFileBase\_$reasonableName"_[0-9]*.root" >> $haddFile
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

cp $RunOnList $CrombieFullDir/.

exit 0
