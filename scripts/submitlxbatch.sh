#!/bin/bash

##
# @file submitlxbatch.sh
#
# Script that is used to submit jobs to lxbatch.
# See the [command line tool reference](@ref commandref)
# for more information on subcommands.
#
# @author Daniel Abercrombie <dabercro@mit.edu>
#

export fresh=$1

# Check for the configuration file

if [ ! -f CrombieSlimmingConfig.sh ]
then
    echo "CrombieSlimmingConfig.sh isn't here."
    echo "You should probably go somewhere else."
    exit 1
fi

# Requires CMSSW to get ROOT, unfortunately
## @todo Try to eliminate the dependency on CMSSW

if [ "$CMSSW_BASE" = "" ]
then
    echo "Require you to be cmsenv somewhere"
    exit 1
fi

# Record the submission in this log file

logFile=`pwd`/LxbatchFileChecks.log
sub=""

if [ "$fresh" != "" ]
then
    sub=" $fresh"
fi

submitLog="Ran crombie submitlxbatch$sub at "`date -u`
export subLogCharNum=${#submitLog}
delim=`perl -e 'print "=" x $ENV{'subLogCharNum'}';`

echo "$delim" >> $logFile
echo "$submitLog" >> $logFile
echo "$delim" >> $logFile

if [ ! -d bout ]                       # Make sure there's a place for the job's stdout.
then
    mkdir bout
fi

source CrombieSlimmingConfig.sh        # Get needed environment variables

# Check that needed environment variables are present

echo "${CrombieFilesPerJob:?}" > /dev/null
echo "${CrombieNBatchProcs:?}" > /dev/null
echo "${CrombieQueue:?}" > /dev/null
echo "${CrombieEosDir:?}" > /dev/null
echo "${CrombieTempDir:?}" > /dev/null
echo "${CrombieFullDir:?}" > /dev/null
echo "${CrombieSlimmerScript:?}" > /dev/null
echo "${CrombieJobScriptList:?}" > /dev/null
echo "${CrombieCheckerScript:?}" > /dev/null

export haddFile=$CrombieTempDir/myHadd.txt

# Dump the list of files on EOS to run on

if [ "$fresh" != "resub" -a "$fresh" != "hadd" ]
then
    crombie dumpfilelist eos
    if [ $? -ne 0 ]
    then
        echo "Crashed while dumping file list."
        exit 1
    fi
fi

ranOnFile=0                            # Keep track on if files are submitted or not

if [ "$fresh" != "hadd" ]
then

    rootNames=`ls $CrombieTempDir/$CrombieFileBase\_*_*.txt | sed 's/.txt//'`

    for outFile in $rootNames
    do

        if [ ! -f $outFile.root ]      # Check each file to see if they exist, if not, submit
        then

            echo "Making: $outFile"
            command="bsub -q $CrombieQueue -n $CrombieNBatchProcs -o bout/out.%J crombie dojob $outFile"

            if [ "$fresh" = "test" ]   # Echo the command if a test
            then                       #   otherwise, execute it

                echo $command

            else

                $command

            fi

            ranOnFile=1                # Job was submitted
        fi

    done

else

    echo "Going directly to hadd step."

fi

if [ "$ranOnFile" -eq 0 ]              # If no jobs submitted or tested, hadd
then

    echo "${CrombieNLocalProcs:?}" > /dev/null
    cat $haddFile | xargs -n2 -P$CrombieNLocalProcs crombie hadd
    echo "All files merged!"

fi
