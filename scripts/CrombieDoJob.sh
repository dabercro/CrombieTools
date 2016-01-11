#! /bin/bash

  cmsbase=$1
 macroDir=$2
  outFile=$3
   NCORES=$4
  slimmer=$5
macroList=$6

cd $cmsbase/src
eval `scram runtime -sh`
cd -

for file in `cat $macroDir/$macroList`
do
    if [ "$file" != "${file/\//}" ]
    then
        making=${file%%/${file##*/}}
        if [ ! -d $making ]
        then
            mkdir -p $making
        fi
    fi
    cp $macroDir/$file $file
done

cp "${outFile%.*}".txt . 

echo "Trying to make $outFile"
echo ""
echo "Using "$NCORES" cores!"

RUNNING=0
NUM=0

./$slimmer compile

OutputBase="lxbatchTmpOutput"
CommandList="ArgsForThisJob.txt"
echo "" > $CommandList

for file in `cat "${outFile%.*}".txt`; do
    echo root://eoscms/$file $OutputBase\_$NUM.root >> $CommandList
done

cat $CommandList | xargs -n2 -P$NCORES ./$slimmer

hadd hadded.root $OutputBase\_*.root

echo ""
echo "Copying to $outFile"

cp hadded.root $outFile
