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
    echo "The generated source C++ codes can be used to read a ROOT's TTree with rest of the RooUtil tool at ease."
    echo "The generation of the code relies on:"
    echo "  > https://github.com/cmstas/Software/blob/master/makeCMS3ClassFiles/makeCMS3ClassFiles.C"
    echo ""
    echo "Usage:"
    echo ""
    echo ${green}"  > sh $(basename $0) [-f] [-h] [-x] ROOTFILE TTREENAME CLASSNAME [NAMESPACENAME=tas] [CLASSINSTANCENAME=cms3] "${reset}
    echo ""
    echo ""
    echo ${green}" -h ${reset}: print this message"
    echo ${green}" -f ${reset}: force run this script"
    echo ${green}" -x ${reset}: create additional looper template files (i.e. process.cc, Makefile)"
    echo ""
    echo ${green}" ROOTFILE          ${reset}= Path to the root file that holds an example TTree that you wish to study."
    echo ${green}" TREENAME          ${reset}= The TTree object TKey name in the ROOTFILE"
    echo ${green}" CLASSNAME         ${reset}= The name you want to give to the class you are creating"
    echo ${green}" NAMESPACENAME     ${reset}= The name you want to give to the namespace for accessing the ttree"
    echo ${green}" CLASSINSTANCENAME ${reset}= The name of the global instance of the class that you are trying to create"
    echo "                     (defaults to 'cms3')"
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
            echo "RooUtil:: No argument is given"
            echo "RooUtil:: The script will assume to create a CMS3 looper"
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
if [ -z ${TREEINSTANCENAME} ]; then TREEINSTANCENAME=cms3; fi

echo ""
e_arrow "RooUtil:: The user has provided following options"
e_arrow "RooUtil:: $(date)"
e_arrow "RooUtil:: =========================================="
e_arrow "RooUtil::  ROOTFILE=$ROOTFILE"
e_arrow "RooUtil::  TREENAME=$TTREENAME"
e_arrow "RooUtil::  MAKECLASSNAME=$MAKECLASSNAME"
e_arrow "RooUtil::  TREEINSTANCENAME=$TREEINSTANCENAME"
e_arrow "RooUtil:: =========================================="
e_arrow "RooUtil:: "

# Print
e_arrow "RooUtil:: Generating ${MAKECLASSNAME}.cc/h file which can load the TTree content from ${ROOTFILE}:${TREENAME} ..."

ROOTFILE=$('cd' $(dirname ${ROOTFILE}); pwd)/$(basename $1)

# Check whether the file already exists
if [ -e ${MAKECLASSNAME}.cc ]; then
    if [ "${FORCE}" == true ]; then
        :
    else
        e_error "RooUtil:: ${MAKECLASSNAME}.cc already exists!"
        e_error "RooUtil:: Do you want to override? If so, provide option -f. For more info use option -h"
        exit
    fi
fi

# Check whether the file already exists
if [ -e ${MAKECLASSNAME}.h ]; then
    if [ "${FORCE}" == true ]; then
        :
    else
        e_error "RooUtil:: ${MAKECLASSNAME}.h already exists!"
        e_error "RooUtil:: Do you want to override? If so, provide option -f. For more info use option -h"
        exit
    fi
fi

source $DIR/root.sh ""

