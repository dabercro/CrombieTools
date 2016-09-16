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

config=CrombieSubmitConfig.sh

if [ ! -f $config ]
then
    echo "$config isn't here."
    echo "You should probably go somewhere else."
    exit 1
fi

source $config

echo "${CrombieFilesPerJob:?}" > /dev/null
echo "${CrombieNBatchProcs:?}" > /dev/null
echo "${CrombieEosDir:?}" > /dev/null
echo "${CrombieTempDir:?}" > /dev/null
echo "${CrombieFullDir:?}" > /dev/null
echo "${CrombieSlimmerScript:?}" > /dev/null
echo "${CrombieJobScriptList:?}" > /dev/null
echo "${CrombieCheckerScript:?}" > /dev/null

echo "${CrombieScram:?}" > /dev/null
echo "${CrombieRelease:?}" > /dev/null

export haddFile=$CrombieTempDir/myHadd.txt

here=`pwd`
condorball=condorinput

if [ ! -d $condorball ]
then
    mkdir $condorball
fi

tarDir=$here/$condorball

_makeTar () {

    outFile=$tarDir/$1
    shift
    files=${@}

    if [ ! -f $outFile -o $(find $files -newer $outFile | wc -l) -ne 0 ]
    then
        tar -czf $outFile $files
    fi

}

if [ "$CrombieCmsswBase" != "" ]
then

    cd $CrombieCmsswBase

    files=""
    for dir in "bin" "cfipython" "lib" "python"
    do
        files=$dirs" $dir/$CrombieScram/*"
    done

    _makeTar cmssw_patch.tgz $files

fi

if [ "$UseCrombie" != "0" ]
then

    cd $CROMBIEPATH
    _makeTar crombie.tgz bin python scripts *Tools

fi

cd $here

_makeTar script_files.tgz `cat $CrombieJobScriptList`

crombie dumpfilelist

cd $CrombieTempDir

_makeTar input_files.tgz $CrombieFileBase\_*_*.txt

cd $tarDir

_makeTar condor_package.tar.gz *.tgz
