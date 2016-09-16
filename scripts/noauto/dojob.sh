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

if [ "$LSB_JOBID" != "" ]       # Do some automatic setup if on LSB
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

    cp $outFile.txt .                       # Copy the list of files for this file to local spot

else                                        # Condor has alternative setup

    ERRORLOG=condor.err

    tar -xzvf condor_package.tar.gz         # Open the package

    export X509_USER_PROXY=`pwd`/*.cert     # Export location of my proxy
    source CrombieSubmitConfig.sh           # Get the configuration

    export SCRAM_ARCH=$CrombieScram         # Get CMSSW setup from CVMFS
    export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
    source $VO_CMS_SW_DIR/cmsset_default.sh

    scramv1 project CMSSW $CrombieRelease
    cd $CrombieRelease

    if [ -f ../cmssw_patch.tgz ]            # If we have a personal patch to CMSSW, apply it
    then

        mv ../cmssw_patch.tgz .
        tar -xzvf cmssw_patch.tgz

    fi

    eval `scramv1 runtime -sh`
    cd ..

    if [ -f crombie.tgz ]                   # Install CrombieTools if present
    then

        mkdir CrombieTools
        mv crombie.tgz CrombieTools/.
        cd CrombieTools
        tar -xzvf crombie.tgz
        export CROMBIEPATH=`pwd`
        export PATH=`pwd`/bin:$PATH
        export PYTHONPATH=`pwd`/python:$PYTHONPATH
        cd ..

    fi

    # Get the input file out

    set -- `tar -ztf input_files.tar.gz | sort`
    tar -xzvf input_files.tar.gz ${!outFile}
    outFile=${!outFile}
    outFile=${outFile%%.txt}

    for file in `ls *.tgz` # Finally extract all the other files
    do

        tar -xzvf $file

    done

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

    if [ "$LSB_JOBID" = "" ]             # If on condor, copy the input files over
    then
        xrdcp root://$CrombieRedirector/$file .
        file=${file##*/}
    fi

    echoCommand="echo $file $OutputBase\_$NUM.root"

    $echoCommand                         # Debugging
    $echoCommand >> $CommandList         # Put into text file
    NUM=$((NUM + 1))

done

# Run the slimmer in parallel using xargs

cat $CommandList | xargs -n2 -P$CrombieNBatchProcs $CrombieSlimmerScript

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

    if [ -f condor.err ]        # Let condor handle the errors for us
    then
        
        cat condor.err >&2

    fi

    exit 0

fi

echo ""

if [ "$LSB_JOBID" = "" ]
then

    echo "Copying to $CrombieTempDir/$outFile.root?"
    lcg-cp -v -D srmv2 -b file://$PWD/$OutputBase.root \
        srm://t3serv006.mit.edu:8443/srm/v2/server\?SFN=$CrombieTempDir/$outFile.root

else

    echo "Copying to $outFile.root"
    cp $OutputBase.root $outFile.root        # Try to copy

fi

if [ $? -ne 0 ]                              # Check for success
then

    echo "Copying failed! Check your quota." >> $ERRORLOG
    echo "Check output in job bout/out.$LSB_JOBID" >> $ERRORLOG
    echo "" >> $ERRORLOG

fi

if [ -f condor.err ]                         # Let condor handle the errors for us
then

    cat condor.err >&2

fi

exit 0
