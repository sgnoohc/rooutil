#!/bin/bash

trap "kill 0" SIGINT

# vim: tabstop=2:softtabstop=2:shiftwidth=2:expandtab

#  .
# ..: P. Chang, philip@physics.ucsd.edu

usage()
{
  echo "Usage:"
  echo "   sh $0 [-n NCORE=36] COMMAND_LIST.txt [PATTERN]"
  exit
}

# Command-line opts
while getopts ":n:xh" OPTION; do
  case $OPTION in
    n) CORE=${OPTARG};;
    h) usage;;
    :) usage;;
  esac
done

# To shift away the parsed options
shift $(($OPTIND - 1))

if [ -z ${CORE} ]; then CORE=36; fi

cores=${CORE}

# must provide the job
if [ "x${1}" == "x" ]; then
  echo "Error: Must provide the job commands txt file"
  usage
  exit
fi

JOBTXTFILE=$1

MACRONAME=$(mktemp stupid_numbers_XXXXXXXXX)
MACRO=/tmp/${MACRONAME}.txt
MACROLOG=/tmp/${MACRONAME}.log
rm $MACRONAME

# filter some jobs
if [ "x${2}" != "x" ]; then
    cat $1 | grep -v '^#' | grep $2 > ${MACRO}
else
    cat $1 | grep -v '^#' > ${MACRO}
fi

. <(curl -s https://raw.githubusercontent.com/roddhjav/progressbar/v1.1/progressbar.sh)

# run the job in parallel
xargs --arg-file=${MACRO} \
      --max-procs=$cores  \
      --replace \
      --verbose \
      /bin/sh -c "{}" > ${MACROLOG} 2>&1 &

while [[ -n $(jobs -r) ]]; do
    NTOTALJOBS=$(wc -l ${MACRO} | awk '{print $1}')
    NJOBSSTARTED=$(wc -l ${MACROLOG} | awk '{print $1}')
    child_count=$(($(pgrep --parent $(jobs -p) | wc -l)))
    NJOBSDONE=$((NJOBSSTARTED - child_count))
    progressbar "Running ${JOBTXTFILE} in parallel..." ${NJOBSDONE} ${NTOTALJOBS}
    sleep 1;
done

wait

rm -f ${MACRO}
rm -f ${MACROLOG}

#eof
