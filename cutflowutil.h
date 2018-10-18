#ifndef cutflowutil_h
#define cutflowutil_h

#include "ttreex.h"
#include <tuple>
#include <vector>
#include <map>
#include "TH1.h"

namespace RooUtil
{
    namespace CutflowUtil
    {
        std::vector<float> getCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx);
        bool passCuts(std::vector<TString> cutlist, RooUtil::TTreeX& tx);
        void fillCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
        void fillRawCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h);
        std::tuple<std::map<TString, TH1F*>, std::map<TString, TH1F*>> createCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists);
        void fillCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists, RooUtil::TTreeX& tx, std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);
        void saveCutflowHistograms(std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows);
    }
}

#endif
