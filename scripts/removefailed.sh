#!/bin/bash

for f in `ls failed`
do
    if [ -f $f ]
    then
        if [ `stat -c %s $f` -ge `stat -c %s failed/$f` ]
        then
            rm failed/$f
        else
            echo "failed/$f is bigger than $f"
        fi
    else
        echo "$f has not been created yet. You need to resubmit the failed job (or it's running)."
    fi
done

rmdir failed
