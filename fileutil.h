//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef fileutil_h
#define fileutil_h

#include "TChain.h"
#include "TDirectory.h"
#include "stringutil.h"
#include "multidraw.h"

namespace RooUtil
{
    namespace FileUtil
    {
        TChain* createTChain(TString, TString);
        TMultiDrawTreePlayer* createTMulti(TString, TString);
        TH1* get(TString);

    }
}

#endif
