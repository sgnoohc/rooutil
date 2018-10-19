#include "cutflowutil.h"

//_______________________________________________________________________________________________________
void RooUtil::CutflowUtil::createCutflowBranches(CutNameListMap& cutlists, RooUtil::TTreeX& tx)
{
    std::map<TString, std::vector<TString>> obj = cutlists.getStdVersion();
    for (auto& cutlist : obj)
    {
        for (auto& cutname : cutlist.second)
        {
            if (!tx.hasBranch<float>(cutname))
                tx.createBranch<float>(cutname);
        }
    }
}

//_______________________________________________________________________________________________________
std::vector<float> RooUtil::CutflowUtil::getCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx)
{
    std::vector<float> rtn;
    float wgtall = 1;
    for (auto& cutname : cutlist)
    {
        float wgt = tx.getBranch<float>(cutname);
        wgtall *= wgt;
        rtn.push_back(wgtall);
    }
    return rtn;
}

//_______________________________________________________________________________________________________
bool RooUtil::CutflowUtil::passCuts(std::vector<TString> cutlist, RooUtil::TTreeX& tx)
{
    std::vector<float> cutflow = getCutflow(cutlist, tx);
    float passwgtall = 1;
    for (auto& passwgt : cutflow)
        passwgtall *= passwgt;
    return (passwgtall != 0);
}

//_______________________________________________________________________________________________________
void RooUtil::CutflowUtil::fillCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h)
{
    std::vector<float> cutflow = getCutflow(cutlist, tx);
    for (unsigned int i = 0; i < cutflow.size(); ++i)
        if (cutflow[i] > 0)
            h->Fill(i, cutflow[i]);
}

//_______________________________________________________________________________________________________
void RooUtil::CutflowUtil::fillRawCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h)
{
    std::vector<float> cutflow = getCutflow(cutlist, tx);
    for (unsigned int i = 0; i < cutflow.size(); ++i)
        if (cutflow[i] > 0)
            h->Fill(i);
}

//_______________________________________________________________________________________________________
std::tuple<std::map<TString, TH1F*>, std::map<TString, TH1F*>> RooUtil::CutflowUtil::createCutflowHistograms(RooUtil::CutflowUtil::CutNameListMap& cutlists)
{
    std::map<TString, std::vector<TString>> obj = cutlists.getStdVersion();
    return createCutflowHistograms(obj);
}

//_______________________________________________________________________________________________________
std::tuple<std::map<TString, TH1F*>, std::map<TString, TH1F*>> RooUtil::CutflowUtil::createCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists)
{
    std::map<TString, TH1F*> cutflows;
    std::map<TString, TH1F*> rawcutflows;
    for (auto& cutlist : cutlists)
    {
        cutflows[cutlist.first] = new TH1F(cutlist.first + "_cutflow", "", cutlist.second.size(), 0, cutlist.second.size());
        rawcutflows[cutlist.first] = new TH1F(cutlist.first + "_rawcutflow", "", cutlist.second.size(), 0, cutlist.second.size());
    }
    return std::make_tuple(cutflows, rawcutflows);
}

//_______________________________________________________________________________________________________
void RooUtil::CutflowUtil::fillCutflowHistograms(RooUtil::CutflowUtil::CutNameListMap& cutlists, RooUtil::TTreeX& tx, std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows)
{
    std::map<TString, std::vector<TString>> obj = cutlists.getStdVersion();
    fillCutflowHistograms(obj, tx, cutflows, rawcutflows);
}

//_______________________________________________________________________________________________________
void RooUtil::CutflowUtil::fillCutflowHistograms(std::map<TString, std::vector<TString>>& cutlists, RooUtil::TTreeX& tx, std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows)
{
    for (auto& cutlist : cutlists)
    {
        RooUtil::CutflowUtil::fillCutflow(cutlist.second, tx, cutflows[cutlist.first]);
        RooUtil::CutflowUtil::fillRawCutflow(cutlist.second, tx, rawcutflows[cutlist.first]);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::CutflowUtil::saveCutflowHistograms(std::map<TString, TH1F*>& cutflows, std::map<TString, TH1F*>& rawcutflows)
{
    for (auto& cutflow : cutflows) cutflow.second->Write();
    for (auto& rawcutflow : rawcutflows) rawcutflow.second->Write();
}
