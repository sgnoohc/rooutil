#include "process.h"

AnalysisConfig ana;

//=============================================================================================
// main()
//=============================================================================================
int main(int argc, char** argv)
{

    parseArguments(argc, argv);
    initializeInputsAndOutputs();
    nt.SetYear(2018); // comment this out in case you don't need to override
    setupAnalysis();

    // Looping input file
    while (ana.looper.nextEvent())
    {

        // If splitting jobs are requested then determine whether to process the event or not based on remainder
        if (ana.job_index != -1 and ana.nsplit_jobs != -1)
        {
            if (ana.looper.getNEventsProcessed() % ana.nsplit_jobs != (unsigned int) ana.job_index)
                continue;
        }

        ana.tx->clear();

        runAnalysis();

        ana.tx->fill();
        ana.cutflow.fill();
    }

    ana.cutflow.saveOutput();
    ana.tx->write();

    delete ana.output_tfile;
}

//=============================================================================================
// Setup analysis (prior to the event looping)
//=============================================================================================
void setupAnalysis()
{
    ana.tx->createBranch<vector<LV>>("reco_leptons_p4");
    ana.tx->createBranch<vector<int>>("reco_leptons_tightid");
    ana.tx->createBranch<vector<int>>("reco_leptons_pdgId");
    ana.tx->createBranch<vector<LV>>("reco_jets_p4");
    ana.tx->createBranch<vector<int>>("reco_jets_bloose");
    ana.tx->createBranch<vector<int>>("reco_jets_bmedium");
    ana.tx->createBranch<vector<int>>("reco_jets_btight");
    ana.tx->createBranch<int>("nbloose");
    ana.tx->createBranch<int>("nbmedium");
    ana.tx->createBranch<int>("nbtight");

    ana.cutflow.addCut("Weight", [&]() { return 1/*set your cut here*/; }, [&]() { return 1; } );

    // Book cutflows
    ana.cutflow.bookCutflows();

    // Book Histograms
    ana.cutflow.bookHistogramsForCutAndBelow(ana.histograms, "Weight");

}

//=============================================================================================
// runAnalysis (within the event looping)
//=============================================================================================
void runAnalysis()
{
    // Select muons
    for (unsigned int imu = 0; imu < nt.Muon_pt().size(); ++imu)
    {
        if (SS::muonID(imu, SS::IDfakable, nt.year()))
        {
            ana.tx->pushbackToBranch<LV>("reco_leptons_p4", nt.Muon_p4()[imu]);
            ana.tx->pushbackToBranch<int>("reco_leptons_tightid", SS::muonID(imu, SS::IDtight, nt.year()));
            ana.tx->pushbackToBranch<int>("reco_leptons_pdgId", (-nt.Muon_charge()[imu]) * 13);
        }
    }

    // Select electrons
    for (unsigned int iel = 0; iel < nt.Electron_pt().size(); ++iel)
    {
        if (SS::electronID(iel, SS::IDfakable, nt.year()))
        {
            ana.tx->pushbackToBranch<LV>("reco_leptons_p4", nt.Electron_p4()[iel]);
            ana.tx->pushbackToBranch<int>("reco_leptons_tightid", SS::electronID(iel, SS::IDtight, nt.year()));
            ana.tx->pushbackToBranch<int>("reco_leptons_pdgId", (-nt.Electron_charge()[iel]) * 11);
        }
    }

    ana.tx->sortVecBranchesByPt("reco_leptons_p4", {}, {"reco_leptons_tightid", "reco_leptons_pdgId"}, {});

    // Select jets
    int nbloose = 0;
    int nbmedium = 0;
    int nbtight = 0;
    for (unsigned int ijet = 0; ijet < nt.Jet_pt().size(); ++ijet)
    {
        // Read jet p4
        const LV& jet_p4 = nt.Jet_p4()[ijet];

        // Overlap check against good leptons
        bool isOverlap = false;
        for (auto& lep_p4 : ana.tx->getBranchLazy<vector<LV>>("reco_leptons_p4"))
        {
            if (RooUtil::Calc::DeltaR(jet_p4, lep_p4) < 0.4)
            {
                isOverlap = true;
                break;
            }
        }

        // Then skip
        if (isOverlap)
            continue;

        if (not (jet_p4.pt() > 20.))
            continue;

        if (not (fabs(jet_p4.eta()) < 5.0))
            continue;

        ana.tx->pushbackToBranch<LV>("reco_jets_p4", jet_p4);
        bool is_loose_btagged = nt.Jet_btagDeepFlavB()[ijet] > 0.0521;
        bool is_medium_btagged = nt.Jet_btagDeepFlavB()[ijet] > 0.3033;
        bool is_tight_btagged = nt.Jet_btagDeepFlavB()[ijet] > 0.7489;

        if (is_loose_btagged) nbloose++;
        if (is_medium_btagged) nbmedium++;
        if (is_tight_btagged) nbtight++;

        ana.tx->pushbackToBranch<int>("reco_jets_bloose", is_loose_btagged);
        ana.tx->pushbackToBranch<int>("reco_jets_bmedium", is_medium_btagged);
        ana.tx->pushbackToBranch<int>("reco_jets_btight", is_tight_btagged);

    }

    ana.tx->setBranch<int>("nbloose", nbloose);
    ana.tx->setBranch<int>("nbmedium", nbmedium);
    ana.tx->setBranch<int>("nbtight", nbtight);

}


















