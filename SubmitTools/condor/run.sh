#! /bin/bash

echo `pwd`

ID=$1

wget -O params.json http://t3serv001.mit.edu/~dabercro/submit/\?id=$ID

# Bootstrap jq out of our tarball

tar -xf condor.tgz bin/*/jq --strip=2

# Get the parameters from the json file

./jq . params.json

EXE=$(./jq -r '.exe' params.json)
INPUT_DIR=$(./jq -r '.input_dir' params.json)
INPUT_FILES=$(./jq -r '.input_files | join(" ")' params.json)
OUTPUT_DIR=/mnt/hadoop/cms$(./jq -r '.output_dir' params.json)
OUTPUT_FILE=$(./jq -r '.output_file' params.json)
SCRAM_ARCH=$(./jq -r '.arch' params.json)
CMSSW_VERSION=$(./jq -r '.cmssw' params.json)
BASE=$(./jq -r '.base' params.json)

# Setup CMSSW

source /cvmfs/cms.cern.ch/cmsset_default.sh

if [ -d $BASE ]
then

    pushd $BASE/src
    eval `scram runtime -sh`
    popd

else

    test -d $CMSSW_VERSION || scram project CMSSW $CMSSW_VERSION
    pushd $CMSSW_VERSION
    tar -xf ../condor.tgz
    cd src

    eval `scram runtime -sh`

    popd

fi

echo ""
echo "After CMSSW setup"
echo ""

# Download or softlink Panda files

echo `pwd`
ls -lh
SCRATCH=/mnt/hadoop/scratch/dabercro

for IN_FILE in $INPUT_FILES
do

    if [ ! -f $IN_FILE ]
    then

        # Check to see if it exists locally in the normal place

        FULL_IN=/mnt/hadoop/cms$INPUT_DIR/$IN_FILE

        echo findtree.py $FULL_IN

        if findtree.py $FULL_IN
        then

            ln -s $FULL_IN $IN_FILE

        else

            if [ ! -f $FULL_IN ]
            then
                STATUS="missing"
            else
                STATUS="corrupt"
            fi

            wget -O $IN_FILE.report "http://t3serv001.mit.edu/~dabercro/submit/?report=$FULL_IN&status=$STATUS"

            if [ ! -d $(dirname `dirname $FULL_IN`) ]
            then

                xrdcp root://xrootd.cmsaf.mit.edu//$INPUT_DIR/$IN_FILE .

            elif [ -d $SCRATCH ]
            then

                NEW_INPUT_DIR=$SCRATCH$INPUT_DIR

                echo ""
                echo "test -d $NEW_INPUT_DIR || mkdir -p $NEW_INPUT_DIR"
                echo "test -f $NEW_INPUT_DIR/$IN_FILE || xrdcp root://xrootd.cmsaf.mit.edu//$INPUT_DIR/$IN_FILE $NEW_INPUT_DIR"
                echo "ln -s $NEW_INPUT_DIR/$IN_FILE $IN_FILE"
                echo ""

                test -d $NEW_INPUT_DIR || mkdir -p $NEW_INPUT_DIR
                test -f $NEW_INPUT_DIR/$IN_FILE || xrdcp root://xrootd.cmsaf.mit.edu//$INPUT_DIR/$IN_FILE $NEW_INPUT_DIR/$IN_FILE
                ln -s $NEW_INPUT_DIR/$IN_FILE $IN_FILE

            fi

        fi

    fi

done

echo ""
echo "After download"
echo ""

echo `pwd`
ls -lh

# Run the slimmer

test -f $OUTPUT_FILE || $EXE $INPUT_FILES $OUTPUT_FILE

exitcode=$?

echo ""
echo "After slimmer"
echo ""

echo `pwd`
ls -lh

# The condor.cfg maps the output file to the correct place

# Give the exit code from the slimmer
exit $exitcode
