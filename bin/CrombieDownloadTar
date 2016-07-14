#!/bin/bash

wget -O CrombiePlots.tar.gz "$1"

tar -xzf CrombiePlots.tar.gz
for dir in `ls plots`
do
    if [ ! -d $dir ]
    then
        mkdir $dir
    fi
    mv plots/$dir/* $dir/.
    rmdir plots/$dir
done

rmdir plots
rm CrombiePlots.tar.gz
