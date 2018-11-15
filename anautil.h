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
            std::map<TString, std::vector<float>> th1fs_varbin;
            std::map<TString, std::tuple<unsigned int, float, float>> th1fs;
            std::map<std::pair<TString, TString>, std::tuple<unsigned int, float, float, unsigned int, float, float>> th2fs;
            Histograms();
            ~Histograms();
            void addHistogram(TString, unsigned int, float, float);
            void addHistogram(TString, std::vector<float>);
            void add2DHistogram(TString, unsigned int, float, float, TString, unsigned int, float, float);
    };

    class Cutflow
    {
        public:
            CutTree cuttree;
            CutTree* last_active_cut; // when getCut is called this is set
            std::map<TREEMAPSTRING, CutTree*> cuttreemap;
            std::map<CUTFLOWMAPSTRING, TH1F*> cutflow_histograms;
            std::map<CUTFLOWMAPSTRING, TH1F*> rawcutflow_histograms;
            std::map<std::tuple<TREEMAPSTRING, TREEMAPSTRING, TREEMAPSTRING>, TH1F*> booked_histograms; // key is <cutname, syst, varname>
            std::map<std::tuple<TREEMAPSTRING, TREEMAPSTRING, TREEMAPSTRING, TREEMAPSTRING>, TH2F*> booked_2dhistograms; // key is <cutname, syst, varname, varnamey>
            std::vector<std::tuple<TREEMAPSTRING, TREEMAPSTRING, TREEMAPSTRING>> booked_histograms_nominal_keys; // key is <cutname, syst="", varname>
            std::vector<std::tuple<TREEMAPSTRING, TREEMAPSTRING, TREEMAPSTRING, TREEMAPSTRING>> booked_2dhistograms_nominal_keys; // key is <cutname, syst="", varname, varnamey>
            TFile* ofile;
            TTree* t;
            TTreeX* tx;
            std::vector<TString> cutsysts;
            std::vector<TString> systs;
            std::map<TString, std::vector<TString>> cutlists;
            bool iseventlistbooked;
            int seterrorcount;
            Cutflow(TFile* o);
            ~Cutflow();
            void addToCutTreeMap(TString n);
            void setLastActiveCut(TString n);
            void addCut(TString n);
            void addCutToLastActiveCut(TString n);
            void copyAndEditCuts(TString, std::map<TString, TString>);
            void printCuts();
            CutTree& getCut(TString n);
            void removeCut(TString n);
            void setCutLists(std::vector<TString> regions);
            void bookCutflowTree(std::vector<TString> regions);
            void bookCutflowHistograms(std::vector<TString> regions);
            void bookCutflowsForRegions(std::vector<TString> regions);
            void bookCutflows();
            void saveOutput();
            void saveCutflows();
            void saveHistograms();
#ifdef USE_CUTLAMBDA
            void setCut    (TString cutname, std::function<bool()> pass, std::function<float()> weight);
            void setCutSyst(TString cutname, TString syst, std::function<bool()> pass, std::function<float()> weight);
#else
            void setCut(TString cutname, bool pass, float weight);
            void setCutSyst(TString cutname, TString syst, bool pass, float weight);
#endif
            void addCutSyst(TString syst, std::vector<TString> pattern);
            void addWgtSyst(TString syst);
            void setWgtSyst(TString syst, float weight); // TODO make TTreeX using lambda...?
            void createWgtSystBranches();
            void setVariable(TString varname, float);
            void setEventID(int, int, unsigned long long);
            void bookEventLists();
            void fill();
            void fillCutflows(TString syst="", bool iswgtsyst=true);
            void fillCutflow(std::vector<TString>& cutlist, TH1F* h, TH1F* hraw, float wgtsyst=1);
            void fillHistograms(TString syst="", bool iswgtsyst=true);
            void bookHistogram(TString, std::pair<TString, std::tuple<unsigned, float, float>>, TString="");
            void bookHistogram(TString, std::pair<TString, std::vector<float>>, TString="");
            void book2DHistogram(TString, std::pair<std::pair<TString, TString>, std::tuple<unsigned, float, float, unsigned, float, float>>, TString="");
            void bookHistograms(Histograms& histograms);
            void bookHistograms(Histograms& histograms, std::vector<TString> cutlist);
            void bookHistogramsForCut(Histograms& histograms, TString);
            void bookHistogramsForCutAndBelow(Histograms& histograms, TString);
            void bookHistogramsForCutAndAbove(Histograms& histograms, TString);
            void bookHistogramsForEndCuts(Histograms& histograms);
            void printSetFunctionError(TString msg);
    };
}

#endif
