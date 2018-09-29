#ifndef cutflowutil_h
#define cutflowutil_h

#include "ttreex.h"

namespace RooUtil
{
    namespace CutflowUtil
    {
        std::vector<float> getCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx);
        void fillCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
        void fillRawCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
    }
}

#endif
