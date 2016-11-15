#!/bin/bash

##
#  @file install.sh
#
#  A simple install script for CrombieTools
#
#  @author Daniel Abercrombie <dabercro@mit.edu>
#

here=`pwd`                    # Some shells drop you off in the wrong place, so hang on to current location

echo " # "                    # This is a verbose script

if [ -f ~/.bashrc ]           # Search for a bash rc or profile file
    then
    profile=~/.bashrc
elif [ -f ~/.bash_profile ]
    then
    profile=~/.bash_profile
else                          # If none exists, I don't know how to automate installation
    echo " # No profile found... Exiting."
    echo " # Make sure either ~/.bashrc or ~/.bash_profile exists!"
    echo " # "
    exit
fi

if [ ! -z "$CROMBIEPATH" ]    # Check for existing installation
then                          #   If it exists, check that everything's set up right

    echo " # You seem to already have an installation at $CROMBIEPATH"
    echo " # I will check that your \$PATH and \$PYTHONPATH point to it."
    echo " # If you are still having trouble, try"
    echo " # "
    echo " #  > crombie clean"
    echo " #  > crombie compile"
    echo " # "

    targetDir=$CROMBIEPATH

else                          # Otherwise make a fresh installation from this repository

    targetDir=$here

fi

source $profile                # Make sure we're not missing any recent additions to the $profile

target=$targetDir/python       # Search PYTHONPATH for CrombieTools/python

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

target=$targetDir/bin          # Search PATH for CrombieTools/bin

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

target=$targetDir              # If $CROMBIEPATH not set, add it to profile

if [ "$CROMBIEPATH" = "" ]
then

    echo "" >> $profile
    echo "# Location of CrombieTools" >> $profile
    echo "export CROMBIEPATH="$target >> $profile

fi

if [ "`type CROMBIEDATE 2> /dev/null`" = "" ]    # If $CROMBIEDATE not set (used for automatic versioning),
then                                             #   add it to profile

    echo "" >> $profile
    echo "# Date format used for versioning" >> $profile
    echo "CROMBIEDATE () {" >> $profile
    echo "    echo \`date +%y%m%d\`" >> $profile
    echo "}" >> $profile
    echo "" >> $profile
    echo "export -f CROMBIEDATE" >> $profile

fi

# Check that the autocomplete line is present in the bash rc or profile

completeline="test -f \$CROMBIEPATH/bin/crombie_completion.sh && . \$CROMBIEPATH/bin/crombie_completion.sh"

if ! grep "^test -f \$CROMBIEPATH/bin/crombie_completion.sh" $profile > /dev/null
then

    echo " #"
    echo " # Adding crombie autocomplete to your profile"
    echo " #"
    echo "" >> $profile
    echo "# Auto completion for crombie subcommands." >> $profile
    echo $completeline >> $profile

else

    echo " # Autocomplete already included"

fi

cd $here                        # Make sure the put the user back where they started

# Give some recommended next actions for the user

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
    echo " #  > crombie compile"
    echo " # "
    echo " #  which will be in your path. That will prevent possible error"
    echo " #  messages later from evironment manipulation."

fi

echo " # -------------------------------------------------------------"
echo " # "
