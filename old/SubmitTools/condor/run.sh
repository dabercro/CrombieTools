#! /bin/bash

hostname
pwd

ID=$1

wget -O params.json http://t3serv001.mit.edu/~dabercro/squid/\?id=$ID

# Bootstrap jq out of our tarball

tar -xf condor.tgz bin/*/jq --strip=2

# Get the parameters from the json file

./jq . params.json

EXE=$(./jq -r '.exe' params.json)
INPUT_DIR=$(./jq -r '.input_dir' params.json)
INPUT_FILES=$(./jq -r '.input_files | join(" ")' params.json)
OUTPUT_FILE=$(./jq -r '.output_file' params.json)
SCRAM_ARCH=$(./jq -r '.arch' params.json)
CMSSW_VERSION=$(./jq -r '.cmssw' params.json)
BASE=$(./jq -r '.base' params.json)

# Setup CMSSW

source /cvmfs/cms.cern.ch/cmsset_default.sh

test -d $CMSSW_VERSION || scram project CMSSW $CMSSW_VERSION
pushd $CMSSW_VERSION
tar -xf ../condor.tgz
cd src

eval `scram runtime -sh`

popd

if [ -d $CMSSW_BASE/data ]
then
    ln -s $CMSSW_BASE/data data
fi

echo ""
echo "After CMSSW setup"
echo ""
pwd
ls -lh

# Download or softlink Panda files

SCRATCH=/mnt/hadoop/scratch/dabercro
NEW_INPUT_DIR=$SCRATCH$INPUT_DIR

for IN_FILE in $INPUT_FILES
do

    if [ ! -f $IN_FILE ]
    then

        # Check to see if it exists locally in the normal place

        FULL_IN=/mnt/hadoop/cms$INPUT_DIR/$IN_FILE

        if [ -f $FULL_IN ]
        then

            cp -v $FULL_IN $IN_FILE

        else

            xrdcp root://xrootd.cmsaf.mit.edu//$INPUT_DIR/$IN_FILE .

        fi

    fi

done

echo ""
echo "After download"
echo ""

pwd
ls -lh

# Run the slimmer

test -f $OUTPUT_FILE || $EXE $INPUT_FILES $OUTPUT_FILE

exitcode=$?

echo ""
echo "After slimmer"
echo ""

pwd
ls -lh

# The condor.cfg maps the output file to the correct place

# Give the exit code from the slimmer
echo "Exiting with $exitcode"
exit $exitcode
