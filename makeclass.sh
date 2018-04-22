#!/bin/bash

# Neat bash trick to get the path where this file sits
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Loading pretty print utilities (colorized with some icons)
source $DIR/utils.sh
ERROR=e_error
UL=e_underline
SUCCESS=e_success


echo ""
echo "========================================================================================================="
echo ""
echo ""
echo "                                       MakeClass script"
echo ""
echo ""
echo "========================================================================================================="

usage(){
    echo ""
    echo "This script creates CLASSNAME.cc and CLASSNAME.h file."
    echo "The generated source C++ codes can be used to read a ROOT's TTree with rest of the TasUtil tool at ease."
    echo "The generation of the code relies on:"
    echo "  > https://github.com/cmstas/Software/blob/master/makeCMS3ClassFiles/makeCMS3ClassFiles.C"
    echo ""
    echo "Usage:"
    echo ""
    echo ${green}"  > sh $(basename $0) [-f] [-h] [-x] ROOTFILE TTREENAME CLASSNAME [NAMESPACENAME=tas] [CLASSINSTANCENAME=mytree] "${reset}
    echo ""
    echo ""
    echo ${green}" -h ${reset}: print this message"
    echo ${green}" -f ${reset}: force run this script"
    echo ${green}" -x ${reset}: create additional looper template files (i.e. ScanChain.C, doAll.C, compile.sh, run.sh, submit_batch.sh)"
    echo ""
    echo ${green}" ROOTFILE          ${reset}= Path to the root file that holds an example TTree that you wish to study."
    echo ${green}" TREENAME          ${reset}= The TTree object TKey name in the ROOTFILE"
    echo ${green}" CLASSNAME         ${reset}= The name you want to give to the class you are creating"
    echo ${green}" NAMESPACENAME     ${reset}= The name you want to give to the namespace for accessing the ttree"
    echo ${green}" CLASSINSTANCENAME ${reset}= The name of the global instance of the class that you are trying to create"
    echo "                     (defaults to 'mytree')"
    echo ""
    e_underline "${red}NOTE: If no argument is given, it will assume to create a CMS3 looper"${reset}
    echo ""
    echo ""
    echo ""
    exit
}

# Command-line opts
while getopts ":fxh" OPTION; do
  case $OPTION in
    f) FORCE=true;;
    x) GENERATEEXTRACODE=true;;
    h) usage;;
    :) usage;;
  esac
done

# To shift away the parsed options
shift $(($OPTIND - 1))

if [ -z $1 ]; then
    if [ -z $2 ]; then
        if [ -z $3 ]; then
            echo "TasUtil:: No argument is given"
            echo "TasUtil:: The script will assume to create a CMS3 looper"
        fi
    fi
fi

if [ -z $1 ]; then usage; fi
if [ -z $2 ]; then usage; fi
if [ -z $3 ]; then usage; fi
ROOTFILE=$1
TTREENAME=$2
MAKECLASSNAME=$3
NAMESPACENAME=$4
TREEINSTANCENAME=$5
if [ -z ${NAMESPACENAME} ]; then NAMESPACENAME=tas; fi
if [ -z ${TREEINSTANCENAME} ]; then TREEINSTANCENAME=mytree; fi

echo ""
e_arrow "TasUtil:: The user has provided following options"
e_arrow "TasUtil:: $(date)"
e_arrow "TasUtil:: =========================================="
e_arrow "TasUtil::  ROOTFILE=$ROOTFILE"
e_arrow "TasUtil::  TREENAME=$TREENAME"
e_arrow "TasUtil::  MAKECLASSNAME=$MAKECLASSNAME"
e_arrow "TasUtil::  TREEINSTANCENAME=$TREEINSTANCENAME"
e_arrow "TasUtil:: =========================================="
e_arrow "TasUtil:: "

# Print
e_arrow "TasUtil:: Generating ${MAKECLASSNAME}.cc/h file which can load the TTree content from ${ROOTFILE}:${TREENAME} ..."

ROOTFILE=$('cd' $(dirname ${ROOTFILE}); pwd)/$(basename $1)

# Check whether the file already exists
if [ -e ${MAKECLASSNAME}.cc ]; then
    if [ "${FORCE}" == true ]; then
        :
    else
        e_error "TasUtil:: ${MAKECLASSNAME}.cc already exists!"
        e_error "TasUtil:: Do you want to override? If so, provide option -f. For more info use option -h"
        exit
    fi
fi

