#! /bin/bash

dir=$1
row=$2
sigExpr=$3

if [ "$row" = "" ]
then
    echo ""
    echo " +---------------------------------------------------------------+"
    echo " | First argument is the directory to put the page. (Can be '.') |"
    echo " | Second argument is number of images per row.                  |"
    echo " | The optional third argument is an expression to be matched    |"
    echo " | for plots to blind. (They will not appear on the webpage.     |"
    echo " +---------------------------------------------------------------+"
    echo ""
    exit
fi

if [ ! "$sigExpr" = "" ]
then
    if [ ! -d $dir/SIGNAL--DONT_LOOK ]
    then
        mkdir $dir/SIGNAL--DONT_LOOK
    fi
    
    if ls $dir/*$sigExpr* 1> /dev/null 2>&1
    then
        mv $dir/*$sigExpr* $dir/SIGNAL--DONT_LOOK/.
    fi

    CrombieMakeWebPage.sh $dir/SIGNAL--DONT_LOOK $row
fi
    
F=Display.html

if [ -f $dir/$F ]
then
    rm $dir/$F
fi

echo "<html>" > $F
echo "<center>" >> $F

count=0

echo '<table border="0" cellspacing="5" width="100%">' >> $F

for pic in `ls $dir`
do

    extension="${pic##*.}"
    filename="${pic%.*}"

    if [ "$extension" = "png" ]
    then 
        if [ `expr $count % $row` -eq 0 ]
	then
            echo '<tr>' >> $F
        fi
        
        echo '<td width="20%">' >> $F
        echo '<p> <a href="'$filename'.C">'"${pic%.*}.C"'</a></p>' >> $F
        echo '<a href="'$filename'.pdf"><img src="'$pic'" alt="'$pic'" width="100%"></a>' >> $F
        echo '</td>' >> $F
        
        count=$((count + 1))
        if [ `expr $count % $row` -eq 0 ]
	then
            echo '</tr>' >> $F
        fi
    fi

done

echo '</table>' >> $F

echo "</center>" >> $F
echo "</html>" >> $F

mv $F $dir/$F