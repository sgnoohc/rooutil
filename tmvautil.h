#ifndef tmvautil_h
#define tmvautil_h

#include <vector>

#include "TMVA/Reader.h"
#include "ttreex.h"

namespace RooUtil
{

    namespace TMVAUtil
    {
        TMVA::Reader* createReader(TString methodType, TString xmlpath, RooUtil::TTreeX& tx);
        std::vector<float> getInputValues(TMVA::Reader* reader, RooUtil::TTreeX& tx);
    }

}

#endif
