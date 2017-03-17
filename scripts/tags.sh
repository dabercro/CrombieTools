#! /bin/bash

# Just echo back the -I and -L tags for compiling executables.
# -l will have to be added separately for each library.

for d in $CROMBIEPATH/*/interface
do
    echo -n "-I$d "
done

echo "-L$CROMBIEPATH/lib"
