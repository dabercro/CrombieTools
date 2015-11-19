#! /bin/bash

dir=$1
row=$2
sig=$3
sigExpr=$4

if [ "$sig" = "sig" ]; then
    if [ ! -d $dir/SIGNAL--DONT_LOOK ]; then
        mkdir $dir/SIGNAL--DONT_LOOK
    fi
    
    if ls $dir/*$sigExpr* 1> /dev/null 2>&1; then
        mv $dir/*$sigExpr* $dir/SIGNAL--DONT_LOOK/.
    fi

    ./makePage.sh $dir/SIGNAL--DONT_LOOK
fi
    
F=Display.html

if [ -f $dir/$F ]; then
    rm $dir/$F
fi

echo "<html>" > $F
echo "<center>" >> $F

count=0

echo '<table border="0" cellspacing="5" width="100%">' >> $F

for pic in `ls $dir`; do

    extension="${pic##*.}"
    filename="${pic%.*}"

    if [ "$extension" = "png" ]; then 
        if [ `expr $count % $row` -eq 0 ]; then
            echo '<tr>' >> $F
        fi
        
        echo '<td width="20%">' >> $F
        echo '<p> <a href="'$filename'.C">'"${pic%.*}.C"'</a></p>' >> $F
        echo '<a href="'$filename'.pdf"><img src="'$pic'" alt="'$pic'" width="100%"></a>' >> $F
        echo '</td>' >> $F
        
        count=$((count + 1))
        if [ `expr $count % $row` -eq 0 ]; then
            echo '</tr>' >> $F
        fi
    fi

done

echo '</table>' >> $F

echo "</center>" >> $F
echo "</html>" >> $F

mv $F $dir/$F