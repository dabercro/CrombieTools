#! /bin/bash

fast=$1

here=`pwd`

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
    ./runSlimmer.py $sample.root {$outBase}_$sample.root
    ls {$outBase}_$sample.root
    $CrombieCheckerScript {$outBase}_$sample.root
    if [ $? -ne 10000 ]
    then
        echo "ERROR: Number of generated events not correct!"
    fi
done

# Run HistWriter to make histogram for background corrections (shape it)
# Apply corrections to Background and (not actually) Signal
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

CompileCrombieTools
