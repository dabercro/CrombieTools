#! /bin/bash

copy=$1

if [ `which doxygen` != "" ]
then
    doxygen docs/CrombieDocs.cfg
    cd docs/latex
    make
    mv refman.pdf CrombieToolsManual.pdf
    cd -
    if [ "$USER" = "dabercro" ] && [ "$copy" = "copy" ]
    then
        targetDir=lxplus.cern.ch:/afs/cern.ch/user/d/dabercro/www/CrombieToolsDocs/.
        scp -r docs/html/* $targetDir
        scp docs/latex/CrombieToolsManual.pdf $targetDir
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
