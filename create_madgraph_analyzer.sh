#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LHEGZFILE=$1
gunzip $LHEGZFILE
LHEFILENAME=${LHEGZFILE/.lhe.gz/.lhe} 
ROOTFILENAME=${LHEGZFILE/.lhe.gz/.root}
python ${DIR}/lhe2root.py ${LHEFILENAME} ${ROOTFILENAME}
makeclass.sh -f -x $ROOTFILENAME Physics LHE tas lhe
cp $DIR/process_madgraph.cc_template process.cc
cp $DIR/plot_madgraph_analyzer.py .
