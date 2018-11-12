#include "anautil.h"

//_______________________________________________________________________________________________________
RooUtil::Cutflow::Cutflow(TFile* o) : cuttree("Root"), last_active_cut(0), ofile(o), t(0), tx(0), iseventlistbooked(false), seterrorcount(0) { cuttreemap["Root"] = &cuttree; }

//_______________________________________________________________________________________________________
RooUtil::Cutflow::~Cutflow() { delete t; delete tx; }

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::addToCutTreeMap(TString n) { if (cuttreemap.find(n.Data()) == cuttreemap.end()) cuttreemap[n.Data()] = cuttree.getCutPointer(n); else error(TString::Format("Cut %s already exists! no duplicate cut names allowed!", n.Data())); }

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
    cuttreemap.erase(cuttreemap.find(n.Data()));
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
    createWgtSystBranches();
//    t->Print();
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookCutflowHistograms(std::vector<TString> regions)
{
    ofile->cd();

    std::tie(cutflow_histograms, rawcutflow_histograms) = RooUtil::CutflowUtil::createCutflowHistograms(cutlists);

    for (auto& syst : systs)
    {
        std::map<CUTFLOWMAPSTRING, TH1F*> cutflow_histograms_tmp;
        std::map<CUTFLOWMAPSTRING, TH1F*> rawcutflow_histograms_tmp;
        std::tie(cutflow_histograms_tmp, rawcutflow_histograms_tmp) = RooUtil::CutflowUtil::createCutflowHistograms(cutlists, syst);
        cutflow_histograms.insert(cutflow_histograms_tmp.begin(), cutflow_histograms_tmp.end());
        rawcutflow_histograms.insert(rawcutflow_histograms_tmp.begin(), rawcutflow_histograms_tmp.end());
    }

    for (auto& cutsyst : cutsysts)
    {
        std::map<CUTFLOWMAPSTRING, TH1F*> cutflow_histograms_tmp;
        std::map<CUTFLOWMAPSTRING, TH1F*> rawcutflow_histograms_tmp;
        std::tie(cutflow_histograms_tmp, rawcutflow_histograms_tmp) = RooUtil::CutflowUtil::createCutflowHistograms(cutlists, cutsyst);
        cutflow_histograms.insert(cutflow_histograms_tmp.begin(), cutflow_histograms_tmp.end());
        rawcutflow_histograms.insert(rawcutflow_histograms_tmp.begin(), rawcutflow_histograms_tmp.end());
    }

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
    TString filename = ofile->GetName();
    TString msg = "Wrote output to " + filename;
    print(msg);
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
    if (!tx)
    {
        TString msg = "No TTreeX object set, setCut() for " + cutname;
        printSetFunctionError(msg);
        return;
    }

#ifdef USE_TTREEX
    tx->setBranch<bool>(cutname, pass, false, true);
    tx->setBranch<float>(cutname+"_weight", weight, false, true);
#else
    cuttreemap[cutname.Data()]->pass_this_cut = pass;
    cuttreemap[cutname.Data()]->weight_this_cut = weight;
#endif

}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setCutSyst(TString cutname, TString syst, bool pass, float weight)
{
    if (!tx)
    {
        TString msg = "No TTreeX object set, setCutSyst() for " + cutname + ", " + syst;
        printSetFunctionError(msg);
        return;
    }
    tx->setBranch<bool>(cutname+syst, pass, false, true);
    tx->setBranch<float>(cutname+syst+"_weight", weight, false, true);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setWgtSyst(TString syst, float weight)
{
    if (!tx)
    {
        TString msg = "No TTreeX object set, setWgtSyst() for " + syst;
        printSetFunctionError(msg);
        return;
    }
    tx->setBranch<float>(syst, weight, false, true);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::addCutSyst(TString syst, std::vector<TString> pattern)
{
    cutsysts.push_back(syst);
    cuttree.addSyst(syst, pattern);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::addWgtSyst(TString syst)
{
    systs.push_back(syst);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::createWgtSystBranches()
{
    for (auto& syst : systs)
    {
        if (!tx->hasBranch<float>(syst))
            tx->createBranch<float>(syst);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setVariable(TString varname, float val)
{
    if (!tx)
    {
        TString msg = "No TTreeX object set, setVariable() for " + varname;
        printSetFunctionError(msg);
        return;
    }
    tx->setBranch<float>(varname, val, false, true);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::setEventID(int run, int lumi, unsigned long long evt)
{
    if (!tx)
    {
        TString msg = "No TTreeX object set, setEventID()";
        printSetFunctionError(msg);
        return;
    }
    tx->setBranch<int>("run", run, false, true);
    tx->setBranch<int>("lumi", lumi, false, true);
    tx->setBranch<unsigned long long>("evt", evt, false, true);
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookEventLists()
{
    if (!tx)
        error("bookEventLists():: No TTreeX has been set. Forgot to call bookCutflows()?");
    if (!tx->hasBranch<int>("run"))
        tx->createBranch<int>("run");
    if (!tx->hasBranch<int>("lumi"))
        tx->createBranch<int>("lumi");
    if (!tx->hasBranch<unsigned long long>("evt"))
        tx->createBranch<unsigned long long>("evt");
    iseventlistbooked = true;
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fill()
{
    if (!tx)
    {
        TString msg = "No TTreeX object set, fill()";
        printSetFunctionError(msg);
        return;
    }
    tx->setBranch<bool>("Root", 1); // Root is internally set
    tx->setBranch<float>("Root_weight", 1); // Root is internally set

    // Evaluate nominal selection cutflows (the non cut varying selections)
    cuttree.evaluate(*tx, "", iseventlistbooked);

    // Nominal cutflow
    fillCutflows();

    // Wgt systematic variations
    for (auto& syst : systs) fillCutflows(syst);

    // Fill nominal histograms
    fillHistograms();

    // Wgt systematic variations
    for (auto& syst : systs) fillHistograms(syst);

    for (auto& cutsyst : cutsysts)
    {
        cuttree.evaluate(*tx, cutsyst, iseventlistbooked);
        fillCutflows(cutsyst, false);
        fillHistograms(cutsyst, false);
    }

//    tx->fill(); // TODO if i want to save this...

    tx->clear();
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fillCutflows(TString syst, bool iswgtsyst)
{
    for (auto& pair : cutlists)
    {
        const TString& region_name = pair.first;
        std::vector<TString>& cutlist = pair.second;
        float wgtsyst = (!syst.IsNull() and iswgtsyst) ? tx->getBranch<float>(syst) : 1;
        fillCutflow(cutlist, cutflow_histograms[(region_name+syst).Data()], rawcutflow_histograms[(region_name+syst).Data()], wgtsyst);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fillCutflow(std::vector<TString>& cutlist, TH1F* h, TH1F* hraw, float wgtsyst)
{
    for (unsigned int i = 0; i < cutlist.size(); ++i)
    {
        bool& pass = cuttreemap[cutlist[i].Data()]->pass;
        if (pass)
        {
            float& weight = cuttreemap[cutlist[i].Data()]->weight;
            h->Fill(i, weight * wgtsyst);
            hraw->Fill(i, 1);
        }
        else
        {
            return;
        }
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::fillHistograms(TString syst, bool iswgtsyst)
{

    float wgtsyst = (!syst.IsNull() and iswgtsyst)? tx->getBranch<float>(syst) : 1.;
    cuttree.fillHistograms(*tx, syst, wgtsyst);

//    for (auto& key1d : booked_histograms_nominal_keys)
//    {
//        TString cutname = std::get<0>(key1d);
//        std::get<1>(key1d) = syst;
//        TString varname = std::get<2>(key1d);
//        bool& passed = cuttreemap[cutname.Data()]->pass;
//        if (!passed)
//            continue;
//        float& weight = cuttreemap[cutname.Data()]->weight;
//        float wgtsyst = (!syst.IsNull() and iswgtsyst)? tx->getBranch<float>(syst) : 1.;
//        TH1F* h = booked_histograms[key1d];
//        h->Fill(tx->getBranch<float>(varname), weight * wgtsyst);
//    }
//
//    for (auto& key2d : booked_2dhistograms_nominal_keys)
//    {
//        TString cutname = std::get<0>(key2d);
//        std::get<1>(key2d) = syst;
//        TString varname = std::get<2>(key2d);
//        TString varnamey = std::get<3>(key2d);
//        bool& passed = cuttreemap[cutname.Data()]->pass;
//        if (!passed)
//            continue;
//        float& weight = cuttreemap[cutname.Data()]->weight;
//        float wgtsyst = (!syst.IsNull() and iswgtsyst)? tx->getBranch<float>(syst) : 1.;
//        TH2F* h = booked_2dhistograms[key2d];
//        h->Fill(tx->getBranch<float>(varname), tx->getBranch<float>(varnamey), weight * wgtsyst);
//    }

//    for (auto& pair : booked_histograms)
//    {
//        TString cutname = std::get<0>(pair.first);
//        TString sysname = std::get<1>(pair.first);
//        TString varname = std::get<2>(pair.first);
//        TH1F* h = pair.second;
//        bool& passed = cuttreemap[cutname.DATA()]->pass;
//        float& weight = cuttreemap[cutname.DATA()]->weight;
//        float wgtsyst = sysname.IsNull() ? 1. : tx->getBranch<float>(sysname);
//        if (passed)
//            h->Fill(tx->getBranch<float>(varname), weight * wgtsyst);
//    }
//
//    for (auto& pair : booked_2dhistograms)
//    {
//        TString cutname = std::get<0>(pair.first);
//        TString sysname = std::get<1>(pair.first);
//        TString varname = std::get<2>(pair.first);
//        TString varnamey = std::get<3>(pair.first);
//        TH2F* h = pair.second;
//        bool& passed = cuttreemap[cutname.DATA()]->pass;
//        float& weight = cuttreemap[cutname.DATA()]->weight;
//        float wgtsyst = sysname.IsNull() ? 1. : tx->getBranch<float>(sysname);
//        if (passed)
//            h->Fill(tx->getBranch<float>(varname), tx->getBranch<float>(varnamey), weight * wgtsyst);
//    }
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
void RooUtil::Cutflow::bookHistogram(TString cut, std::pair<TString, std::tuple<unsigned, float, float>> key, TString syst)
{
    TString varname = key.first;
    unsigned int nbin = std::get<0>(key.second);
    float min = std::get<1>(key.second);
    float max = std::get<2>(key.second);
    TString histname = cut + syst + "__" + varname;
    if (booked_histograms.find(std::make_tuple(cut.Data(), syst.Data(), varname.Data())) == booked_histograms.end())
    {
        booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())] = new TH1F(histname, "", nbin, min, max);
        booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())]->SetDirectory(0);
        booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())]->Sumw2();
        if (syst.IsNull())
        {
            booked_histograms_nominal_keys.push_back(std::make_tuple(cut.Data(), syst.Data(), varname.Data()));
        }
        if (!tx)
            error("bookHistogram():: No TTreeX has been set. Forgot to call bookCutflows()?");
        if (!tx->hasBranch<float>(varname))
            tx->createBranch<float>(varname);
        cuttreemap[cut.Data()]->addHist1D(booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())], varname, syst);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistogram(TString cut, std::pair<TString, std::vector<float>> key, TString syst)
{
    TString varname = key.first;
    std::vector<float> boundaries = key.second;
    TString histname = cut + syst + "__" + varname;
    if (booked_histograms.find(std::make_tuple(cut.Data(), syst.Data(), varname.Data())) == booked_histograms.end())
    {
        Float_t bounds[boundaries.size()];
        for (unsigned int i = 0; i < boundaries.size(); ++i)
            bounds[i] = boundaries[i];
        booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())] = new TH1F(histname, "", boundaries.size()-1, bounds);
        booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())]->SetDirectory(0);
        booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())]->Sumw2();
        if (syst.IsNull())
        {
            booked_histograms_nominal_keys.push_back(std::make_tuple(cut.Data(), syst.Data(), varname.Data()));
        }
        if (!tx)
            error("bookHistogram():: No TTreeX has been set. Forgot to call bookCutflows()?");
        if (!tx->hasBranch<float>(varname))
            tx->createBranch<float>(varname);
        cuttreemap[cut.Data()]->addHist1D(booked_histograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data())], varname, syst);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::book2DHistogram(TString cut, std::pair<std::pair<TString, TString>, std::tuple<unsigned, float, float, unsigned, float, float>> key, TString syst)
{
    TString varname = key.first.first;
    TString varnamey = key.first.second;
    unsigned int nbin = std::get<0>(key.second);
    float min = std::get<1>(key.second);
    float max = std::get<2>(key.second);
    unsigned int nbiny = std::get<0>(key.second);
    float miny = std::get<1>(key.second);
    float maxy = std::get<2>(key.second);
    TString histname = cut + syst + "__" + varname+"_v_"+varnamey;
    if (booked_2dhistograms.find(std::make_tuple(cut.Data(), syst.Data(), varname.Data(), varnamey.Data())) == booked_2dhistograms.end())
    {
        booked_2dhistograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data(), varnamey.Data())] = new TH2F(histname, "", nbin, min, max, nbiny, miny, maxy);
        booked_2dhistograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data(), varnamey.Data())]->SetDirectory(0);
        booked_2dhistograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data(), varnamey.Data())]->Sumw2();
        if (syst.IsNull())
        {
            booked_2dhistograms_nominal_keys.push_back(std::make_tuple(cut.Data(), syst.Data(), varname.Data(), varnamey.Data()));
        }
        if (!tx)
            error("book2DHistogram():: No TTreeX has been set. Forgot to call bookCutflows()?");
        if (!tx->hasBranch<float>(varname))
            tx->createBranch<float>(varname);
        if (!tx->hasBranch<float>(varnamey))
            tx->createBranch<float>(varnamey);
        cuttreemap[cut.Data()]->addHist2D(booked_2dhistograms[std::make_tuple(cut.Data(), syst.Data(), varname.Data(), varnamey.Data())], varname, varnamey, syst);
    }
}

