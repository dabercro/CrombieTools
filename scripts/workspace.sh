#!/bin/bash

full=$1

testDir="test"

for dir in `ls $CROMBIEPATH/templates`
do

    # Ignore the files directly in the templates directory
    if [ -f $CROMBIEPATH/templates/$dir ]
    then
        continue
    fi

    # Make a local directory if needed
    if [ ! -d $dir ]
    then
        mkdir $dir
    fi

    if [ "$full" = "test" ]
    then
        for testFile in `ls $CROMBIEPATH/templates/$dir/$testDir`
        do
            if [ ! -f $dir/$testFile ]
            then
                cp $CROMBIEPATH/templates/$dir/$testDir/$testFile $dir/$testFile
            fi
        done
    fi

    for file in `ls $CROMBIEPATH/templates/$dir`
    do
        if [ "$file" = "$testDir" ]
        then
            continue
        fi

        if [ ! -f $dir/$file ]
        then
            cp $CROMBIEPATH/templates/$dir/$file $dir/$file
        fi

    done
done

location=`pwd`

if [ ! -f docs/${location##*/}.tex ]
then
    mv docs/results.tex docs/${location##*/}.tex
else
    rm docs/results.tex
fi

if [ ! -d docs/figs ]
then
    mkdir docs/figs
fi
