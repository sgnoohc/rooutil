//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef duplicate_h
#define duplicate_h

#include "dorky.h"

namespace RooUtil
{
    void remove_duplicate(TChain* chain, TString output, const char* run_bname, const char* lumi_bname, const char* evt_bname, int size=0);
}