if [ -e $DIR/makeCMS3ClassFiles.C ]; then
  echo "running makeCMS3ClassFiles.C"
  root -l -b -q $DIR/makeCMS3ClassFiles.C\(\"${ROOTFILE}\",\"${TTREENAME}\",\"${MAKECLASSNAME}\",\"${NAMESPACENAME}\",\"${TREEINSTANCENAME}\"\)  &> /dev/null
fi

if [ $? -eq 0 ]; then
    e_arrow "RooUtil:: Generated ${MAKECLASSNAME}.cc/h successfully!"
else
    e_error "RooUtil:: Failed to generate ${MAKECLASSNAME}.cc/h!"
    exit
fi

if [ "$GENERATEEXTRACODE" == true ]; then

#    #
#    # Add "rooutil" to the class
#    #
#    echo "#include \"rooutil/rooutil.cc\"" >> ${MAKECLASSNAME}.cc
#    echo "#include \"rooutil/rooutil.h\"" >> ${MAKECLASSNAME}.h


    if [ -e process.cc ]; then
        e_error "RooUtil:: process.cc already exists. We will leave it alone. Erase it if you want to override"
    else

        #
        # Create process.cc
        #
        echo "#include \"${MAKECLASSNAME}.h\""                                                                                                                 >  process.cc
        echo "#include \"rooutil.h\""                                                                                                                          >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "// ./process INPUTFILEPATH OUTPUTFILE [NEVENTS]"                                                                                                 >> process.cc
        echo "int main(int argc, char** argv)"                                                                                                                 >> process.cc
        echo "{"                                                                                                                                               >> process.cc
        echo "    // Argument checking"                                                                                                                        >> process.cc
        echo "    if (argc < 3)"                                                                                                                               >> process.cc
        echo "    {"                                                                                                                                           >> process.cc
        echo "        std::cout << \"Usage:\" << std::endl;"                                                                                                   >> process.cc
        echo "        std::cout << \"  $ ./process INPUTFILES OUTPUTFILE [NEVENTS]\" << std::endl;"                                                            >> process.cc
        echo "        std::cout << std::endl;"                                                                                                                 >> process.cc
        echo "        std::cout << \"  INPUTFILES      comma separated file list\" << std::endl;"                                                              >> process.cc
        echo "        std::cout << \"  OUTPUTFILE      output file\" << std::endl;"                                                                            >> process.cc
        echo "        std::cout << \"  [NEVENTS=-1]    # of events to run over\" << std::endl;"                                                                >> process.cc
        echo "        std::cout << std::endl;"                                                                                                                 >> process.cc
        echo "        return 1;"                                                                                                                               >> process.cc
        echo "    }"                                                                                                                                           >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Creating output file where we will put the outputs of the processing"                                                                     >> process.cc
        echo "    TFile* ofile = new TFile(argv[2], \"create\");"                                                                                              >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Create a TChain of the input files"                                                                                                       >> process.cc
        echo "    // The input files can be comma separated (e.g. \"file1.root,file2.root\") or with wildcard (n.b. be sure to escape)"                        >> process.cc
        echo "    TChain* ch = RooUtil::FileUtil::createTChain(\"${TTREENAME}\", argv[1]);"                                                                    >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Number of events to loop over"                                                                                                            >> process.cc
        echo "    int nEvents = argc > 3 ? atoi(argv[3]) : -1;"                                                                                                >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Create a Looper object to loop over input files"                                                                                          >> process.cc
        echo "    RooUtil::Looper<${MAKECLASSNAME}> looper(ch, &${TREEINSTANCENAME}, nEvents);"                                                                >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Cutflow utility object that creates a tree structure of cuts"                                                                             >> process.cc
        echo "    RooUtil::Cutflow cutflow(ofile);"                                                                                                            >> process.cc
        echo "    cutflow.addCut(\"DiElChannel\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );"                    >> process.cc
        echo "    cutflow.addCut(\"DiMuChannel\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );"                    >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    cutflow.getCut(\"DiElChannel\");"                                                                                                            >> process.cc
        echo "    cutflow.addCutToLastActiveCut(\"DiElChannelCutA\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );" >> process.cc
        echo "    cutflow.addCutToLastActiveCut(\"DiElChannelCutB\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );" >> process.cc
        echo "    cutflow.addCutToLastActiveCut(\"DiElChannelCutC\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );" >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    cutflow.getCut(\"DiMuChannel\");"                                                                                                            >> process.cc
        echo "    cutflow.addCutToLastActiveCut(\"DiMuChannelCutA\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );" >> process.cc
        echo "    cutflow.addCutToLastActiveCut(\"DiMuChannelCutB\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );" >> process.cc
        echo "    cutflow.addCutToLastActiveCut(\"DiMuChannelCutC\", [&]() { return 1/*set your cut here*/; }, [&]() { return 1/*set your weight here*/; } );" >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Print cut structure"                                                                                                                      >> process.cc
        echo "    cutflow.printCuts();"                                                                                                                        >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Histogram utility object that is used to define the histograms"                                                                           >> process.cc
        echo "    RooUtil::Histograms histograms;"                                                                                                             >> process.cc
        echo "    histograms.addHistogram(\"Mll\", 180, 0, 250, [&]() { return 1/* set your variable here*/; } );"                                             >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Book cutflows"                                                                                                                            >> process.cc
        echo "    cutflow.bookCutflows();"                                                                                                                     >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Book Histograms"                                                                                                                          >> process.cc
        echo "    cutflow.bookHistogramsForCutAndBelow(histograms, \"DiElChannel\");"                                                                          >> process.cc
        echo "    cutflow.bookHistogramsForCutAndBelow(histograms, \"DiMuChannel\");"                                                                          >> process.cc
        echo "    // cutflow.bookHistograms(histograms); // if just want to book everywhere"                                                                   >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Looping input file"                                                                                                                       >> process.cc
        echo "    while (looper.nextEvent())"                                                                                                                  >> process.cc
        echo "    {"                                                                                                                                           >> process.cc
        echo "        //Do what you need to do in for each event here"                                                                                         >> process.cc
        echo "        //To save use the following function"                                                                                                    >> process.cc
        echo "        cutflow.fill();"                                                                                                                         >> process.cc
        echo "    }"                                                                                                                                           >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // Writing output file"                                                                                                                      >> process.cc
        echo "    cutflow.saveOutput();"                                                                                                                       >> process.cc
        echo ""                                                                                                                                                >> process.cc
        echo "    // The below can be sometimes crucial"                                                                                                       >> process.cc
        echo "    delete ofile;"                                                                                                                               >> process.cc
        echo "}"                                                                                                                                               >> process.cc

    fi

    #
    # Create Makefile
    #
    if [ -e Makefile ]; then
        e_error "RooUtil:: Makefile already exists. We will leave it alone. Erase it if you want to override"
    else
        echo '# Simple makefile'                                                                                                                            >  Makefile
        echo ''                                                                                                                                             >> Makefile
        echo 'EXE=doAnalysis'                                                                                                                               >> Makefile
        echo ''                                                                                                                                             >> Makefile
        echo 'SOURCES=$(wildcard *.cc)'                                                                                                                     >> Makefile
        echo 'OBJECTS=$(SOURCES:.cc=.o)'                                                                                                                    >> Makefile
        echo 'HEADERS=$(SOURCES:.cc=.h)'                                                                                                                    >> Makefile
        echo ''                                                                                                                                             >> Makefile
        echo 'CC          = g++'                                                                                                                            >> Makefile
        echo 'CXX         = g++'                                                                                                                            >> Makefile
        echo 'CXXFLAGS    = -g -O2 -Wall -fPIC -Wshadow -Woverloaded-virtual'                                                                               >> Makefile
        echo 'LD          = g++'                                                                                                                            >> Makefile
        echo 'LDFLAGS     = -g -O2'                                                                                                                         >> Makefile
        echo 'SOFLAGS     = -g -shared'                                                                                                                     >> Makefile
        echo 'CXXFLAGS    = -g -O2 -Wall -fPIC -Wshadow -Woverloaded-virtual'                                                                               >> Makefile
        echo 'LDFLAGS     = -g -O2'                                                                                                                         >> Makefile
        echo 'ROOTLIBS    = $(shell root-config --libs)'                                                                                                    >> Makefile
        echo 'ROOTCFLAGS  = $(shell root-config --cflags)'                                                                                                  >> Makefile
        echo 'CXXFLAGS   += $(ROOTCFLAGS)'                                                                                                                  >> Makefile
        echo 'CFLAGS      = $(ROOTCFLAGS) -Wall -Wno-unused-function -g -O2 -fPIC -fno-var-tracking'                                                        >> Makefile
        echo 'EXTRACFLAGS = $(shell rooutil-config)'                                                                                                        >> Makefile
        echo 'EXTRAFLAGS  = -fPIC -ITMultiDrawTreePlayer -Wunused-variable -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer'                              >> Makefile
        echo ''                                                                                                                                             >> Makefile
        echo '$(EXE): $(OBJECTS) '${MAKECLASSNAME}'.o'                                                                                                      >> Makefile
        echo '	$(LD) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) $(ROOTLIBS) $(EXTRAFLAGS) -o $@'                                                                    >> Makefile
        echo ''                                                                                                                                             >> Makefile
        echo '%.o: %.cc'                                                                                                                                    >> Makefile
        echo '	$(CC) $(CFLAGS) $(EXTRACFLAGS) $< -c'                                                                                                       >> Makefile
        echo ''                                                                                                                                             >> Makefile
        echo 'clean:'                                                                                                                                       >> Makefile
        echo '	rm -f *.o $(EXE)'                                                                                                                           >> Makefile
    fi

    #echo "	sh rooutil/makeclass.sh -f -x TEMPLATE_TREE_PATH ${TTREENAME} ${MAKECLASSNAME} ${NAMESPACENAME} ${TREEINSTANCENAME}  > /dev/null 2>&1"  >> Makefile

    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: Contact Philip Chang <philip@ucsd.edu> for any questions."
    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: Happy Coding!"
    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: Compile via 'make'"

else

    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: To use these classes, one includes the source files to your code and get a TTree object from one of the root file and do the following:"
    e_arrow "RooUtil:: "
    e_arrow "RooUtil::  ... "
    e_arrow "RooUtil::  ... "
    e_arrow "RooUtil::  TFile* file = new TFile(\"/path/to/my/rootfile.root\");"
    e_arrow "RooUtil::  TTree* tree = (TTree*) file->Get(\"${TREENAME}\");"
    e_arrow "RooUtil::  ${TREEINSTANCENAME}.Init(tree);"
    e_arrow "RooUtil::  ... "
    e_arrow "RooUtil::  ... "
    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: Contact Philip Chang <philip@ucsd.edu> for any questions."
    e_arrow "RooUtil:: "
    e_arrow "RooUtil:: Happy Coding!"

fi
#eof

