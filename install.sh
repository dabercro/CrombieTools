#! /bin/bash

# A simple install script for CrombieTools

profile=0
here=`pwd`

if [ -f ~/.bashrc ]
    then
    profile=~/.bashrc
elif [ -f ~/.bash_profile ]
    then
    profile=~/.bash_profile
else
    echo "No profile found... Exiting."
    exit
fi

source $profile

target=$here/python
case ":$PYTHONPATH:" in
    *:$target:*)
        echo "Python already is included." 
	;;
    *)
	echo "" >> $profile
	echo "# Python objects in CrombieTools" >> $profile
	echo "export PYTHONPATH=\$PYTHONPATH:"$target >> $profile
	;;
esac

target=$here/bin
case ":$PATH:" in
    *:$target:*)
        echo "Executables are already included." 
	;;
    *)
	echo "" >> $profile
	echo "# Executables in CrombieTools" >> $profile
	echo "export PATH=\$PATH:"$target >> $profile
	;;
esac

target=$here
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

cd $here

echo "-------------------------------------------------------------"
echo " Now execute the following command:"
echo ""
echo "> source $profile"
echo ""
echo " Or just log out and log back in or open a new bash shell."
echo ""
echo " After that, I recommend running :"
echo ""
echo "> CompileCrombieTools"
echo ""
echo " which will be in your path. That will prevent possible error"
echo " messages later from evironment manipulation."
echo "-------------------------------------------------------------"
echo ""
