//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "fileutil.h"

TChain* RooUtil::FileUtil::createTChain(TString name, TString inputs)
{
    TChain* chain = new TChain(name);
    for (auto& ff : RooUtil::StringUtil::split(inputs, ",")) chain->Add(ff);
    return chain;
}

TMultiDrawTreePlayer* RooUtil::FileUtil::createTMulti(TString name, TString inputs)
{
    TVirtualTreePlayer::SetPlayer("TMultiDrawTreePlayer");
    TChain* t = RooUtil::FileUtil::createTChain(name, inputs);
    TMultiDrawTreePlayer* p = dynamic_cast<TMultiDrawTreePlayer*>(t->GetPlayer());
    return p;
}

TH1* RooUtil::FileUtil::get(TString name)
{
    return (TH1*) gDirectory->Get(name);
}
