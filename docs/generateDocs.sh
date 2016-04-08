#!/bin/bash

## \file generateDocs.sh
#  Generates Doxygen files if doxygen is installed.
#  If the user is me (dabercro), also has a subcommand to 
#  copy the resulting files to CERN's AFS space.
#  \author Daniel Abercrombie

 copy=$1
where=$2

if [ `which doxygen 2> /dev/null` != "" ]
then
    pdfName=CrombieToolsManual.pdf
    doxygen CrombieDocs.cfg
    if [ "$copy" != "test" ]                     # In this case, I'm just testing making html fast
    then
        cd latex
        make
        mv refman.pdf $pdfName
        cd -
    fi
    if [ "$USER" = "dabercro" ] && [ "$copy" = "copy" ]
    then

        case $where in
            lxplus)
                targetHost=lxplus.cern.ch
                targetDir=/afs/cern.ch/user/d/dabercro/www/CrombieToolsDocs
                ;;
            athena)
                targetHost=athena.dialup.mit.edu
                targetDir=/afs/athena.mit.edu/user/d/a/dabercro/www/CrombieToolsDocs
                ;;

            *)                                   # Default location, because passwordless
                targetHost=t3desk003.mit.edu
                targetDir=/home/dabercro/public_html/CrombieToolsDocs
                ;;
        esac

        if [ `which gtar 2> /dev/null` = "" ]    # Macs use BSD tar by default, so I've installed gtar
        then
            useTar=tar
        else
            useTar=gtar
        fi
        echo "Copying documentation to $targetHost."
        $useTar -czf - html/* latex/$pdfName |
            ssh $targetHost "mkdir -p $targetDir 2> /dev/null ; cd $targetDir ; rm -rf search ; tar -xzf - ; mv html/* . ; mv latex/$pdfName ."
    fi
else
    echo "You need the 'doxygen' package to" 
    echo "generate the documentation."
    echo "See this page for more information:"
    echo ""
    echo "http://www.stack.nl/~dimitri/doxygen/"
    echo ""
    echo "You will also need the graphviz package"
    echo "to use the configuration as is:"
    echo ""
    echo "http://www.graphviz.org/"
    echo ""
    echo "If you don't want to install these, check out the main page of my documentation here:"
    echo ""
    echo "http://dabercro.web.cern.ch/dabercro/CrombieToolsDocs/"
    echo ""
fi
