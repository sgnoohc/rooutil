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
    for (auto& pair : mapIsBranchSet) pair.second = false;
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

//_________________________________________________________________________________________________
void TTreeX::createFlatBranch(std::vector<TString> p4_bns, std::vector<TString> float_bns, std::vector<TString> int_bns, std::vector<TString> bool_bns, int multiplicity)
{
    for ( auto& p4_bn : p4_bns )
    {
        for (int i = 0; i < multiplicity; ++i)
            createBranch<LV>(TString::Format("%s_%d", p4_bn.Data(), i));
    }
    for ( auto& float_bn : float_bns )
    {
        for (int i = 0; i < multiplicity; ++i)
            createBranch<float>(TString::Format("%s_%d", float_bn.Data(), i));
    }
    for ( auto& int_bn : int_bns )
    {
        for (int i = 0; i < multiplicity; ++i)
            createBranch<int>(TString::Format("%s_%d", int_bn.Data(), i));
    }
    for ( auto& bool_bn : bool_bns )
    {
        for (bool i = 0; i < multiplicity; ++i)
            createBranch<bool>(TString::Format("%s_%d", bool_bn.Data(), i));
    }
}

//_________________________________________________________________________________________________
void TTreeX::setFlatBranch(std::vector<TString> p4_bns, std::vector<TString> float_bns, std::vector<TString> int_bns, std::vector<TString> bool_bns, int multiplicity)
{
    for ( auto& p4_bn : p4_bns )
    {
        const std::vector<LV>& vec = getBranch<std::vector<LV>>(p4_bn, false);
        for (int i = 0; i < multiplicity && i < (int) vec.size(); ++i)
            setBranch<LV>(TString::Format("%s_%d", p4_bn.Data(), i), vec[i]);
    }
    for ( auto& float_bn : float_bns )
    {
        const std::vector<float>& vec = getBranch<std::vector<float>>(float_bn, false);
        for (int i = 0; i < multiplicity && i < (int) vec.size(); ++i)
            setBranch<float>(TString::Format("%s_%d", float_bn.Data(), i), vec[i]);
    }
    for ( auto& int_bn : int_bns )
    {
        const std::vector<int>& vec = getBranch<std::vector<int>>(int_bn, false);
        for (int i = 0; i < multiplicity && i < (int) vec.size(); ++i)
            setBranch<int>(TString::Format("%s_%d", int_bn.Data(), i), vec[i]);
    }
    for ( auto& bool_bn : bool_bns )
    {
        const std::vector<bool>& vec = getBranch<std::vector<bool>>(bool_bn, false);
        for (bool i = 0; i < multiplicity && i < (int) vec.size(); ++i)
            setBranch<bool>(TString::Format("%s_%d", bool_bn.Data(), i), vec[i]);
    }
}

