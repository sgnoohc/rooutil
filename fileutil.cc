//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "fileutil.h"

TChain* RooUtil::FileUtil::createTChain(TString name, TString inputs)
{
    TChain* chain = new TChain(name);
    for (auto& ff : RooUtil::StringUtil::split(inputs, ",")) chain->Add(ff);
    return chain;
}
