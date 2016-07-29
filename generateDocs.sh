#!/bin/bash

##
#  @file generateDocs.sh
#
#  Generates Doxygen files if doxygen is installed.
#  If the user is me (dabercro), also has a subcommand `copy`
#  to copy the resulting files to CERN's AFS space.
#
#  @author Daniel Abercrombie
#

 copy=$1
where=$2

if [ `which doxygen 2> /dev/null` != "" ]
then

    pdfName=CrombieToolsManual.pdf

    if [ "$USER" = "dabercro" ] && [ "$copy" = "copy" ]     # If I am me, can copy to the appropriate server
    then

        case $where in
            lxplus)                                         # The official location linked to on GitHub
                targetHost=lxplus.cern.ch
                targetDir=/afs/cern.ch/user/d/dabercro/www/CrombieToolsDocs
                ;;
            athena)
                targetHost=athena.dialup.mit.edu
                targetDir=/afs/athena.mit.edu/user/d/a/dabercro/www/CrombieToolsDocs
                ;;
            *)                                              # Default location, because passwordless
                targetHost=t3desk003.mit.edu
                targetDir=/home/dabercro/public_html/CrombieToolsDocs
                ;;
        esac

        if [ `which gtar 2> /dev/null` = "" ]               # Macs use BSD tar by default, so I've installed gtar
        then
            useTar=tar
        else
            useTar=gtar
        fi

        echo "Copying documentation to $targetHost."        # Stream tar over ssh and untar at server
        $useTar -czf - docs/html/* docs/latex/$pdfName |
            ssh $targetHost "mkdir -p $targetDir 2> /dev/null ; cd $targetDir ; rm -rf search ; tar -xzf - ; mv docs/html/* . ; mv docs/latex/$pdfName ."

    else

        doxygen docs/CrombieDocs.cfg                        # If not copying, create the documentation

        if [ "$copy" != "test" ]                            # If testing, just make the html
        then                                                #   otherwise, make the LaTeX manual too

            cd docs/latex
            make
            mv refman.pdf $pdfName
            cd -

        fi
        
        if [ `cat doxygen.log | wc -l` -eq 0 ]              # If the doxygen.log is empty remove it
        then                                                #   otherwise, leave it for reading warnings

            rm doxygen.log

        fi
    fi

else                                                        # If doxygen is not installed on system,
                                                            #   give user some info on doxygen installation
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
