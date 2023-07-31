#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export SCRAM_ARCH=el8_amd64_gcc10
export CMSSW_VERSION=CMSSW_12_5_0
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
eval `scramv1 runtime -sh`
cd - > /dev/null

echo 'Setup following ROOT'
which root

#eof
