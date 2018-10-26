#include "anautil.h"

//_______________________________________________________________________________________________________
RooUtil::Cutflow::Cutflow(TFile* o) : cuttree("Root"), last_active_cut(0), ofile(o), t(0), tx(0) { cuttreemap["Root"] = &cuttree; }

//_______________________________________________________________________________________________________
RooUtil::Cutflow::~Cutflow() { delete t; delete tx; }

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::addToCutTreeMap(TString n) { if (cuttreemap.find(n) == cuttreemap.end()) cuttreemap[n] = cuttree.getCutPointer(n); else error(TString::Format("Cut %s already exists! no duplicate cut names allowed!", n.Data())); }

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setLastActiveCut(TString n) { last_active_cut = cuttree.getCutPointer(n); }

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::addCut(TString n) { cuttree.addCut(n); addToCutTreeMap(n); setLastActiveCut(n); }

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::addCutToLastActiveCut(TString n) { last_active_cut->addCut(n); addToCutTreeMap(n); setLastActiveCut(n); }

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::printCuts() { cuttree.printCuts(); }

//_______________________________________________________________________________________________________
CutTree& RooUtil::Cutflow::getCut(TString n) { CutTree& c = cuttree.getCut(n); setLastActiveCut(n); return c; }

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::removeCut(TString n)
{
    CutTree* c = cuttree.getCutPointer(n);
    c->parent->children.erase(std::find(c->parent->children.begin(), c->parent->children.end(), c));
    cuttreemap.erase(cuttreemap.find(n));
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setCutLists(std::vector<TString> regions)
{
    for (auto& region : regions)
    {
        cutlists[region] = cuttree.getCutList(region);
//        std::cout << region << std::endl;
//        for (auto& cutname : cutlists[region])
//            std::cout << cutname << std::endl;
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookCutflowTree(std::vector<TString> regions)
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
//    t->Print();
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookCutflowHistograms(std::vector<TString> regions)
{
    ofile->cd();
    std::tie(cutflow_histograms, rawcutflow_histograms) = RooUtil::CutflowUtil::createCutflowHistograms(cutlists);
    for (auto& cutflow_histogram : cutflow_histograms)
    {
        TString msg = "Booked cutflow histogram for cut = " + cutflow_histogram.first;
        print(msg);
    }
    for (auto& rawcutflow_histogram : rawcutflow_histograms)
    {
        TString msg = "Booked rawcutflow histogram for cut = " + rawcutflow_histogram.first;
        print(msg);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookCutflowsForRegions(std::vector<TString> regions)
{
    setCutLists(regions);
    bookCutflowTree(regions);
    bookCutflowHistograms(regions);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookCutflows()
{
    std::vector<TString> regions = cuttree.getEndCuts();
    setCutLists(regions);
    bookCutflowTree(regions);
    bookCutflowHistograms(regions);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::saveOutput()
{
    saveCutflows();
    saveHistograms();
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::saveCutflows()
{
    // Save cutflow histograms
    ofile->cd();
    RooUtil::CutflowUtil::saveCutflowHistograms(cutflow_histograms, rawcutflow_histograms);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::saveHistograms()
{
    ofile->cd();
    for (auto& pair : booked_histograms)
        pair.second->Write();
    for (auto& pair : booked_2dhistograms)
        pair.second->Write();
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setCut(TString cutname, bool pass, float weight)
{
    if (!tx->hasBranch<bool>(cutname))
        return;
    tx->setBranch<bool>(cutname, pass);
    tx->setBranch<float>(cutname+"_weight", weight);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setVariable(TString varname, float val)
{
    tx->setBranch<float>(varname, val);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setEventID(int run, int lumi, unsigned long long evt)
{
    tx->setBranch<int>("run", run);
    tx->setBranch<int>("lumi", lumi);
    tx->setBranch<unsigned long long>("evt", evt);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookEventLists()
{
    if (!tx->hasBranch<int>("run"))
        tx->createBranch<int>("run");
    if (!tx->hasBranch<int>("lumi"))
        tx->createBranch<int>("lumi");
    if (!tx->hasBranch<unsigned long long>("evt"))
        tx->createBranch<unsigned long long>("evt");
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fill()
{
    tx->setBranch<bool>("Root", 1); // Root is internally set
    tx->setBranch<float>("Root_weight", 1); // Root is internally set
    cuttree.evaluate(*tx);
    fillCutflows();
    fillHistograms();
    tx->fill();
    tx->clear();
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fillCutflows()
{
//    RooUtil::CutflowUtil::fillCutflowHistograms(cutlists, *tx, cutflow_histograms, rawcutflow_histograms);
    for (auto& pair : cutlists)
    {
        const TString& region_name = pair.first;
        std::vector<TString>& cutlist = pair.second;
        fillCutflow(cutlist, cutflow_histograms[region_name], rawcutflow_histograms[region_name]);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fillCutflow(std::vector<TString>& cutlist, TH1F* h, TH1F* hraw)
{
    for (unsigned int i = 0; i < cutlist.size(); ++i)
    {
        bool& pass = cuttreemap[cutlist[i]]->pass;
        if (pass)
        {
            float& weight = cuttreemap[cutlist[i]]->weight;
            h->Fill(i, weight);
            hraw->Fill(i, 1);
        }
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fillHistograms()
{
//    std::map<std::tuple<TString, TString>, TH1F*> booked_histograms; // key is <cutname, varname>
//    std::map<std::tuple<TString, TString, TString>, TH2F*> booked_2dhistograms; // key is <cutname, varname, varnamey>
    for (auto& pair : booked_histograms)
    {
        TString cutname = std::get<0>(pair.first);
        TString varname = std::get<1>(pair.first);
        TH1F* h = pair.second;
        bool& passed = cuttreemap[cutname]->pass;
        float& weight = cuttreemap[cutname]->weight;
        if (passed)
            h->Fill(tx->getBranch<float>(varname), weight);
    }

    for (auto& pair : booked_2dhistograms)
    {
        TString cutname = std::get<0>(pair.first);
        TString varname = std::get<1>(pair.first);
        TString varnamey = std::get<2>(pair.first);
        TH2F* h = pair.second;
        bool& passed = cuttreemap[cutname]->pass;
        float& weight = cuttreemap[cutname]->weight;
        if (passed)
            h->Fill(tx->getBranch<float>(varname), tx->getBranch<float>(varnamey), weight);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistograms(Histograms& histograms)
{
    std::vector<TString> regions = cuttree.getEndCuts();
    for (auto& region : regions)
    {
        std::vector<TString> cutlist = cuttree.getCutList(region);
        bookHistograms(histograms, cutlist);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistograms(Histograms& histograms, std::vector<TString> cutlist)
{
    for (auto& cut : cutlist)
    {
        bookHistogramsForCut(histograms, cut);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistogram(TString cut, std::pair<TString, std::tuple<unsigned, int, int>> key)
{
    TString varname = key.first;
    unsigned int nbin = std::get<0>(key.second);
    float min = std::get<1>(key.second);
    float max = std::get<2>(key.second);
    TString histname = cut + "__" + varname;
    if (booked_histograms.find(std::make_tuple(cut, varname)) == booked_histograms.end())
    {
        booked_histograms[std::make_tuple(cut, varname)] = new TH1F(histname, "", nbin, min, max);
        booked_histograms[std::make_tuple(cut, varname)]->SetDirectory(0);
        if (!tx)
            error("bookHistogram():: No TTreeX has been set. Forgot to call bookCutflows()?");
        if (!tx->hasBranch<float>(varname))
            tx->createBranch<float>(varname);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::book2DHistogram(TString cut, std::pair<std::pair<TString, TString>, std::tuple<unsigned, int, int, unsigned, int, int>> key)
{
    TString varname = key.first.first;
    TString varnamey = key.first.second;
    unsigned int nbin = std::get<0>(key.second);
    float min = std::get<1>(key.second);
    float max = std::get<2>(key.second);
    unsigned int nbiny = std::get<0>(key.second);
    float miny = std::get<1>(key.second);
    float maxy = std::get<2>(key.second);
    TString histname = cut + "__" + varname+"_v_"+varnamey;
    if (booked_2dhistograms.find(std::make_tuple(cut, varname, varnamey)) == booked_2dhistograms.end())
    {
        booked_2dhistograms[std::make_tuple(cut, varname, varnamey)] = new TH2F(histname, "", nbin, min, max, nbiny, miny, maxy);
        booked_2dhistograms[std::make_tuple(cut, varname, varnamey)]->SetDirectory(0);
        if (!tx)
            error("book2DHistogram():: No TTreeX has been set. Forgot to call bookCutflows()?");
        if (!tx->hasBranch<float>(varname))
            tx->createBranch<float>(varname);
        if (!tx->hasBranch<float>(varnamey))
            tx->createBranch<float>(varnamey);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistogramsForCut(Histograms& histograms, TString cut)
{
    for (auto& key : histograms.th1fs) bookHistogram(cut, key);
    for (auto& key : histograms.th2fs) book2DHistogram(cut, key);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistogramsForCutAndBelow(Histograms& histograms, TString cut)
{
    std::vector<TString> cutlist = cuttree.getCutListBelow(cut);
    for (auto& cut : cutlist)
    {
        bookHistogramsForCut(histograms, cut);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistogramsForCutAndAbove(Histograms& histograms, TString cut)
{
    error("bookHistogramsForCutAndAbove not yet implemented");
}

//_______________________________________________________________________________________________________
RooUtil::Histograms::Histograms()
{
}

//_______________________________________________________________________________________________________
RooUtil::Histograms::~Histograms()
{
}

//_______________________________________________________________________________________________________
void RooUtil::Histograms::addHistogram(TString name, unsigned int n, float min, float max)
{
    if (th1fs.find(name) == th1fs.end())
        th1fs[name] = std::make_tuple(n, min, max);
    else
        error(TString::Format("histogram already exists name = %s", name.Data()));
}

//_______________________________________________________________________________________________________
void RooUtil::Histograms::add2DHistogram(TString name, unsigned int n, float min, float max, TString namey, unsigned int ny, float miny, float maxy)
{
    if (th2fs.find(std::make_pair(name, namey)) == th2fs.end())
        th2fs[std::make_pair(name, namey)] = std::make_tuple(n, min, max, ny, miny, maxy);
    else
        error(TString::Format("histogram already exists name = %s", (name+"_v_"+namey).Data()));
}

