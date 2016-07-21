#!/bin/bash

full=$1

testDir="test"

for dir in `ls $CROMBIEPATH/templates`
do
    
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
mv docs/results.tex docs/${location##*/}.tex
mkdir docs/figs
