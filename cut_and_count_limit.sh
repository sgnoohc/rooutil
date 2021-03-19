#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

function col {
if [ $# -lt 1 ]; then
    echo "usage: col <col #>"
    return 1
fi
num=$1

if [[ $num -lt 0 ]]; then 
    awk "{print \$(NF+$((num+1)))}"
else
    awk -v x=$num '{print $x}'
fi
}

root -l -b -q $DIR/cut_and_count_limit.C\($1,$2,$3\) | grep "95 percent" | col 5
