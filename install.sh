#! /bin/bash

# A simple install script for CrombieTools

profile=0

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

target=`pwd`/python
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

target=`pwd`/bin
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

source $profile
