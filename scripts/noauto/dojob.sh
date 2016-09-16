#!/bin/bash

##
# @file: dojob.sh
#
# This is the script that is run for each LXBATCH or Condor job run by CrombieTools.
# The only argument is the filename that will be created.
# Any other configuration is done in [CrombieSlimmingConfig.sh](@ref slimming).
#
# @author: Daniel Abercrombie <dabercro@mit.edu>
#

outFile=$1                      # The only argument is the output file (no extension)

env

if [ $LSB_JOBID != "" ]         # Do some automatic setup if on LSB
then

    macroDir=$LS_SUBCWD         # Get the configuration from the submission directory

    source $macroDir/CrombieSlimmingConfig.sh

    ERRORLOG=$macroDir/LxbatchFileChecks.log


    if [ "$CMSSW_BASE" != "" ]  # If CMSSW_BASE filled, scram it
    then

        cd $CMSSW_BASE/src
        eval `scram runtime -sh`
        cd -

    fi

    # Copy all files from $CrombieJobScriptList to local directories

    for file in `cat $macroDir/$CrombieJobScriptList`
    do

        if [ "$file" != "${file/\//}" ]    # If the file has directories
        then                               #   make the directory for it to live in

            making=${file%%/${file##*/}}

            if [ ! -d $making ]
            then

                mkdir -p $making

            fi
        fi

        cp $macroDir/$file $file            # Copy file to local location

    done

    cp $outFile.txt .      # Copy the list of files for this file to local spot

else                       # Condor has alternative setup

    

fi

echo "Trying to make $outFile.root"
echo ""

NUM=0                      # Count the subfiles made

$CrombieSlimmerScript      # Compile the slimmer script

if [ $? -ne 0 ]            # Check that compilation succeeded
then
    echo "Compilation failed. Submit again..."
    exit 0
fi

OutputBase="lxbatchTmpOutput"            # Some default names
CommandList="ArgsForThisJob.txt"         # These variables aren't too important
> $CommandList                           # Initialize command list

for file in `cat $outFile.txt`           # Dump name of each input file into argument list
do

    if [ "${file:0:7}" = "root://" ]     # If full name is there, use it
    then

        echoCommand="echo $file $OutputBase\_$NUM.root"

    else                                 # Otherwise, add the server by hand

        echoCommand="echo root://$eosServer/$file $OutputBase\_$NUM.root"

    fi

    $echoCommand                         # Debugging
    $echoCommand >> $CommandList         # Put into text file
    NUM=$((NUM + 1))

done

# Run the slimmer in parallel using xargs

cat $CommandList | xargs -n2 -P$LSB_DJOB_NUMPROC $CrombieSlimmerScript

# Hadd the results together

hadd $OutputBase.root $OutputBase\_*.root

FATALERROR=0
ALLEMPTY=1
FINALEMPTY=0

for file in `ls $OutputBase*.root`       # Now check for errors
do

    "$CrombieCheckerScript" $file        # Check, with exit code 5 for fatal

    if [ $? -ne 0 ]                      # If found error
    then

        if [ $? -eq 5 ]                  # Check for fatal error
        then

            echo "Could not find acceptable output in $file" >> $ERRORLOG
            echo "Check output in job bout/out.$LSB_JOBID" >> $ERRORLOG
            echo "" >> $ERRORLOG
            FATALERROR=1

        elif [ "$file" = "$OutputBase.root" ]    # If the final output is not fatal, make a note
        then

            FINALEMPTY=1

        fi

    else

        ALLEMPTY=0              # Note whether or not everything is empty

    fi

done

if [ $FINALEMPTY -eq 1 ]        # If the final is empty and not everything else is
then                            #   that's bad and should be a fatal error.
                                #   If everything is empty, that's okay.
    if [ $ALLEMPTY -eq 0 ]      #   Copy a placeholder over anyway.
    then

        echo "Final output is empty, but not all are." >> $ERRORLOG
        echo "Check output in job bout/out.$LSB_JOBID" >> $ERRORLOG
        echo "" >> $ERRORLOG
        FATALERROR=1

    else

        echo "Everything is empty in this job, copying anyway." >> $ERRORLOG
        echo "Check output in job bout/out.$LSB_JOBID" >> $ERRORLOG
        echo "" >> $ERRORLOG

    fi
fi

if [ $FATALERROR -eq 1 ]        # For fatal error, report to user and quit quietly
then

    echo "FATAL ERROR in bout/out.$LSB_JOBID" >> $ERRORLOG
    echo "" >> $ERRORLOG
    exit 0

fi

echo ""
echo "Copying to $outFile.root"

cp $OutputBase.root $outFile.root        # Try to copy

if [ $? -ne 0 ]                          # Check for success
then

    echo "Copying failed! Check your quota." >> $ERRORLOG
    echo "Check output in job bout/out.$LSB_JOBID" >> $ERRORLOG
    echo "" >> $ERRORLOG

fi

exit 0
