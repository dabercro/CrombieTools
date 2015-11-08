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

case ":$PYTHONPATH:" in
    *:`pwd`/python:*)
        echo "Python already is included." 
	;;
    *)
	echo "" >> $profile
	echo "# Variables for CrombieTools" >> $profile
	echo "export PYTHONPATH=\$PYTHONPATH:"`pwd`/python >> $profile
	;;
esac

source $profile
