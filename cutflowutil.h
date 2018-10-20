#ifndef cutflowutil_h
#define cutflowutil_h

#include "ttreex.h"
#include "printutil.h"
#include <tuple>
#include <vector>
#include <map>
#include "TH1.h"
#include "TString.h"
#include <iostream>
#include <algorithm>

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
        void createCutflowBranches(std::map<TString, std::vector<TString>>& cutlists, RooUtil::TTreeX& tx);
        std::tuple<std::vector<bool>, std::vector<float>> getCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx);
        bool passCuts(std::vector<TString> cutlist, RooUtil::TTreeX& tx);
        void fillCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
        void fillRawCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
        std::tuple<std::map<TString, TH1F*>, std::map<TString, TH1F*>> createCutflowHistograms(CutNameListMap& cutlists);
        std::tuple<std::map<TString, TH1F*>, std::map<TString, TH1F*>> createCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists);
        void fillCutflowHistograms(CutNameListMap& cutlists, RooUtil::TTreeX& tx, std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);
        void fillCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists, RooUtil::TTreeX& tx, std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);
        void saveCutflowHistograms(std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);

    }

    class CutTree
    {
        public:
            TString name; 
            CutTree* parent;
            std::vector<CutTree*> children;
            CutTree(TString n) : name(n), parent(0) {}
            ~CutTree()
            {
                for (auto& child : children)
                {
                    delete child;
                }
            }
            void printCuts(int indent=0)
            {
                TString msg = "";
                for (int i = 0; i < indent; ++i) msg += " ";
                msg += name;
                print(msg);
                for (auto& child : children)
                    (*child).printCuts(indent+1);
            }
            void addCut(TString n)
            {
                CutTree* obj = new CutTree(n);
                obj->parent = this;
                children.push_back(obj);
            }
            CutTree* getCutPointer(TString n)
            {
                // If the name match then return itself
                if (name.EqualTo(n))
                {
                    return this;
                }
                else
                {
                    // Otherwise, loop over the children an if a children has the correct one return the found CutTree
                    for (auto& child : children)
                    {
                        CutTree* c = child->getCutPointer(n);
                        if (c)
                            return c;
                    }
                    return 0;
                }
            }
            // Wrapper to return the object instead of pointer
            CutTree& getCut(TString n)
            {
                CutTree* c = getCutPointer(n);
                if (c)
                {
                    return *c;
                }
                else
                {
                    RooUtil::error(TString::Format("Asked for %s cut, but did not find the cut", n.Data()));
                    return *this;
                }
            }
            std::vector<TString> getCutList(TString n, std::vector<TString> cut_list=std::vector<TString>())
            {
                // Main idea: start from the end node provided by the first argument "n", and work your way up to the root node.
                //
                // The algorithm will first determine whether I am starting from a specific cut requested by the user or within in recursion.
                // If the cut_list.size() == 0, the function is called by the user (since no list is aggregated so far)
                // In that case, first find the pointer to the object we want and set it to "c"
                // If cut_list.size() is non-zero then take this as the cut that I am starting and I go up the chain to aggregate all the cuts prior to the requested cut
                CutTree* c = 0;
                if (cut_list.size() == 0)
                {
                    c = &getCut(n);
                    cut_list.push_back(c->name);
                }
                else
                {
                    c = this;
                    cut_list.push_back(n);
                }
                if (c->parent)
                {
                    return (c->parent)->getCutList((c->parent)->name, cut_list);
                }
                else
                {
                    std::reverse(cut_list.begin(), cut_list.end());
                    return cut_list;
                }
            }
            std::vector<TString> getEndCuts(std::vector<TString> endcuts=std::vector<TString>())
            {
                if (children.size() == 0)
                {
                    endcuts.push_back(name);
                    return endcuts;
                }
                for (auto& child : children)
                    endcuts = child->getEndCuts(endcuts);
                return endcuts;
            }
            std::vector<TString> getCutListBelow(TString n, std::vector<TString> cut_list=std::vector<TString>())
            {
                // Main idea: start from the node provided by the first argument "n", and work your way down to the ends.
                CutTree* c = 0;
                if (cut_list.size() == 0)
                {
                    c = &getCut(n);
                    cut_list.push_back(c->name);
                }
                else
                {
                    c = this;
                    cut_list.push_back(n);
                }
                if (children.size() > 0)
                {
                    for (auto& child : c->children)
                    {
                        cut_list = child->getCutListBelow(child->name, cut_list);
                    }
                    return cut_list;
                }
                else
                {
                    return cut_list;
                }
            }
    };
}

#endif
