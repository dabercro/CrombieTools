#! /bin/bash

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
        if [ "$toRemove" != "test.sh" ]
        then
            rm -rf $toRemove
        fi
    done
    CrombieClean
fi

CrombieWorkspace test

cd $here/slimmer

host=`hostname`
if [ "${host:0:6}" = "lxplus" ]
then
    CrombieSubmitLxplus test
    echo "Just kidding, I didn't really submit anything... At least, I shouldn't have ;^)"
fi

cd $here/analysis

source CrombieAnalysisConfig.sh     # This should source the slimming config too

if [ "${host:0:6}" != "lxplus" ]
then
    cd $here
    mkdir $CrombieFullDir
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
    $CrombieCheckerScript ${outBase}_$sample.root test
    if [ $? -ne 0 ]
    then
        echo "ERROR: Tree not found!"
    fi
done

echo "Making correction histogram!"
./makeHist.py
# NEED TO MAKE THIS PARALLEL!
echo "Adding corrections to .root Files!"
./corrector.py
# Figure out something for reweighter...
# Make up a good runs file
# Run Skimmer to cut out based on some variable to eliminate background

cd $here/analysis

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

# CompileCrombieTools
