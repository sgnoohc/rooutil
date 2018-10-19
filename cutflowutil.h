#ifndef cutflowutil_h
#define cutflowutil_h

#include "ttreex.h"
#include <tuple>
#include <vector>
#include <map>
#include "TH1.h"
#include <iostream>

namespace RooUtil
{
    namespace CutflowUtil
    {

        class CutNameList
        {
            public:
            std::vector<TString> cutlist;
            CutNameList() {}
            CutNameList(const CutNameList& cutnamelist) { cutlist = cutnamelist.cutlist; }
            void clear() { cutlist.clear(); }
            void addCutName(TString cutname) { cutlist.push_back(cutname); }
            void print() { for (auto& str : cutlist) std::cout << str << std::endl; }
        };

        class CutNameListMap
        {
            public:
            std::map<TString, CutNameList> cutlists;
            std::vector<TString> cutlist;
            CutNameList& operator[] (TString name) { return cutlists[name]; }
            void clear() { cutlists.clear(); }
            void print() { for (auto& cutlist : cutlists) { std::cout << "CutNameList - " << cutlist.first << std::endl; cutlist.second.print(); } }
            std::map<TString, std::vector<TString>> getStdVersion()
            {
                std::map<TString, std::vector<TString>> obj_cutlists;
                for (auto& cutlist : cutlists)
                    obj_cutlists[cutlist.first] = cutlist.second.cutlist;
                return obj_cutlists;
            }
        };

        void createCutflowBranches(CutNameListMap& cutlists, RooUtil::TTreeX& tx);

        std::vector<float> getCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx);
        bool passCuts(std::vector<TString> cutlist, RooUtil::TTreeX& tx);
        void fillCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
        void fillRawCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
        std::tuple<std::map<TString, TH1F*>, std::map<TString, TH1F*>> createCutflowHistograms(CutNameListMap& cutlists);
        std::tuple<std::map<TString, TH1F*>, std::map<TString, TH1F*>> createCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists);
        void fillCutflowHistograms(CutNameListMap& cutlists, RooUtil::TTreeX& tx, std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);
        void fillCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists, RooUtil::TTreeX& tx, std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);
        void saveCutflowHistograms(std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);
    }
}

#endif
