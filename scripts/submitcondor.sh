#!/bin/bash

##
# @file submitcondor.sh
#
# @brief Submits jobs to condor.
# It relies on only software on CVMFS.
# @todo add documentation to the cli ref
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
echo "${CrombieRedirector:?}" > /dev/null
echo "${CrombieProjectName:?}" > /dev/null
echo "${CrombieCondorOutput:?}" > /dev/null

export haddFile=$CrombieTempDir/myHadd.txt          # Export the hadd file location

here=`pwd`
condorball=/work/dabercro/$CrombieProjectName

if [ ! -d $condorball ]
then
    mkdir -p $condorball
fi

tarDir=$condorball

_makeTar () {     # This remakes the target tarball if any of the input files are newer

    echo _makeTar ${@}

    outFile=$tarDir/$1
    shift
    files=${@}

    if [ ! -f $outFile -o $(find $files -newer $outFile | wc -l) -ne 0 ]
    then
        tar -czhvf $outFile $files
    fi

}

if [ "$CrombieCmsswBase" != "" ]                    # If we have a modified CMSSW,
then                                                #   locate it with this variable

    cd $CrombieCmsswBase

    files=""

    for dir in `ls`                                 # pack everything
    do

        if [ "$dir" != "src" ]                      # except src
        then

            files=$files" $dir"

        fi

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

chmod 777 $CrombieTempDir
cd $CrombieTempDir

files=""
njobs=0

existingFiles=`lcg-ls -D srmv2 -b srm://t3serv006.mit.edu:8443/srm/v2/server?SFN=$CrombieCondorOutput`

for file in $CrombieFileBase\_*_*.txt                         # Check how many of these files
do

    outFile=${file%%.txt}.root
    if [[ ! $existingFiles =~ `basename $outFile` ]]
    then

        files=$files" $file"
        njobs=$((njobs + 1))

    fi

done

tar -czf $tarDir/input_files.tar.gz $files                    # Update this every time

cd $tarDir

if [ ! -f  `basename $X509_USER_PROXY` ]
then

    cp $X509_USER_PROXY .

fi

# Make the final tar file
cp $here/$CrombieSlimmerScript .
cp $here/Crombie*Config.sh .
cp $here/CrombieCondorConfig.cgf .

_makeTar condor_package.tar.gz *.tgz $CrombieSlimmerScript Crombie*Config.sh `basename $X509_USER_PROXY`

sed 's@CROMBIEDOJOB@'$CROMBIEPATH'/scripts/noauto/dojob.sh@g' CrombieCondorConfig.cgf | sed 's@NJOBS@'$njobs'@g' | \
    sed 's@PROJECTNAME@'$USER/$CrombieProjectName'@g' > config.cfg

if [ $njobs -ne 0 ]
then

    command="condor_submit config.cfg"

    if [ "$fresh" = "test" ]
    then

        echo $command

    else

        $command

    fi

else

    echo "${CrombieNLocalProcs:?}" > /dev/null
    echo cat $haddFile | xargs -n2 -P$CrombieNLocalProcs crombie hadd
    echo "All files merged!"

fi

cd $here
