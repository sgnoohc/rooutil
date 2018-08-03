//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "fileutil.h"

TChain* RooUtil::FileUtil::createTChain(TString name, TString inputs)
{
    using namespace std;

    // hadoopmap
    ifstream infile("hadoopmap.txt");
    std::map<TString, TString> _map;
    if (infile.good())
    {
        ifstream mapfile;
        mapfile.open( "hadoopmap.txt" );
        std::string line, oldpath, newpath;
        while ( std::getline( mapfile, line ) ) 
        {
            mapfile >> oldpath >> newpath;
            TString oldpath_tstr = oldpath.c_str();
            TString newpath_tstr = newpath.c_str();
            _map[oldpath_tstr] = newpath_tstr;
        }
    }

    TChain* chain = new TChain(name);
    for (auto& ff : RooUtil::StringUtil::split(inputs, ","))
    {
        RooUtil::print(Form("Adding %s", ff.Data()));
        TString filepath = ff;
        if ( _map.find( ff ) != _map.end() )
            filepath = _map[ff];
        chain->Add(filepath);
    }
    return chain;
}

TMultiDrawTreePlayer* RooUtil::FileUtil::createTMulti(TChain* t)
{
    TMultiDrawTreePlayer* p = new TMultiDrawTreePlayer();
    if (p) p->SetTree((TTree*) t);
    return p;
}

TMultiDrawTreePlayer* RooUtil::FileUtil::createTMulti(TString name, TString inputs)
{
    TChain* t = RooUtil::FileUtil::createTChain(name, inputs);
    return createTMulti(t);
}

TH1* RooUtil::FileUtil::get(TString name)
{
    return (TH1*) gDirectory->Get(name);
}

std::map<TString, TH1*> RooUtil::FileUtil::getAllHistograms(TFile* f)
{
    std::map<TString, TH1*> hists;
    for (int ikey = 0; ikey < f->GetListOfKeys()->GetEntries(); ++ikey)
    {
        TString histname = f->GetListOfKeys()->At(ikey)->GetName();
        hists[histname] = (TH1*) f->Get(histname);
    }
    return hists;
}

void RooUtil::FileUtil::saveAllHistograms(std::map<TString, TH1*> allhists, TFile* ofile)
{
    ofile->cd();
    for (auto& hist : allhists)
        if (hist.second)
            hist.second->Write();
}

void RooUtil::FileUtil::saveJson(json& j, TFile* ofile, TString jsonname)
{
    ofile->cd();
    std::string s = j.dump();
    std::vector<std::string> v = {s};
    ofile->cd();
    ofile->WriteObjectAny(&v, "vector<string>", jsonname.Data());
}

json RooUtil::FileUtil::getJson(TFile* ofile, TString jsonname)
{
    ofile->cd();
    std::vector<std::string>* vp = 0;
    ofile->GetObject(jsonname.Data(), vp);
    std::string s = vp->at(0);
    json j = json::parse(s);
    return j;
}
