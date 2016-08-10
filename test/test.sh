#!/bin/bash

fast=$1

here=`pwd`

if [ "$CROMBIEPATH" = "" ]
then
    echo "Installation not complete!"
    echo " Did you run ../install.sh and source your bash profile?"
    exit 1
fi

echo "Testing package installed at $CROMBIEPATH"

if [ "$fast" != "fast" ]                   # Start from fresh directory
then                                       # unless we only want to quickly
    for toRemove in `ls`                   # check new features or tests
    do
        if [ -d $toRemove ]                # Only remove directories
        then                               # Leave all testing scripts
            rm -rf $toRemove               # and READMEs
        fi
    done
    crombie clean
fi

crombie workspace test

cd $here/slimmer

host=`hostname`
if [ "${host:0:6}" = "lxplus" ]
then
    crombie submit test
    echo "Just kidding, I didn't really submit anything... At least, I shouldn't have ;^)"
fi

cd $here/plotter

source CrombiePlotterConfig.sh             # This should source the slimming config too

if [ "${host:0:6}" != "lxplus" ]
then
    cd $here
    mkdir $CrombieFullDir &> /dev/null
fi

if [ ! -d $CrombieFullDir ]
then
    echo "scripts/CrombieDumpFileList should have made your out directory, but didn't!"
fi

cd $here/slimmer

outBase=$CrombieFullDir/$CrombieFileBase
for sample in "Data" "Signal" "MC1" "MC2" "MC3"
do
    echo "Generating pretend $sample."
    ./runSlimmer.py $sample.root ${outBase}_$sample.root
    ls ${outBase}_$sample.root
    $CrombieCheckerScript ${outBase}_$sample.root
    if [ $? -ne 0 ]
    then
        echo "ERROR: Tree not found!"
    fi
done

echo "Skimming with good runs!"
./FlatSkimmer.sh

cd $here

mkdir txtoutput
echo "Running crombie diff"
crombie diff FullOut SkimOut > txtoutput/diffoutput.txt

echo "Comparing output..."
diff txtoutput/diffoutput.txt diffoutput.txt

cd $here/slimmer

echo "Making correction histogram!"
./makeHist.py
echo "Adding corrections to .root Files!"
./corrector.py

cd $here/plotter

./AddXSec.py
./reweight.py
./corrector.py

./Stack.py
# Make limit tree
# Make stacks with MC configs
# Make stacks using limit trees
# Make parameter fit plots (2D and FitParameters)
# Traing BDTs
# Make ROC curves
# Plot Hists to show discriminators include normalization
# Include systematics
# Make more stack plots with BDT cuts
# Make cutflow

## @todo Make tests

crombie compile
cd $here

if [ "$fast" != "fast" ]
then
    echo "-------------------------------------------------------------------"
    echo "Making sure that rerunning doesn't overwrite files or recompile ..."
    echo "-------------------------------------------------------------------"
    ./test.sh fast
fi
