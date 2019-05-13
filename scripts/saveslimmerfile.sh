#! /bin/bash

outdir=$HOME/public_html/slimmerfiles/$(date +'%y%m%d')
if [ ! -d $outdir ]
then
    mkdir -v -p $outdir
fi

for f in "$@"
do

    target=$outdir/$(basename $f)

    if [ -f $target ]
    then
        echo "!!! $target already exists! Not overwriting. You have to delete !!!"
    else
        cp -v $f $outdir
    fi

done
