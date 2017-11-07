//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef fileutil_h
#define fileutil_h

#include "TChain.h"
#include "stringutil.h"

namespace RooUtil
{
    namespace FileUtil
    {
        TChain* createTChain(TString, TString);
    }
}

#endif
