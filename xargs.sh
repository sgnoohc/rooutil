#!/bin/bash

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

. <(curl -sLo- "https://git.io/progressbar")

bar::start

# run the job in parallel
xargs --arg-file=${MACRO} \
      --max-procs=$cores  \
      --replace \
      --verbose \
      /bin/sh -c "{}" > ${MACROLOG} 2>&1 &

while [[ -n $(jobs -r) ]]; do
  NTOTALJOBS=$(wc -l ${MACRO} | awk '{print $1}')
  NJOBSDONE=$(wc -l ${MACROLOG} | awk '{print $1}')
  bar::status_changed ${NJOBSDONE} ${NTOTALJOBS}
  sleep 1;
done

rm -f ${MACRO}
rm -f ${MACROLOG}

#eof
