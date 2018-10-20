#ifndef anautil_h
#define anautil_h

#include "cutflowutil.h"
#include "ttreex.h"
#include "printutil.h"
#include <utility>
#include <vector>
#include <map>
#include <tuple>
#include "TH1.h"
#include "TString.h"
#include <iostream>
#include <algorithm>

namespace RooUtil
{
    class Histograms
    {
        public:
            std::map<TString, std::tuple<unsigned int, float, float>> th1fs;
            std::map<std::pair<TString, TString>, std::tuple<unsigned int, float, float, int, float, float>> th2fs;
            Histograms();
            ~Histograms();
            void addHistogram(TString, unsigned int, float, float);
            void add2DHistogram(TString, unsigned int, float, float, TString, unsigned int, float, float);
    };

    class Cutflow
    {
        public:
            CutTree cuttree;
            CutTree* last_active_cut; // when getCut is called this is set
            std::map<TString, TH1F*> cutflow_histograms;
            std::map<TString, TH1F*> rawcutflow_histograms;
            std::map<std::tuple<TString, TString>, TH1F*> booked_histograms; // key is <cutname, varname>
            std::map<std::tuple<TString, TString, TString>, TH2F*> booked_2dhistograms; // key is <cutname, varname, varnamey>
            TFile* ofile;
            TTree* t;
            TTreeX* tx;
            std::map<TString, std::vector<TString>> cutlists;
            Cutflow(TFile* o);
            ~Cutflow();
            void setLastActiveCut(TString n);
            void addCut(TString n);
            void addCutToLastActiveCut(TString n);
            void printCuts();
            CutTree& getCut(TString n);
            void setCutLists(std::vector<TString> regions);
            void bookCutflowTree(std::vector<TString> regions);
            void bookCutflowHistograms(std::vector<TString> regions);
            void bookCutflowsForRegions(std::vector<TString> regions);
            void bookCutflows();
            void saveOutput();
            void saveCutflows();
            void saveHistograms();
            void setCut(TString cutname, bool pass, float weight);
            void setVariable(TString varname, float);
            void fill();
            void fillCutflows();
            void fillHistograms();
            void bookHistogram(TString, std::pair<TString, std::tuple<unsigned, int, int>>);
            void book2DHistogram(TString, std::pair<std::pair<TString, TString>, std::tuple<unsigned, int, int, unsigned, int, int>>);
            void bookHistograms(Histograms& histograms);
            void bookHistograms(Histograms& histograms, std::vector<TString> cutlist);
            void bookHistogramsForCut(Histograms& histograms, TString);
            void bookHistogramsForCutAndBelow(Histograms& histograms, TString);
            void bookHistogramsForCutAndAbove(Histograms& histograms, TString);
    };
}

#endif
