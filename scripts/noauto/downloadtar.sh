#!/bin/bash

wget -O CrombiePlots.tar.gz "$1"   # Get the tar file from the input URL

tar -xzf CrombiePlots.tar.gz       # Extract the tar file

for dir in `ls plots`              # For each directory
do

    if [ ! -d $dir ]               # Make directories that are missing
    then

        mkdir $dir

    fi

    mv plots/$dir/* $dir/.         # Move output out of plots
    rmdir plots/$dir               # Remove the subdirectory

done

rmdir plots                        # Clean up the directory
rm CrombiePlots.tar.gz             #   and tarball
