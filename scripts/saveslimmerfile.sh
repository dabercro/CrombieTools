#! /bin/bash

outdir=$HOME/public_html/slimmerfiles/$(date +'%y%m%d')
if [ ! -d $outdir ]
then
    mkdir -v -p $outdir
fi

for f in "$@"
do
    cp -v $f $outdir
done