//_______________________________________________________________________________________________________
void RooUtil::Cutflow::bookHistogramsForCut(Histograms& histograms, TString cut)
{
    for (auto& key : histograms.th1fs)        bookHistogram  (cut, key);
    for (auto& key : histograms.th1fs_varbin) bookHistogram  (cut, key);
    for (auto& key : histograms.th2fs)        book2DHistogram(cut, key);
    for (auto& syst : systs)
    {
        for (auto& key : histograms.th1fs)        bookHistogram  (cut, key, syst);
        for (auto& key : histograms.th1fs_varbin) bookHistogram  (cut, key, syst);
        for (auto& key : histograms.th2fs)        book2DHistogram(cut, key, syst);
    }
    for (auto& cutsyst : cutsysts)
    {
        for (auto& key : histograms.th1fs)        bookHistogram  (cut, key, cutsyst);
        for (auto& key : histograms.th1fs_varbin) bookHistogram  (cut, key, cutsyst);
        for (auto& key : histograms.th2fs)        book2DHistogram(cut, key, cutsyst);
    }
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
void RooUtil::Cutflow::printSetFunctionError(TString msg)
{
    if (seterrorcount < 100)
    {
        print(msg);
        seterrorcount++;
    }
    else if (seterrorcount == 100)
    {
        print(msg);
        print("Suppressing Cutflow::set\"Func\"() errors ... ");
        seterrorcount++;
    }
    else
    {
        return;
    }
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
void RooUtil::Histograms::addHistogram(TString name, std::vector<float> boundaries)
{
    if (th1fs_varbin.find(name) == th1fs_varbin.end())
        th1fs_varbin[name] = boundaries;
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

