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
                for (int i = 0; i < indent; ++i) std::cout << " ";
                std::cout << name << std::endl;
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
        };

        class Cutflow
        {
            public:
            CutTree cuttree;
            CutTree* last_active_cut; // when getCut is called this is set
            std::map<TString, TH1F*> cutflow_histograms;
            std::map<TString, TH1F*> rawcutflow_histograms;
            TFile* ofile;
            TTree* t;
            TTreeX* tx;
            std::map<TString, std::vector<TString>> cutlists;
            Cutflow(TFile* o) : cuttree("Root"), last_active_cut(0), ofile(o), t(0), tx(0) {}
            ~Cutflow() { delete t; delete tx; }
            void setLastActiveCut(TString n) { last_active_cut = cuttree.getCutPointer(n); }
            void addCut(TString n) { cuttree.addCut(n); setLastActiveCut(n); }
            void addCutToLastActiveCut(TString n) { last_active_cut->addCut(n); setLastActiveCut(n); }
            void printCuts() { cuttree.printCuts(); }
            CutTree& getCut(TString n) { CutTree& c = cuttree.getCut(n); setLastActiveCut(n); return c; }
            void setCutLists(std::vector<TString> regions)
            {
                for (auto& region : regions)
                {
                    cutlists[region] = cuttree.getCutList(region);
                    std::cout << region << std::endl;
                    for (auto& cutname : cutlists[region])
                        std::cout << cutname << std::endl;
                }
            }
            void bookCutflowTree(std::vector<TString> regions)
            {
                if (!t)
                {
                    ofile->cd();
                    t = new TTree("cut_tree", "cut_tree");
                    t->SetDirectory(0);
                }
                if (!tx)
                {
                    ofile->cd();
                    tx = new TTreeX(t);
                }
                RooUtil::CutflowUtil::createCutflowBranches(cutlists, *tx);
                t->Print();
            }
            void bookCutflowHistograms(std::vector<TString> regions)
            {
                ofile->cd();
                std::tie(cutflow_histograms, rawcutflow_histograms) = createCutflowHistograms(cutlists);
                for (auto& cutflow_histogram : cutflow_histograms)
                {
                    std::cout << "Booked cutflow histogram for cut = " << cutflow_histogram.first << std::endl;
                    cutflow_histogram.second->Print("all");
                }
                for (auto& rawcutflow_histogram : rawcutflow_histograms)
                {
                    std::cout << "Booked rawcutflow histogram for cut = " << rawcutflow_histogram.first << std::endl;
                    rawcutflow_histogram.second->Print("all");
                }
            }
            void bookCutflowsForRegions(std::vector<TString> regions)
            {
                setCutLists(regions);
                bookCutflowTree(regions);
                bookCutflowHistograms(regions);
            }
            void saveOutput()
            {
                // Save cutflow histograms
                ofile->cd();
                RooUtil::CutflowUtil::saveCutflowHistograms(cutflow_histograms, rawcutflow_histograms);
            }
            void setCut(TString cutname, bool pass, float weight)
            {
                tx->setBranch<bool>(cutname, pass);
                tx->setBranch<float>(cutname+"_weight", weight);
            }
            void fillCutflows()
            {
                tx->setBranch<bool>("Root", 1); // Root is internally set
                tx->setBranch<float>("Root_weight", 1); // Root is internally set
                RooUtil::CutflowUtil::fillCutflowHistograms(cutlists, *tx, cutflow_histograms, rawcutflow_histograms);
                tx->fill();
                tx->clear();
            }
        };

    }
}

#endif
