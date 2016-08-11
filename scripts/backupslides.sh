#!/bin/bash

##
#  @file backupslides.sh
#
#  Creates backup slides using all of the pdf images
#  in the local figs directory.
#
#  @author Daniel Abercrombie
#

figs=figs

if [ ! -d $figs ]
then
    echo "You have no $figs directory here!" >&2
    exit 10
fi

destination=backup_slides.tex
> $destination

for found in `find $figs -name "*.pdf" | rev | sort | rev`
do
    f=${found##$figs/}

    if grep $f *.tex > /dev/null
    then
        continue
    fi
    title=${f%%.pdf}
    echo "\begin{frame}" >> $destination
    echo "   \frametitle{\small ${title//_/\_}}" >> $destination
    echo "   \centering" >> $destination
    echo "   \includegraphics[width=0.7\linewidth]{$f}" >> $destination
    echo "\end{frame}" >> $destination
    echo "" >> $destination
done