#! /bin/bash

isEOS=$1

source CrombieSlimmingConfig.sh

if [ "$CrombieDirList" = "" ]
then
    CrombieDirList=$CrombieFullDir/CrombieDirList.txt
    if [ "$isEOS" = "eos" ]
    then
        /afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select ls $CrombieEosDir > $CrombieDirList
    else
        ls $CrombieRegDir > $CrombieDirList
    fi
fi

if [ ! -d bout ]
then
    echo "Making log output directory as ./bout"
    mkdir bout
fi 

if [ ! -d $CrombieFullDir ]
then
    mkdir -p $CrombieFullDir
fi

if [ ! -d $CrombieTempDir ]
then
    mkdir -p $CrombieTempDir
else
    rm $CrombieTempDir/*.txt
    if [ "$fresh" = "fresh" ]
    then
        rm $CrombieTempDir/*.root
    fi
fi

if [ MonoJetTree.txt -nt MonoJetTree.h ]
then
    ./makeTree.sh
fi

> $haddFile

ranOnFile=0

lastDir=''

count=0

for dir in `cat $CrombieDirList`
do
    fileInCount=$CrombieFilesPerJob

    reasonableName="${dir%%/*}"
    reasonableName="${reasonableName%%_Tune*}"
    reasonableName="${reasonableName%%-madgraph*}"

    if [ "$reasonableName" != "$lastDir" ]
    then 
        count=0
        lastDir=$reasonableName
        echo "$CrombieFullDir/monojet_$reasonableName.root $CrombieTempDir/monojet_"$reasonableName"_*.root" >> $haddFile
    fi

    if [ "$isEOS" = "eos" ]
    then
        for inFile in `/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select find $CrombieEosDir/$dir`
        do
            if [ "${inFile##*_}" = "pilot.root" -o "${inFile##*.}" != "root" ]
            then
                continue
            fi
            inFile="${inFile##*$CrombieEosDir/$dir/}"
            
            if [ "$fileInCount" -eq "$CrombieFilesPerJob" ]
            then
                fileInCount=0
                count=$((count + 1))
                currentConfig=$CrombieTempDir/monojet_$reasonableName\_$count.txt
                > $currentConfig
            fi
            echo $CrombieEosDir/$dir/$inFile >> $currentConfig
            fileInCount=$((fileInCount + 1))
        done
    else
        for inFile in `find $CrombieRegDir/$dir -name '*.root'`; do
            if [ "${inFile##*_}" = "pilot.root" ]; then
                continue
            fi
            if [ "$fileInCount" -eq "$CrombieFilesPerJob" ]
            then
                fileInCount=0
                count=$((count + 1))
                currentConfig=$outDir/monojet_$reasonableName\_$count.txt
                > $currentConfig
            fi
            echo $inFile $CrombieTempDir/TerminalRunning/monojet_$reasonableName\_$count\_$indexFiles.root >> $currentConfig
            fileInCount=$((fileInCount + 1))
            indexFiles=$((indexFiles + 1))
        done
    fi
done
