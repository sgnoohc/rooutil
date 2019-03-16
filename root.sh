#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if $(type root &> /dev/null); then
    :
else
    #echo "Setting up ROOT"
    export SCRAM_ARCH=slc6_amd64_gcc530   # or whatever scram_arch you need for your desired CMSSW release
    export CMSSW_VERSION=CMSSW_9_2_0
    source /cvmfs/cms.cern.ch/cmsset_default.sh
    cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
    eval `scramv1 runtime -sh`
    cd - > /dev/null
fi

echo 'Setup following ROOT'
which root

if [ -z "$1" ]; then
    :
else
    root -l $1
fi

#eof