//_________________________________________________________________________________________________
template <> void TTreeX::setBranch<Int_t               >(TString bn, Int_t                val) { if (mapInt_t     .find(bn) != mapInt_t     .end()) {mapInt_t     [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<Bool_t              >(TString bn, Bool_t               val) { if (mapBool_t    .find(bn) != mapBool_t    .end()) {mapBool_t    [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<Float_t             >(TString bn, Float_t              val) { if (mapFloat_t   .find(bn) != mapFloat_t   .end()) {mapFloat_t   [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<TString             >(TString bn, TString              val) { if (mapTString   .find(bn) != mapTString   .end()) {mapTString   [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<LV                  >(TString bn, LV                   val) { if (mapLV        .find(bn) != mapLV        .end()) {mapLV        [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<TBits               >(TString bn, TBits                val) { if (mapTBits     .find(bn) != mapTBits     .end()) {mapTBits     [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<unsigned long long  >(TString bn, unsigned long long   val) { if (mapULL       .find(bn) != mapULL       .end()) {mapULL       [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<std::vector<Int_t  >>(TString bn, std::vector<Int_t  > val) { if (mapVecInt_t  .find(bn) != mapVecInt_t  .end()) {mapVecInt_t  [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<std::vector<Bool_t >>(TString bn, std::vector<Bool_t > val) { if (mapVecBool_t .find(bn) != mapVecBool_t .end()) {mapVecBool_t [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<std::vector<Float_t>>(TString bn, std::vector<Float_t> val) { if (mapVecFloat_t.find(bn) != mapVecFloat_t.end()) {mapVecFloat_t[bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<std::vector<TString>>(TString bn, std::vector<TString> val) { if (mapVecTString.find(bn) != mapVecTString.end()) {mapVecTString[bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::setBranch<std::vector<LV     >>(TString bn, std::vector<LV     > val) { if (mapVecLV     .find(bn) != mapVecLV     .end()) {mapVecLV     [bn] = val; mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::pushbackToBranch<Int_t        >(TString bn, Int_t       val) { if (mapVecInt_t  .find(bn) != mapVecInt_t  .end()) {mapVecInt_t  [bn].push_back(val); mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::pushbackToBranch<Bool_t       >(TString bn, Bool_t      val) { if (mapVecBool_t .find(bn) != mapVecBool_t .end()) {mapVecBool_t [bn].push_back(val); mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::pushbackToBranch<Float_t      >(TString bn, Float_t     val) { if (mapVecFloat_t.find(bn) != mapVecFloat_t.end()) {mapVecFloat_t[bn].push_back(val); mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::pushbackToBranch<TString      >(TString bn, TString     val) { if (mapVecTString.find(bn) != mapVecTString.end()) {mapVecTString[bn].push_back(val); mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }
template <> void TTreeX::pushbackToBranch<LV           >(TString bn, LV          val) { if (mapVecLV     .find(bn) != mapVecLV     .end()) {mapVecLV     [bn].push_back(val); mapIsBranchSet[bn] = true;} else {error(TString::Format("branch doesn't exist bn = %s", bn.Data()));} }

//_________________________________________________________________________________________________
template <> const Int_t               & TTreeX::getBranch<Int_t               >(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapInt_t     [bn]; }
template <> const Bool_t              & TTreeX::getBranch<Bool_t              >(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapBool_t    [bn]; }
template <> const Float_t             & TTreeX::getBranch<Float_t             >(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapFloat_t   [bn]; }
template <> const TString             & TTreeX::getBranch<TString             >(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapTString   [bn]; }
template <> const LV                  & TTreeX::getBranch<LV                  >(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapLV        [bn]; }
template <> const TBits               & TTreeX::getBranch<TBits               >(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapTBits     [bn]; }
template <> const unsigned long long  & TTreeX::getBranch<unsigned long long  >(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapULL       [bn]; }
template <> const std::vector<Int_t  >& TTreeX::getBranch<std::vector<Int_t  >>(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapVecInt_t  [bn]; }
template <> const std::vector<Bool_t >& TTreeX::getBranch<std::vector<Bool_t >>(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapVecBool_t [bn]; }
template <> const std::vector<Float_t>& TTreeX::getBranch<std::vector<Float_t>>(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapVecFloat_t[bn]; }
template <> const std::vector<TString>& TTreeX::getBranch<std::vector<TString>>(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapVecTString[bn]; }
template <> const std::vector<LV     >& TTreeX::getBranch<std::vector<LV     >>(TString bn, bool check) { if (check) if (!mapIsBranchSet[bn]) error(TString::Format("branch hasn't been set yet bn = %s", bn.Data())); return mapVecLV     [bn]; }

//_________________________________________________________________________________________________
template <> void TTreeX::createBranch<Int_t               >(TString bn) { if (mapInt_t     .find(bn) == mapInt_t     .end()) ttree->Branch(bn, &(mapInt_t      [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<Bool_t              >(TString bn) { if (mapBool_t    .find(bn) == mapBool_t    .end()) ttree->Branch(bn, &(mapBool_t     [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<Float_t             >(TString bn) { if (mapFloat_t   .find(bn) == mapFloat_t   .end()) ttree->Branch(bn, &(mapFloat_t    [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<TString             >(TString bn) { if (mapTString   .find(bn) == mapTString   .end()) ttree->Branch(bn, &(mapTString    [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<LV                  >(TString bn) { if (mapLV        .find(bn) == mapLV        .end()) ttree->Branch(bn, &(mapLV         [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<TBits               >(TString bn) { if (mapTBits     .find(bn) == mapTBits     .end()) ttree->Branch(bn, &(mapTBits      [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<unsigned long long  >(TString bn) { if (mapULL       .find(bn) == mapULL       .end()) ttree->Branch(bn, &(mapULL        [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<std::vector<Int_t  >>(TString bn) { if (mapVecInt_t  .find(bn) == mapVecInt_t  .end()) ttree->Branch(bn, &(mapVecInt_t   [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<std::vector<Bool_t >>(TString bn) { if (mapVecBool_t .find(bn) == mapVecBool_t .end()) ttree->Branch(bn, &(mapVecBool_t  [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<std::vector<Float_t>>(TString bn) { if (mapVecFloat_t.find(bn) == mapVecFloat_t.end()) ttree->Branch(bn, &(mapVecFloat_t [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<std::vector<TString>>(TString bn) { if (mapVecTString.find(bn) == mapVecTString.end()) ttree->Branch(bn, &(mapVecTString [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}
template <> void TTreeX::createBranch<std::vector<LV     >>(TString bn) { if (mapVecLV     .find(bn) == mapVecLV     .end()) ttree->Branch(bn, &(mapVecLV      [bn])); else error(TString::Format("branch already exists bn = %s", bn.Data()));}

//_________________________________________________________________________________________________
template <> void TTreeX::setBranch<std::map<TString, std::vector<Int_t>>>(std::map<TString, std::vector<Int_t>>& objidx)
{
    for (auto& pair : objidx)
    {
        setBranch<Int_t>("n" + pair.first, pair.second.size());
        setBranch<std::vector<Int_t>>(pair.first, pair.second);
    }
}
template <> void TTreeX::createBranch<std::map<TString, std::vector<Int_t>>>(std::map<TString, std::vector<Int_t>>& objidx)
{
    for (auto& pair : objidx)
    {
        createBranch<Int_t>("n" + pair.first);
        createBranch<std::vector<Int_t>>(pair.first);
    }
}

//eof
