#include "cutflowutil.h"

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
            h->Fill(i + 1, cutflow[i]);
}

//_______________________________________________________________________________________________________
void RooUtil::CutflowUtil::fillRawCutflow(std::vector<TString> cutlist, RooUtil::TTreeX& tx, TH1F* h)
{
    std::vector<float> cutflow = getCutflow(cutlist, tx);
    for (unsigned int i = 0; i < cutflow.size(); ++i)
        if (cutflow[i] > 0)
            h->Fill(i + 1);
}
