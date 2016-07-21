#!/bin/bash

if [ ! -d figs ]
then
    echo "You have no figs directory here!"
    exit 0
fi

destination=backup_slides.tex
> $destination

for found in `find figs -name "*.pdf" | rev | sort | rev`
do
    f=${found##figs/}

    if grep $f *.tex
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