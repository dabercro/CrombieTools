#!/bin/bash

date=$1

projName=`git rev-parse --show-toplevel`

if [ ! -f $projName/docs/presentation.tex ]
then
    echo ""
    echo "Must be called from within a directory that was set up by"
    echo "CrombieWorkspace and initialized as a git repository."
    echo ""
    exit 0
fi

if [ "$date" = "" ]
then
    date=$CROMBIEDATE
fi

if [ -d $projName/docs/$date ]
then
    echo "Presentation already exists with selected date: $date"
    exit 0
fi

target=$projName/docs/$date

mkdir $target
cp $projName/docs/presentation.tex $target/$USER\_${projName##*/}\_$date.tex
mkdir $target/figs
cp $projName/docs/download.sh $target/figs/.

echo "All done!"
echo "Don't forget to tag the repository after the presentation is complete..."