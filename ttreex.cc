//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "ttreex.h"

using namespace RooUtil;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// TTree++ (TTreeX) class
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//_________________________________________________________________________________________________
RooUtil::TTreeX::TTreeX()
{
    ttree = 0;
}

//_________________________________________________________________________________________________
RooUtil::TTreeX::TTreeX(TString treename, TString title)
{
    ttree = new TTree(treename.Data(), title.Data());
}

//_________________________________________________________________________________________________
RooUtil::TTreeX::TTreeX(TTree* tree)
{
    ttree = tree;
}

//_________________________________________________________________________________________________
RooUtil::TTreeX::~TTreeX()
{
}

//_________________________________________________________________________________________________
void* RooUtil::TTreeX::getValPtr(TString brname)
{
    TBranch* br = ttree->GetBranch(brname);
    unsigned int nleaves = br->GetListOfLeaves()->GetEntries();
    if (nleaves != 1)
        RooUtil::error("# of leaf for this branch="
                + brname + " is not equals to 1!", __FUNCTION__);
    if (!(((TLeaf*) br->GetListOfLeaves()->At(0))->GetValuePointer()))
        ttree->GetEntry(0);
    return ((TLeaf*) br->GetListOfLeaves()->At(0))->GetValuePointer();
}

//__________________________________________________________________________________________________
void RooUtil::TTreeX::clear()
{
    for (auto& pair : mapInt_t  ) pair.second = -999;
    for (auto& pair : mapBool_t ) pair.second = 0;
    for (auto& pair : mapFloat_t) pair.second = -999;
    for (auto& pair : mapTString) pair.second = "";
    for (auto& pair : mapLV     ) pair.second.SetXYZT(0, 0, 0, 0 ) ;
    for (auto& pair : mapVecInt_t  ) pair.second.clear();
    for (auto& pair : mapVecBool_t ) pair.second.clear();
    for (auto& pair : mapVecFloat_t) pair.second.clear();
    for (auto& pair : mapVecTString) pair.second.clear();
    for (auto& pair : mapVecLV     ) pair.second.clear();
}

//__________________________________________________________________________________________________
void RooUtil::TTreeX::save(TFile* ofile)
{
    RooUtil::print(Form("TTreeX::save() saving tree to %s", ofile->GetName()));
    ofile->cd();
    this->ttree->Write();
}

//__________________________________________________________________________________________________
void TTreeX::sortVecBranchesByPt(TString p4_bn, std::vector<TString> aux_float_bns, std::vector<TString> aux_int_bns, std::vector<TString> aux_bool_bns)
{
    // https://stackoverflow.com/questions/236172/how-do-i-sort-a-stdvector-by-the-values-of-a-different-stdvector
    // The first argument is the p4 branches
    // The rest of the argument holds the list of auxilary branches that needs to be sorted together.

    // Creating a "ordered" index list
    std::vector<std::pair<size_t, lviter> > order(mapVecLV[p4_bn].size());

    size_t n = 0;
    for (lviter it = mapVecLV[p4_bn].begin(); it != mapVecLV[p4_bn].end(); ++it, ++n)
            order[n] = make_pair(n, it);

    sort(order.begin(), order.end(), ordering());

    // Sort!
    mapVecLV[p4_bn] = sortFromRef<LV>(mapVecLV[p4_bn], order);

    for ( auto& aux_float_bn : aux_float_bns )
        mapVecFloat_t[aux_float_bn] = sortFromRef<Float_t>(mapVecFloat_t[aux_float_bn], order);

    for ( auto& aux_int_bn : aux_int_bns )
        mapVecInt_t[aux_int_bn] = sortFromRef<Int_t>(mapVecInt_t[aux_int_bn], order);

    for ( auto& aux_bool_bn : aux_bool_bns )
        mapVecBool_t[aux_bool_bn] = sortFromRef<Bool_t>(mapVecBool_t[aux_bool_bn], order);

}

//eof
