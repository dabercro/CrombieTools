#!/bin/bash

date=$1

projName=`git rev-parse --show-toplevel`

docs=`find $projName -name docs`

if [ ! -f $docs/presentation.tex ]
then
    echo ""
    echo "Must be called from within a directory that was set up by"
    echo "crombie workspace and initialized as a git repository."
    echo ""
    exit 0
fi

if [ "$date" = "" ]
then
    date=$(date +%y%m%d)
fi

if [ -d $docs/$date ]
then
    echo "Presentation already exists with selected date: $date"
    exit 0
fi

target=$docs/$date

mkdir $target
cp $docs/presentation.tex $target/$USER\_${projName##*/}\_$date.tex
mkdir $target/figs
cp $docs/download.sh $target/figs/.

cd $target

echo "All done!"
echo "Don't forget to tag the repository after the presentation is complete..."
