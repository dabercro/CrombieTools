#! /bin/bash

## \file generateDocs.sh
#  Generates Doxygen files if doxygen is installed.
#  If the user is me (dabercro), also has a subcommand to 
#  copy the resulting files to CERN's AFS space.
#  \author Daniel Abercrombie

copy=$1

if [ `which doxygen` != "" ]
then
    pdfName=CrombieToolsManual.pdf
    doxygen docs/CrombieDocs.cfg
    if [ "$copy" != "test" ]     # In this case, I'm just testing Doxygen configs
    then
        cd docs/latex
        make
        mv refman.pdf $pdfName
        cd -
    fi
    if [ "$USER" = "dabercro" ] && [ "$copy" = "copy" ]
    then
        targetDir=/afs/cern.ch/user/d/dabercro/www/CrombieToolsDocs
        if [ `which gtar` = "" ]  # Macs use BSD tar by default, so I've installed gtar
        then
            useTar=tar
        else
            useTar=gtar
        fi
        $useTar -czf - docs/html/* docs/latex/$pdfName | ssh lxplus.cern.ch "cd $targetDir ; rm -rf search ; tar -xzf - ; mv docs/html/* . ; mv docs/latex/$pdfName ."
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
