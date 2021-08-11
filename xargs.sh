#!/bin/bash

trap "kill 0" SIGINT

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

# . <(curl -s https://raw.githubusercontent.com/roddhjav/progressbar/v1.1/progressbar.sh)
# https://gist.github.com/mllamazares/c412b31bde94c7ad5c30
function ProgressBar {

    actual="$1"
    total="$2"
    running="$3"

    let progreso=(${actual}*100/${total}*100)/100
    let realizado=(${progreso}*4)/10
    let queda=40-$realizado

    if [[ ${progreso} -lt 59 ]]; then 
      color="91"
    elif [[ ${progreso} -lt 73 ]]; then 
      color="94"
    elif [[ ${progreso} -lt 86 ]]; then 
      color="93"
    else
      color="92"
    fi

    timestamp=$(date +%H:%M:%S)

    lleno=$(printf "%${realizado}s")
    vacio=$(printf "%${queda}s")

    tput cuu 1 && tput el

    if [[ ${actual} == ${total} ]]; then
        printf "\rXArgs.sh:: ${timestamp}: Running... [\e[7;${color}m${lleno// /#}\e[00m${vacio// /-}] \e[1;${color}m${progreso}%%\e[00m [${actual}/${total} done]\n"
    else
        printf "\rXArgs.sh:: ${timestamp}: Running... [\e[7;${color}m${lleno// /#}\e[00m${vacio// /-}] \e[1;${color}m${progreso}%%\e[00m [${actual}/${total} done, ${running} running...]\n"
    fi
        
}

function iniProgressBar {
    printf "\n"
}

# run the job in parallel
xargs --arg-file=${MACRO} \
      --max-procs=$cores  \
      --replace \
      /bin/sh -c "echo \"XARGSSHJOBSTART\" >> ${MACROLOG}; {}" &
      # --verbose \

sleep 1;

echo "\\\\/ ________"                
echo "/\\\\ XArgs.sh"
starttimestamp=$(date +%H:%M:%S)
echo "XArgs.sh:: ${starttimestamp}: Start running script ${JOBTXTFILE} in parallel"
iniProgressBar
while [[ -n $(jobs -r) ]]; do
    JOBS=$(jobs -p)
    if [ -z "${JOBS}" ]; then
        break
    fi
    child_count=$(($(pgrep --parent ${JOBS} | wc -l)))
    NTOTALJOBS=$(wc -l ${MACRO} | awk '{print $1}')
    NJOBSSTARTED=$(cat ${MACROLOG} | grep "XARGSSHJOBSTART" | wc | awk '{print $1}')
    NJOBSDONE=$((NJOBSSTARTED - child_count))
    # progressbar "+ Running jobs in parallel..." ${NJOBSDONE} ${NTOTALJOBS}
    ProgressBar ${NJOBSDONE} ${NTOTALJOBS} ${child_count}
    sleep 1;
done

# One last check to reach that 100% progress bar
NTOTALJOBS=$(wc -l ${MACRO} | awk '{print $1}')
NJOBSSTARTED=$(cat ${MACROLOG} | grep "XARGSSHJOBSTART" | wc | awk '{print $1}')
NJOBSDONE=$((NJOBSSTARTED))
ProgressBar ${NJOBSDONE} ${NTOTALJOBS}
sleep 1;
endtimestamp=$(date +%H:%M:%S)
echo "XArgs.sh:: ${endtimestamp}: Done!"

wait

rm -f ${MACRO}
rm -f ${MACROLOG}

#eof
