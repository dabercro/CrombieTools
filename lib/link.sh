#!/bin/bash

##
# @file lib/link.sh
#
# Softlinks compiled shared objects in CrombieTools/lib directory
# with proper library names.
# This is only used for convenience if you are compiling C macros.
#
# @author Daniel Abercrombie <dabercro@mit.edu>
#

for file in `ls ../*Tools/src/*.so`
do

    filename=${file##*/}
    classname=${filename%%_cc.so}

    test -f lib${classname}.so || ln -s $file lib${classname}.so

done
