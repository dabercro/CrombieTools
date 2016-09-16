#!/bin/bash

##
# @file dumpfilelist.sh
#
# This is the file that creates the input files for running slimmers over EOS,
# files in Phedex, or other local directories.
#
# @todo Clean this up when not using multiple EOS directories
#
# @author Daniel Abercrombie <dabercro@mit.edu>
#

isEOS=$1

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
    then                                       # which clears out temp .root files from before

        rm $CrombieTempDir/*.root 2> /dev/null

    fi
fi

# Check to see if we will be using multi EOS

if [ -f $CrombieEosDir ]
then
    CrombieEosDir=`cat $CrombieEosDir`         # Set list of EOS directories
    usingMultiEOS=1
else
    usingMultiEOS=0
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

    if [ -f $CrombieDatasets ]
    then

        cp $CrombieDatasets $RunOnList

    else
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

            
_CheckCount () {

    if [ "$fileInCount" -eq "$CrombieFilesPerJob" ]
    then
        
        fileInCount=0
        count=$((count + 1))
        currentConfig=$CrombieTempDir/$CrombieFileBase\_$reasonableName\_$count.txt
        > $currentConfig
        
    fi

}

for dir in `cat $RunOnList`
do

    if [ "${dir:0:1}" = "#" ]                   # Skip commented directories
    then

        continue

    fi

    if [ "${dir%%=*}" = "eoshost" ]             # Can change eoshost
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

    elif [ "${dir%%=*}" = "eosdir" ]            # Can change eosdir
    then

        CrombieEosDir=${dir##*=}
        continue

    fi

    fileInCount=$CrombieFilesPerJob

    stripleading="${dir#/}"
    reasonableName="${stripleading%%/*}"

    if [ "$reasonableName" != "$lastDir" ]      # Build the file that hadds everything
    then 

        count=0
        lastDir=$reasonableName
        echo $CrombieFullDir/$CrombieFileBase\_$reasonableName.root $CrombieTempDir/$CrombieFileBase\_$reasonableName"_[0-9]*.root" >> $haddFile

    fi

    if [ "$isEOS" = "eos" ]                     # Find all the root files in EOS
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
            
            _CheckCount

            echo root://$eoshost/$inFile >> $currentConfig

            fileInCount=$((fileInCount + 1))

        done

    elif [ -f $CrombieDatasets ]                # If we have a list of datasets, then dump the file names from Phedex
    then

        outputJSON=$CrombieTempDir/$reasonableName.json

        if [ ! -f $outputJSON ]
        then

            wget --no-check-certificate -O $outputJSON "https://cmsweb.cern.ch/phedex/datasvc/json/prod/data?dataset=$dir"

        fi

        for inFile in `jq '.phedex.dbs|.[].dataset|.[].block|.[].file|.[].lfn' $outputJSON | sed 's/"//g' | sort | uniq`
        do

            _CheckCount

            echo $inFile >> $currentConfig
            fileInCount=$((fileInCount + 1))
            indexFiles=$((indexFiles + 1))

        done

    else                                        # Otherwise, just find all of the .root files in a directory

        for inFile in `find $CrombieRegDir/$dir -name '*.root'`; do

            if [ "${inFile##*_}" = "pilot.root" ]; then

                continue

            fi

            _CheckCount

            echo $inFile $CrombieTempDir/TerminalRunning/$CrombieFileBase\_$reasonableName\_$count\_$indexFiles.root >> $currentConfig
            fileInCount=$((fileInCount + 1))
            indexFiles=$((indexFiles + 1))

        done

    fi

done

cp $RunOnList $CrombieFullDir/.

exit 0