# Check whether the file already exists
if [ -e ${MAKECLASSNAME}.h ]; then
    if [ "${FORCE}" == true ]; then
        :
    else
        e_error "TasUtil:: ${MAKECLASSNAME}.h already exists!"
        e_error "TasUtil:: Do you want to override? If so, provide option -f. For more info use option -h"
        exit
    fi
fi

source $DIR/root.sh ""

if [ -e ~/cmstas/Software/makeCMS3ClassFiles/makeCMS3ClassFiles.C ]; then
  root -l -b -q ~/cmstas/Software/makeCMS3ClassFiles/makeCMS3ClassFiles.C\(\"${ROOTFILE}\",\"${TTREENAME}\",\"${MAKECLASSNAME}\",\"${NAMESPACENAME}\",\"${TREEINSTANCENAME}\"\) &> /dev/null
else
  git clone git@github.com:cmstas/Software.git
  root -l -b -q Software/makeCMS3ClassFiles/makeCMS3ClassFiles.C\(\"${ROOTFILE}\",\"${TTREENAME}\",\"${MAKECLASSNAME}\",\"${NAMESPACENAME}\",\"${TREEINSTANCENAME}\"\) &> /dev/null
  rm -rf Software
fi

e_arrow "TasUtil:: Generated ${MAKECLASSNAME}.cc/h successfully!"

if [ "$GENERATEEXTRACODE" == true ]; then

    if [ -e ScanChain.C ]; then
        if [ "${FORCE}" == true ]; then
            :
        else
            e_error "TasUtil:: ScanChain.C already exists!"
            e_error "TasUtil:: Do you want to override? If so, provide option -f. For more info use option -h"
            exit
        fi
    fi
    #
    # Create ScanChain.C
    #
    echo "#include \"tasutil.cc\""                                                             >  ScanChain.C
    echo "#include \"${MAKECLASSNAME}.cc\""                                                    >> ScanChain.C
    echo ""                                                                                    >> ScanChain.C
    echo "void ScanChain(TChain* chain, TString outputname, int nEvents=-1)"                   >> ScanChain.C
    echo "{"                                                                                   >> ScanChain.C
    echo "    TasUtil::Looper<${MAKECLASSNAME}> looper(chain, &${TREEINSTANCENAME}, nEvents);" >> ScanChain.C
    echo "    while (looper.nextEvent())"                                                      >> ScanChain.C
    echo "    {"                                                                               >> ScanChain.C
    echo "        // Do what you need to do in for each event here"                            >> ScanChain.C
    echo "    }"                                                                               >> ScanChain.C
    echo "}"                                                                                   >> ScanChain.C

    if [ -e ScanChain.C ]; then
        if [ "${FORCE}" == true ]; then
            :
        else
            e_error "TasUtil:: doAll.C already exists!"
            e_error "TasUtil:: Do you want to override? If so, provide option -f. For more info use option -h"
            exit
        fi
    fi
    #
    # Create doAll.C
    #
    echo "#include \"ScanChain.C\""                                                   >  doAll.C
    echo "#include \"TChain.h\""                                                      >> doAll.C
    echo ""                                                                           >> doAll.C
    echo "void doAll(TString filepath=\"\", TString outputname=\"\", int nEvents=-1)" >> doAll.C
    echo "{"                                                                          >> doAll.C
    echo "    if (filepath.IsNull()) return;"                                         >> doAll.C
    echo "    TChain* chain = new TChain(\"${TTREENAME}\");"                          >> doAll.C
    echo "    chain->Add(filepath);"                                                  >> doAll.C
    echo "    ScanChain(chain, outputname, nEvents);"                                 >> doAll.C
    echo "}"                                                                          >> doAll.C

    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: Contact Philip Chang <philip@physics.ucsd.edu> for any questions."
    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: Happy Coding!"

else

    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: To use these classes, one includes the source files to your code and get a TTree object from one of the root file and do the following:"
    e_arrow "TasUtil:: "
    e_arrow "TasUtil::  ... "
    e_arrow "TasUtil::  ... "
    e_arrow "TasUtil::  TFile* file = new TFile(\"/path/to/my/rootfile.root\");"
    e_arrow "TasUtil::  TTree* tree = (TTree*) file->Get(\"${TREENAME}\");"
    e_arrow "TasUtil::  ${TREEINSTANCENAME}.Init(tree);"
    e_arrow "TasUtil::  ... "
    e_arrow "TasUtil::  ... "
    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: Contact Philip Chang <philip@physics.ucsd.edu> for any questions."
    e_arrow "TasUtil:: "
    e_arrow "TasUtil:: Happy Coding!"

fi
#eof
#!/bin/bash
