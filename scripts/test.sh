#!/bin/bash

##
#  @file test.sh
#
#  The tests to run on a new installation or
#  while developing CrombieTools.
#
#  @todo test all the useful things.
#
#  @author Daniel Abercrombie
#

fast=$1

check () {

    code=$?
    echo "Exit code: $code"
    if [ $code -ne 0 ]
    then
        exit $code
    fi

}

returnto=`pwd`

if [ "$CROMBIEPATH" = "" ]
then
    echo "Installation not complete!"
    echo " Did you run ../install.sh and source your bash profile?"
    exit 1
fi

here=$CROMBIEPATH/old/test

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
check

cd $here/slimmer

host=`hostname`
if [ "${host:0:6}" = "lxplus" ]
then
    crombie submitlxbatch test
    check
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
    echo "'crombie dumpeosfiles' should have made your out directory, but didn't!"
fi

cd $here/slimmer

outBase=$CrombieFullDir/$CrombieFileBase
for sample in "Data" "Signal" "MC1" "MC2" "MC3"
do
    echo "Generating pretend $sample."
    ./runSlimmer.py $sample.root ${outBase}_$sample.root
    check
    ls ${outBase}_$sample.root
    $CrombieCheckerScript ${outBase}_$sample.root
    check
done

echo "Skimming with good runs!"
./FlatSkimmer.sh
check

cd $here

mkdir txtoutput 2> /dev/null
echo "Running crombie diff"
crombie diff FullOut/ SkimOut/ | sort | tail -n 5 > txtoutput/diffoutput.txt
check

cd $here/slimmer

echo "Making correction histogram!"
./makeHist.py
check
echo "Adding corrections to .root Files!"
./corrector.py
check

cd $here/plotter

echo "Adding cross sections for fun."
./AddXSec.py
check
echo "Reweighting by something."
./reweight.py
check
echo "Applying reweight corrections."
./corrector.py
check

echo "Making stack plots."
./Stack.py
check

# Make limit tree
# Make stacks with MC configs
# Make stacks using limit trees
# Make parameter fit plots (2D and FitParameters)
# Traing BDTs
# Make ROC curves
# Plot Hists to show discriminators include normalization
# Include systematics
# Make more stack plots with BDT cuts

echo "Making cutflow."
./cutflow.py
check

echo "Making brazilian plots."
./Brazil.py
check

cd $here/docs

echo "Trying to make docs."
pdflatex test.tex &> /dev/null
pdflatex test.tex &> /dev/null

mkdir figs 2> /dev/null

cp $here/plotter/plots/* figs/.

echo "Making backup slides."
crombie backupslides
check

pdflatex \\nonstopmode\\input presentation.tex &> /dev/null
pdflatex \\nonstopmode\\input presentation.tex &> /dev/null

echo "Done trying."

cd $here

## Makes a huge mess because of global mutexes in RunParallel
# echo "Compiling everything left over."
# crombie compile
# check

cd $returnto

if [ "$fast" != "fast" ]
then
    echo "-------------------------------------------------------------------"
    echo "Making sure that rerunning doesn't overwrite files or recompile ..."
    echo "-------------------------------------------------------------------"
    crombie test fast
    check
fi

exit 0
