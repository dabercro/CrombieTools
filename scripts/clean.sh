#!/bin/bash

object=$1

find $CROMBIEPATH/*Tools -name "$object*.so"  | xargs rm 2> /dev/null
find $CROMBIEPATH/*Tools -name "$object*.d"   | xargs rm 2> /dev/null
find $CROMBIEPATH/*Tools -name "$object*.pcm" | xargs rm 2> /dev/null
find $CROMBIEPATH -name "$object*.pyc" | xargs rm 2> /dev/null

pycacheDir=$CROMBIEPATH/python/CrombieTools/__pycache__
if [ -d $pycacheDir ]
then
    rmdir $pycacheDir
fi