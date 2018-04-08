#include "duplicate.h"

void RooUtil::remove_duplicate(TChain* chain, TString output, const char* run_bname, const char* lumi_bname, const char* evt_bname, int size=0)
{
    cout << chain->GetEntries() << endl;
    TFile* ofile = TFile::Open(output, "RECREATE");
    TTree* otree = chain.CloneTree(0);

    if (size)
    {
        otree->SetMaxTreeSize(size);
    }

    int run = 0;
    int lumi = 0;
    unsigned long long evt = 0;

    otree->SetBranchAddress(run_bname, &run);
    otree->SetBranchAddress(lumi_bname, &lumi);
    otree->SetBranchAddress(evt_bname, &evt);

    for (Long64_t ientry = 0; ientry < chain->GetEntries(); ++ientry)
    {
        chain->GetEntry(ientry);
        duplicate_removal::DorkyEventIdentifier id(run, evt, lumi);
        if (duplicate_removal::is_duplicate(id))
            continue; 
        otree->Fill();
    }

    otree->Write();
}
