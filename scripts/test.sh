#!/bin/bash

##
#  @file test.sh
#
#  The tests to run on a new installation or
#  while developing CrombieTools.
#
#  @author Daniel Abercrombie
#

fast=$1

returnto=`pwd`

if [ "$CROMBIEPATH" = "" ]
then
    echo "Installation not complete!"
    echo " Did you run ../install.sh and source your bash profile?"
    exit 1
fi

here=$CROMBIEPATH/test

echo "Testing package installed at $CROMBIEPATH"

cd $here

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

mkdir txtoutput 2> /dev/null
echo "Running crombie diff"
crombie diff FullOut/ SkimOut/ | sort | tail -n 5 > txtoutput/diffoutput.txt

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
## @todo Make tests
# Make limit tree
# Make stacks with MC configs
# Make stacks using limit trees
# Make parameter fit plots (2D and FitParameters)
# Traing BDTs
# Make ROC curves
# Plot Hists to show discriminators include normalization
# Include systematics
# Make more stack plots with BDT cuts
./cutflow.py

cd $here/docs

pdflatex test.tex &> /dev/null
pdflatex test.tex &> /dev/null

mkdir figs 2> /dev/null

cp $here/plotter/plots/* figs/.

crombie backupslides

pdflatex \\nonstopmode\\input presentation.tex &> /dev/null
pdflatex \\nonstopmode\\input presentation.tex &> /dev/null

cd $here

crombie compile

cd $returnto

if [ "$fast" != "fast" ]
then
    echo "-------------------------------------------------------------------"
    echo "Making sure that rerunning doesn't overwrite files or recompile ..."
    echo "-------------------------------------------------------------------"
    crombie test fast
fi