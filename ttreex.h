//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef ttreex_h
#define ttreex_h

// C/C++
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdarg.h>
#include <functional>
#include <cmath>
#include <utility>

// ROOT
#include "TBenchmark.h"
#include "TBits.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TChainElement.h"
#include "TTreeCache.h"
#include "TTreePerfStats.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "Math/LorentzVector.h"

#include "printutil.h"

//#define MAP std::unordered_map
//#define STRING std::string
#define MAP std::map
#define STRING TString

///////////////////////////////////////////////////////////////////////////////////////////////
// LorentzVector typedef that we use very often
///////////////////////////////////////////////////////////////////////////////////////////////
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LV;

namespace RooUtil
{

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // TTreeX class
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // NOTE: This class assumes accessing TTree in the SNT style which uses the following,
    // https://github.com/cmstas/Software/blob/master/makeCMS3ClassFiles/makeCMS3ClassFiles.C
    // It is assumed that the "template" class passed to this class will have
    // 1. "Init(TTree*)"
    // 2. "GetEntry(uint)"
    // 3. "progress(nevtProc'ed, total)"
    class TTreeX
    {

        public:
        enum kType
        {
            kInt_t      =  1,
            kBool_t     =  2,
            kFloat_t    =  3,
            kTString    =  4,
            kLV         =  5,
            kVecInt_t   = 11,
            kVecBool_t  = 12,
            kVecFloat_t = 13,
            kVecTString = 14,
            kVecLV      = 15
        };
        typedef std::vector<LV>::const_iterator lviter;

        private:
        TTree* ttree;
        std::map<TString, Int_t  > mapInt_t;
        std::map<TString, Bool_t > mapBool_t;
        std::map<TString, Float_t> mapFloat_t;
        std::map<TString, TString> mapTString;
        std::map<TString, LV     > mapLV;
        std::map<TString, TBits  > mapTBits;
        std::map<TString, std::vector<Int_t  > > mapVecInt_t;
        std::map<TString, std::vector<Bool_t > > mapVecBool_t;
        std::map<TString, std::vector<Float_t> > mapVecFloat_t;
        std::map<TString, std::vector<TString> > mapVecTString;
        std::map<TString, std::vector<LV     > > mapVecLV;

        public:
        TTreeX();
        TTreeX(TString treename, TString title);
        TTreeX(TTree* tree);
        ~TTreeX();
        TTree* getTree() { return ttree; }
        void setTree(TTree* tree) { ttree = tree; }
        void* getValPtr(TString brname);
        template <class T>
        T* get(TString brname, int entry=-1);

        template <class T>
        void createBranch(TString);
        template <class T>
        void setBranch(TString, T);
        template <class T>
        void createBranch(T&);
        template <class T>
        void setBranch(T&);
        template <class T>
        void pushbackToBranch(TString, T);

        void sortVecBranchesByPt(TString, std::vector<TString>, std::vector<TString>, std::vector<TString>);
        template <class T>
        std::vector<T> sortFromRef( std::vector<T> const& in, std::vector<std::pair<size_t, lviter> > const& reference);
        struct ordering
        {
            bool operator ()(std::pair<size_t, lviter> const& a, std::pair<size_t, lviter> const& b) 
            {
                return (*(a.second)).pt() > (*(b.second)).pt();
            }
        };

        void clear();
        void save(TFile*);
    };

