#!/bin/bash

full=$1

testDir="test"

for dir in `ls $CROMBIEPATH/templates`
do
    
    test ! -d $dir && mkdir $dir

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

        test ! -f $dir/$file && cp $CROMBIEPATH/templates/$dir/$file $dir/$file

    done
done

location=`pwd`

if [ ! -f docs/${location##*/}.tex ]
then
    mv docs/results.tex docs/${location##*/}.tex
else
    rm docs/results.tex
fi

test ! -d docs/figs && mkdir docs/figs