//-----------------======================-----------------======================-----------------======================-----------------=======================
//-----------------======================-----------------======================-----------------======================-----------------=======================
//-----------------======================-----------------======================-----------------======================-----------------=======================
//-----------------======================-----------------======================-----------------======================-----------------=======================

void parseArguments(int argc, char** argv)
{
//********************************************************************************
//
// 1. Parsing options
//
//********************************************************************************

    // cxxopts is just a tool to parse argc, and argv easily

    // Grand option setting
    cxxopts::Options options("\n  $ doAnalysis",  "\n         **********************\n         *                    *\n         *       Looper       *\n         *                    *\n         **********************\n");

    // Read the options
    options.add_options()
        ("i,input"       , "Comma separated input file list OR if just a directory is provided it will glob all in the directory BUT must end with '/' for the path", cxxopts::value<std::string>())
        ("t,tree"        , "Name of the tree in the root file to open and loop over"                                             , cxxopts::value<std::string>())
        ("o,output"      , "Output file name"                                                                                    , cxxopts::value<std::string>())
        ("n,nevents"     , "N events to loop over"                                                                               , cxxopts::value<int>()->default_value("-1"))
        ("j,nsplit_jobs" , "Enable splitting jobs by N blocks (--job_index must be set)"                                         , cxxopts::value<int>())
        ("I,job_index"   , "job_index of split jobs (--nsplit_jobs must be set. index starts from 0. i.e. 0, 1, 2, 3, etc...)"   , cxxopts::value<int>())
        ("d,debug"       , "Run debug job. i.e. overrides output option to 'debug.root' and 'recreate's the file.")
        ("h,help"        , "Print help")
        ;

    auto result = options.parse(argc, argv);

    // NOTE: When an option was provided (e.g. -i or --input), then the result.count("<option name>") is more than 0
    // Therefore, the option can be parsed easily by asking the condition if (result.count("<option name>");
    // That's how the several options are parsed below

    //_______________________________________________________________________________
    // --help
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --input
    if (result.count("input"))
    {
        ana.input_file_list_tstring = result["input"].as<std::string>();
    }
    else
    {
        std::cout << options.help() << std::endl;
        std::cout << "ERROR: Input list is not provided! Check your arguments" << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --tree
    if (result.count("tree"))
    {
        ana.input_tree_name = result["tree"].as<std::string>();
    }
    else
    {
        std::cout << options.help() << std::endl;
        std::cout << "ERROR: Input tree name is not provided! Check your arguments" << std::endl;
        exit(1);
    }

    //_______________________________________________________________________________
    // --debug
    if (result.count("debug"))
    {
        ana.output_tfile = new TFile("debug.root", "recreate");
        ana.tx = new RooUtil::TTreeX("variable", "variable");
    }
    else
    {
        //_______________________________________________________________________________
        // --output
        if (result.count("output"))
        {
            ana.output_tfile = new TFile(result["output"].as<std::string>().c_str(), "create");
            ana.tx = new RooUtil::TTreeX("variable", "variable");
            if (not ana.output_tfile->IsOpen())
            {
                std::cout << options.help() << std::endl;
                std::cout << "ERROR: output already exists! provide new output name or delete old file. OUTPUTFILE=" << result["output"].as<std::string>() << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: Output file name is not provided! Check your arguments" << std::endl;
            exit(1);
        }
    }

    //_______________________________________________________________________________
    // --nevents
    ana.n_events = result["nevents"].as<int>();

    //_______________________________________________________________________________
    // --nsplit_jobs
    if (result.count("nsplit_jobs"))
    {
        ana.nsplit_jobs = result["nsplit_jobs"].as<int>();
        if (ana.nsplit_jobs <= 0)
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --nsplit_jobs" << ana.nsplit_jobs << " has zero or negative value!" << std::endl;
            std::cout << "I am not sure what this means..." << std::endl;
            exit(1);
        }
    }
    else
    {
        ana.nsplit_jobs = -1;
    }

    //_______________________________________________________________________________
    // --nsplit_jobs
    if (result.count("job_index"))
    {
        ana.job_index = result["job_index"].as<int>();
        if (ana.job_index < 0)
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --job_index" << ana.job_index << " has negative value!" << std::endl;
            std::cout << "I am not sure what this means..." << std::endl;
            exit(1);
        }
    }
    else
    {
        ana.job_index = -1;
    }


    // Sanity check for split jobs (if one is set the other must be set too)
    if (result.count("job_index") or result.count("nsplit_jobs"))
    {
        // If one is not provided then throw error
        if ( not (result.count("job_index") and result.count("nsplit_jobs")))
        {
            std::cout << options.help() << std::endl;
            std::cout << "ERROR: option string --job_index and --nsplit_jobs must be set at the same time!" << std::endl;
            exit(1);
        }
        // If it is set then check for sanity
        else
        {
            if (ana.job_index >= ana.nsplit_jobs)
            {
                std::cout << options.help() << std::endl;
                std::cout << "ERROR: --job_index >= --nsplit_jobs ! This does not make sense..." << std::endl;
                exit(1);
            }
        }
    }

    //
    // Printing out the option settings overview
    //
    std::cout <<  "=========================================================" << std::endl;
    std::cout <<  " Setting of the analysis job based on provided arguments " << std::endl;
    std::cout <<  "---------------------------------------------------------" << std::endl;
    std::cout <<  " ana.input_file_list_tstring: " << ana.input_file_list_tstring <<  std::endl;
    std::cout <<  " ana.output_tfile: " << ana.output_tfile->GetName() <<  std::endl;
    std::cout <<  " ana.n_events: " << ana.n_events <<  std::endl;
    std::cout <<  " ana.nsplit_jobs: " << ana.nsplit_jobs <<  std::endl;
    std::cout <<  " ana.job_index: " << ana.job_index <<  std::endl;
    std::cout <<  "=========================================================" << std::endl;

}

void initializeInputsAndOutputs()
{

    // Create the TChain that holds the TTree's of the baby ntuples
    ana.events_tchain = RooUtil::FileUtil::createTChain(ana.input_tree_name, ana.input_file_list_tstring);

    ana.looper.init(ana.events_tchain, &nt, ana.n_events);

    // Set the cutflow object output file
    ana.cutflow.setTFile(ana.output_tfile);

}