    //_________________________________________________________________________________________________
    template <> void TTreeX::setBranch<Int_t               >(TString bn, Int_t                val) { mapInt_t     [bn] = val; }
    template <> void TTreeX::setBranch<Bool_t              >(TString bn, Bool_t               val) { mapBool_t    [bn] = val; }
    template <> void TTreeX::setBranch<Float_t             >(TString bn, Float_t              val) { mapFloat_t   [bn] = val; }
    template <> void TTreeX::setBranch<TString             >(TString bn, TString              val) { mapTString   [bn] = val; }
    template <> void TTreeX::setBranch<LV                  >(TString bn, LV                   val) { mapLV        [bn] = val; }
    template <> void TTreeX::setBranch<TBits               >(TString bn, TBits                val) { mapTBits     [bn] = val; }
    template <> void TTreeX::setBranch<std::vector<Int_t  >>(TString bn, std::vector<Int_t  > val) { mapVecInt_t  [bn] = val; }
    template <> void TTreeX::setBranch<std::vector<Bool_t >>(TString bn, std::vector<Bool_t > val) { mapVecBool_t [bn] = val; }
    template <> void TTreeX::setBranch<std::vector<Float_t>>(TString bn, std::vector<Float_t> val) { mapVecFloat_t[bn] = val; }
    template <> void TTreeX::setBranch<std::vector<TString>>(TString bn, std::vector<TString> val) { mapVecTString[bn] = val; }
    template <> void TTreeX::setBranch<std::vector<LV     >>(TString bn, std::vector<LV     > val) { mapVecLV     [bn] = val; }
    template <> void TTreeX::pushbackToBranch<Int_t        >(TString bn, Int_t       val) { mapVecInt_t  [bn].push_back(val); }
    template <> void TTreeX::pushbackToBranch<Bool_t       >(TString bn, Bool_t      val) { mapVecBool_t [bn].push_back(val); }
    template <> void TTreeX::pushbackToBranch<Float_t      >(TString bn, Float_t     val) { mapVecFloat_t[bn].push_back(val); }
    template <> void TTreeX::pushbackToBranch<TString      >(TString bn, TString     val) { mapVecTString[bn].push_back(val); }
    template <> void TTreeX::pushbackToBranch<LV           >(TString bn, LV          val) { mapVecLV     [bn].push_back(val); }

    //_________________________________________________________________________________________________
    template <> void TTreeX::createBranch<Int_t               >(TString bn) { ttree->Branch(bn, &(mapInt_t      [bn])); }
    template <> void TTreeX::createBranch<Bool_t              >(TString bn) { ttree->Branch(bn, &(mapBool_t     [bn])); }
    template <> void TTreeX::createBranch<Float_t             >(TString bn) { ttree->Branch(bn, &(mapFloat_t    [bn])); }
    template <> void TTreeX::createBranch<TString             >(TString bn) { ttree->Branch(bn, &(mapTString    [bn])); }
    template <> void TTreeX::createBranch<LV                  >(TString bn) { ttree->Branch(bn, &(mapLV         [bn])); }
    template <> void TTreeX::createBranch<TBits               >(TString bn) { ttree->Branch(bn, &(mapTBits      [bn])); }
    template <> void TTreeX::createBranch<std::vector<Int_t  >>(TString bn) { ttree->Branch(bn, &(mapVecInt_t   [bn])); }
    template <> void TTreeX::createBranch<std::vector<Bool_t >>(TString bn) { ttree->Branch(bn, &(mapVecBool_t  [bn])); }
    template <> void TTreeX::createBranch<std::vector<Float_t>>(TString bn) { ttree->Branch(bn, &(mapVecFloat_t [bn])); }
    template <> void TTreeX::createBranch<std::vector<TString>>(TString bn) { ttree->Branch(bn, &(mapVecTString [bn])); }
    template <> void TTreeX::createBranch<std::vector<LV     >>(TString bn) { ttree->Branch(bn, &(mapVecLV      [bn])); }

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

    template <class T>
    std::vector<T> TTreeX::sortFromRef( std::vector<T> const& in, std::vector<std::pair<size_t, TTreeX::lviter> > const& reference)
    {
        std::vector<T> ret(in.size());

        size_t const size = in.size();
        for (size_t i = 0; i < size; ++i)
            ret[i] = in[reference[i].first];

        return ret;
    }

}

//_________________________________________________________________________________________________
template <class T>
T* RooUtil::TTreeX::get(TString brname, int entry)
{
    if (entry >= 0)
        ttree->GetEntry(entry);
    return (T*) getValPtr(brname);
}

#endif
