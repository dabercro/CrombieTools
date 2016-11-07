#!/bin/bash

##
# @file include/link.sh
#
# Softlinks compiled shared objects in CrombieTools/include directory
# with all the appropriate header files.
# This is only used for convenience if you are compiling C macros.
#
# @author Daniel Abercrombie <dabercro@mit.edu>
#

for file in `ls ../*Tools/interface/*.h`
do

    filename=${file##*/}

    ln -s $file $filename

done
