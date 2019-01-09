#!/bin/bash

# vim: tabstop=2:softtabstop=2:shiftwidth=2:expandtab

#  .
# ..: P. Chang, philip@physics.ucsd.edu

cores=36

# must provide the job
if [ "x${1}" == "x" ]; then
  echo "Must provide the job commands txt file"
  exit
fi

JOBTXTFILE=$1

MACRONAME=$(mktemp stupid_numbers_XXXXXXXXX)
MACRO=/tmp/${MACRONAME}.txt

# filter some jobs
if [ "x${2}" != "x" ]; then
  cat $1 | grep -v \# | grep $2 > ${MACRO}
else
  cat $1 | grep -v \# > ${MACRO}
fi

# run the job in parallel
xargs --arg-file=${MACRO} \
      --max-procs=$cores  \
      --replace \
      --verbose \
      /bin/sh -c "{}"

#eof
