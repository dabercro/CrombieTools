#!/bin/bash

##
# @file submitcondor.sh
#
# @brief Submits jobs to condor.
# It relies on only software on CVMFS.
#
# @author Daniel Abercrombie <dabercro@mit.edu>
#

export fresh=$1

config=CrombieSubmitConfig.sh                      # Search for the configuration

if [ ! -f $config ]
then
    echo "$config isn't here."
    echo "You should probably go somewhere else."
    exit 1
fi

source $config                                      # Source the configuration

echo "${CrombieFilesPerJob:?}" > /dev/null          # Check for variables
echo "${CrombieNBatchProcs:?}" > /dev/null          #   that we'll want
echo "${CrombieEosDir:?}" > /dev/null
echo "${CrombieTempDir:?}" > /dev/null
echo "${CrombieFullDir:?}" > /dev/null
echo "${CrombieSlimmerScript:?}" > /dev/null
echo "${CrombieJobScriptList:?}" > /dev/null
echo "${CrombieCheckerScript:?}" > /dev/null

echo "${CrombieScram:?}" > /dev/null
echo "${CrombieRelease:?}" > /dev/null

export haddFile=$CrombieTempDir/myHadd.txt          # Export the hadd file location

here=`pwd`
condorball=condorinput

if [ ! -d $condorball ]
then
    mkdir $condorball
fi

tarDir=$here/$condorball

_makeTar () {     # This remakes the target tarball if any of the input files are newer

    outFile=$tarDir/$1
    shift
    files=${@}

    if [ ! -f $outFile -o $(find $files -newer $outFile | wc -l) -ne 0 ]
    then
        tar -czf $outFile $files
    fi

}

if [ "$CrombieCmsswBase" != "" ]                    # If we have a modified CMSSW,
then                                                #   locate it with this variable

    cd $CrombieCmsswBase

    files=""
    for dir in "bin" "cfipython" "lib" "python"     # Just pack up these directories
    do
        files=$dirs" $dir/$CrombieScram/*"
    done

    _makeTar cmssw_patch.tgz $files

fi

if [ "$UseCrombie" != "0" ]                         # Pack up CrombieTools if we want it
then

    cd $CROMBIEPATH
    _makeTar crombie.tgz bin lib python scripts *Tools

fi

cd $here

_makeTar script_files.tgz `cat $CrombieJobScriptList`         # Collect any other files or scripts needed for the run

crombie dumpfilelist                                          # Make a list of input files

cd $CrombieTempDir

files=""
njobs=0

for file in $CrombieFileBase\_*_*.txt                         # Check how many of these files
do

    if [ ! -f ${file%%.txt}.root ]
    then

        files=$files" $file"
        njobs=$((njobs + 1))

    fi

done

tar -czf $tarDir/input_files.tar.gz $files                    # Update this every time

cd $tarDir

_makeTar condor_package.tar.gz *.tgz ../Crombie*Config.sh     # Make the final tar file

sed 's@CROMBIEDOJOB@$CROMBIEPATH/scripts/noauto/dojob.sh@g' ../CrombieCondorConfig.cgf | sed 's@NJOBS@$njobs@g' > config.cfg

if [ $njobs -ne 0 ]
then

    condor_submit config.cfg

else

    echo "${CrombieNLocalProcs:?}" > /dev/null
    cat $haddFile | xargs -n2 -P$CrombieNLocalProcs crombie hadd
    echo "All files merged!"

fi
