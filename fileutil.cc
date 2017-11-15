//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "fileutil.h"

TChain* RooUtil::FileUtil::createTChain(TString name, TString inputs)
{
    TChain* chain = new TChain(name);
    for (auto& ff : RooUtil::StringUtil::split(inputs, ",")) chain->Add(ff);
    return chain;
}

TMultiDrawTreePlayer* RooUtil::FileUtil::createTMulti(TChain* t)
{
    TMultiDrawTreePlayer* p = new TMultiDrawTreePlayer();
    if (p) p->SetTree((TTree*) t);
    return p;
}

TMultiDrawTreePlayer* RooUtil::FileUtil::createTMulti(TString name, TString inputs)
{
    TChain* t = RooUtil::FileUtil::createTChain(name, inputs);
    return createTMulti(t);
}

TH1* RooUtil::FileUtil::get(TString name)
{
    return (TH1*) gDirectory->Get(name);
}
