#!/bin/bash

## @file install.sh
#  A simple install script for CrombieTools
#  @author Daniel Abercrombie <dabercro@mit.edu>

here=`pwd`

echo " # "

if [ -f ~/.bashrc ]
    then
    profile=~/.bashrc
elif [ -f ~/.bash_profile ]
    then
    profile=~/.bash_profile
else
    echo " # No profile found... Exiting."
    echo " # Make sure either ~/.bashrc or ~/.bash_profile exists!"
    echo " # "
    exit
fi

# Check for existing installation:
if [ ! -z "$CROMBIEPATH" ]
then
    echo " # You seem to already have an installation at $CROMBIEPATH"
    echo " # I will check that your \$PATH and \$PYTHONPATH point to it."
    echo " # If you are still having trouble, try"
    echo " # "
    echo " #  > CrombieClean"
    echo " #  > CompileCrombieTools"
    echo " # "
    targetDir=$CROMBIEPATH
else
    targetDir=$here
fi

source $profile

target=$targetDir/python
case ":$PYTHONPATH:" in
    *:$target:*)
        echo " # Python already is included." 
	;;
    *)
	echo "" >> $profile
	echo "# Python objects in CrombieTools" >> $profile
	echo "export PYTHONPATH=\$PYTHONPATH:"$target >> $profile
	;;
esac

target=$targetDir/bin
case ":$PATH:" in
    *:$target:*)
        echo " # Executables are already included." 
	;;
    *)
	echo "" >> $profile
	echo "# Executables in CrombieTools" >> $profile
	echo "export PATH=\$PATH:"$target >> $profile
	;;
esac

target=$targetDir
if [ "$CROMBIEPATH" = "" ]
then
    echo "" >> $profile
    echo "# Location of CrombieTools" >> $profile
    echo "export CROMBIEPATH="$target >> $profile
fi

if [ "$CROMBIEDATE" = "" ]
then
    echo "" >> $profile
    echo "# Date format used for versioning" >> $profile
    echo "export CROMBIEDATE=\`date +%y%m%d\`" >> $profile
fi

completeline=". \$CROMBIEPATH/bin/crombie_subs.sh"

if ! grep '^. \$CROMBIEPATH/bin/crombie_subs.sh' $profile > /dev/null
then
    echo " #"
    echo " # Adding crombie autocomplete to your profile"
    echo " #"
    echo "" >> $profile
    echo "# Autocomplete for crombie subcommands." >> $profile
    echo $completeline >> $profile
else
    echo " # Autocomplete already included"
fi

cd $here

echo " # "
echo " # -------------------------------------------------------------"
echo " #  Now execute the following command:"
echo " # "
echo " # > source $profile"
echo " # "
echo " #  Or just log out and log back in or open a new bash shell."
if [ "$here" = "$targetDir" ]
then
    echo " # "
    echo " #  After that, I recommend running the test!"
    echo " # "
    echo " #  > cd test"
    echo " #  > ./test.sh"
    echo " # "
    echo " #  Or at the very least :"
    echo " # "
    echo " #  > CompileCrombieTools"
    echo " # "
    echo " #  which will be in your path. That will prevent possible error"
    echo " #  messages later from evironment manipulation."
fi
echo " # -------------------------------------------------------------"
echo " # "
