#! /bin/bash

cmsbase=$1
outFile=$2

macroDir=$LS_SUBCWD

source $macroDir/CrombieSlimmingConfig.sh

cd $cmsbase/src
eval `scram runtime -sh`
cd -

for file in `cat $macroDir/$CrombieJobScriptList`
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
echo "Using "$CrombieNumberProcs" cores!"

RUNNING=0
NUM=0

./$CrombieSlimmerScript compile

OutputBase="lxbatchTmpOutput"
CommandList="ArgsForThisJob.txt"
echo "" > $CommandList

for file in `cat "${outFile%.*}".txt`; do
    echo root://eoscms/$file $OutputBase\_$NUM.root >> $CommandList
    NUM=$((NUM + 1))
done

cat $CommandList | xargs -n2 -P$CrombieNumberProcs ./$CrombieSlimmerScript

hadd $OutputBase.root $OutputBase\_*.root

echo ""
echo "Copying to $outFile"

cp $OutputBase.root $outFile
