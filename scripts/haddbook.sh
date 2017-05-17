#! /bin/bash

export book=$1
shift
datasetlist=$1
shift
ncores=$1
shift

if [ "$ncores" = "" ] || [ ! -f "$datasetlist" ]
then

    echo ""
    echo "Usage: $0 <book> <datasetlist> <ncores>"
    echo ""
    echo "<datasetlist> must be the name of a text file"
    echo "with a list of datasets to hadd together."
    echo ""
    exit 0

fi

_do_hadd () {

    dataset=$1
    hadd /data/t3home000/dabercro/$book/$1.root /mnt/hadoop/cms/store/user/paus/$book/*$1*/*.root

}

export -f _do_hadd

test -d /data/t3home000/dabercro/$book || mkdir -p /data/t3home000/dabercro/$book

cat $datasetlist | awk -F "+" '{ print $1 }' | uniq | xargs -P$ncores bash -c '_do_hadd "$@"'
