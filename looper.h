//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef looper_cc
#define looper_cc

// C/C++
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdarg.h>
#include <functional>
#include <cmath>

// ROOT
#include "TBenchmark.h"
#include "TBits.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TChainElement.h"
#include "TTreeCache.h"
#include "TTreePerfStats.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "Math/LorentzVector.h"

#include "printutil.h"

namespace RooUtil
{

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Looper class
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // NOTE: This class assumes accessing TTree in the SNT style which uses the following,
    // https://github.com/cmstas/Software/blob/master/makeCMS3ClassFiles/makeCMS3ClassFiles.C
    // It is assumed that the "template" class passed to this class will have
    // 1. "Init(TTree*)"
    // 2. "GetEntry(uint)"
    // 3. "progress(nevtProc'ed, total)"
    template <class TREECLASS>
    class Looper
    {
        // Members
        TChain* tchain;
        TObjArray *listOfFiles;
        TObjArrayIter* fileIter;
        TFile* tfile;
        TTree* ttree;
        TTreePerfStats* ps;
        unsigned int nEventsTotalInChain;
        unsigned int nEventsTotalInTree;
        int nEventsToProcess;
        unsigned int nEventsProcessed;
        unsigned int indexOfEventInTTree;
        bool fastmode;
        TREECLASS* treeclass;
        TStopwatch my_timer;
        int bar_id;
        int print_rate;
        bool doskim;
        TString skimfilename;
        TFile* skimfile;
        TTree* skimtree;
        unsigned int nEventsSkimmed;
        std::vector<TString> skimbrfiltpttn;
        bool silent;
        public:
        // Functions
        Looper( TChain* chain = 0, TREECLASS* treeclass = 0, int nEventsToProcess = -1 );
        ~Looper();
        void setTChain( TChain* c );
        void setTreeClass( TREECLASS* t );
        void printCurrentEventIndex();
        void setSilent(bool s=true) { silent = s; }
        bool allEventsInTreeProcessed();
        bool allEventsInChainProcessed();
        bool nextEvent();
        TTree* getTree() { return ttree; }
        unsigned int getNEventsProcessed() { return nEventsProcessed; }
        void setSkim( TString ofilename );
        void setSkimBranchFilterPattern( std::vector<TString> x ) { skimbrfiltpttn = x; }
        void fillSkim();
        void saveSkim();
        TTree* getSkimTree() { return skimtree; }
        void setSkimMaxSize( Long64_t maxsize ) { skimtree->SetMaxTreeSize( maxsize ); }
        TTreePerfStats* getTTreePerfStats() { return ps; }
        private:
        void setFileList();
        void setNEventsToProcess();
        bool nextTree();
        bool nextEventInTree();
        void initProgressBar();
        void printProgressBar(bool force=false);
        void createSkimTree();
        void copyAddressesToSkimTree();
    };

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// Event Looper (Looper) class template implementation
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////

// It's easier to put the implementation in the header file to avoid compilation issues.

//_________________________________________________________________________________________________
template <class TREECLASS>
RooUtil::Looper<TREECLASS>::Looper( TChain* c, TREECLASS* t, int nevtToProc ) :
    tchain( 0 ),
    listOfFiles( 0 ),
    fileIter( 0 ),
    tfile( 0 ),
    ttree( 0 ),
    ps( 0 ),
    nEventsTotalInChain( 0 ),
    nEventsTotalInTree( 0 ),
    nEventsToProcess( nevtToProc ),
    nEventsProcessed( 0 ),
    indexOfEventInTTree( 0 ),
    fastmode( true ),
    treeclass( 0 ),
    bar_id( 0 ),
    print_rate( 432 ),
    doskim( false ),
    skimfilename( "" ),
    skimfile( 0 ),
    skimtree( 0 ),
    nEventsSkimmed( 0 ),
    silent( false )
{
    initProgressBar();
    print( "Start EventLooping" );
    start();

    if ( c )
        setTChain( c );

    if ( t )
        setTreeClass( t );

    if ( nevtToProc > 5000 )
        fastmode = true;

    c->GetEntry( 0 );
    t->Init( c->GetTree() );
}

//_________________________________________________________________________________________________
template <class TREECLASS>
RooUtil::Looper<TREECLASS>::~Looper()
{
    print( "Finished EventLooping" );
    end();

    if ( fileIter )
        delete fileIter;

    if ( tfile )
        delete tfile;
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::setTChain( TChain* c )
{
    if ( c )
    {
        tchain = c;
        setNEventsToProcess();
        setFileList();
    }
    else
        error( "You provided a null TChain pointer!", __FUNCTION__ );
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::setTreeClass( TREECLASS* t )
{
    if ( t )
        treeclass = t;
    else
        error( "You provided a null TreeClass pointer!", __FUNCTION__ );
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::printCurrentEventIndex()
{
    RooUtil::print( TString::Format( "Current TFile = %s", tfile->GetName() ) );
    RooUtil::print( TString::Format( "Current TTree = %s", ttree->GetName() ) );
    RooUtil::print( TString::Format( "Current Entry # in TTree = %d", indexOfEventInTTree ) );
}

//_________________________________________________________________________________________________
template <class TREECLASS>
bool RooUtil::Looper<TREECLASS>::nextTree()
{
    if ( !fileIter )
        error( "fileIter not set but you are trying to access the next file", __FUNCTION__ );

    // Get the TChainElement from TObjArrayIter.
    // If no more to run over, Next returns 0.
    TChainElement* chainelement = ( TChainElement* ) fileIter->Next();

    if ( chainelement )
    {
        // If doskim is true and if this is the very first file being opened in the TChain,
        // flag it to create a tfile and ttree where the skimmed events will go to.
        bool createskimtree = false;

        if ( !ttree && doskim )
            createskimtree = true;

        // If there is already a TFile opened from previous iteration, close it.
        if ( tfile )
            tfile->Close();

        // Open up a new file
        tfile = new TFile( chainelement->GetTitle() );
        // Get the ttree
        ttree = ( TTree* ) tfile->Get( tchain->GetName() );

        if ( !ttree )
            error( "TTree is null!??", __FUNCTION__ );

        // Set some fast mode stuff
        if ( fastmode )
            TTreeCache::SetLearnEntries( 10 );

        if ( fastmode )
            ttree->SetCacheSize( 128 * 1024 * 1024 );

        // Print some info to stdout
        print( "Working on " +
                TString( tfile->GetName() ) +
                "/TTree:" +
                TString( ttree->GetName() ) );
        printProgressBar(true);
        // Reset the nEventsTotalInTree in this tree
        nEventsTotalInTree = ttree->GetEntries();
        // Reset the event index as we got a new ttree
        indexOfEventInTTree = 0;
        // Set the ttree to the TREECLASS
        treeclass->Init( ttree );

        // If skimming create the skim tree after the treeclass inits it.
        // This is to make sure the branch addresses are correct.
        if ( createskimtree )
            createSkimTree();
        else if ( doskim )
            copyAddressesToSkimTree();

//        // TTreePerfStats
//        if ( ps )
//            ps->SaveAs( "perf.root" );
//
//        ps = new TTreePerfStats( "ioperf", ttree );
        // Return that I got a good one
        return true;
    }
    else
    {
        // Announce that we are done with this chain
        //        print("");
        //        print("Done with all trees in this chain", __FUNCTION__);
        return false;
    }
}

//_________________________________________________________________________________________________
template <class TREECLASS>
bool RooUtil::Looper<TREECLASS>::allEventsInTreeProcessed()
{
    if ( indexOfEventInTTree >= nEventsTotalInTree )
        return true;
    else
        return false;
}

//_________________________________________________________________________________________________
template <class TREECLASS>
bool RooUtil::Looper<TREECLASS>::allEventsInChainProcessed()
{
    if ( nEventsProcessed >= ( unsigned int ) nEventsToProcess )
        return true;
    else
        return false;
}

//_________________________________________________________________________________________________
template <class TREECLASS>
bool RooUtil::Looper<TREECLASS>::nextEventInTree()
{
    //    treeclass->progress(nEventsProcessed, nEventsToProcess);
    // Sanity check before loading the next event.
    if ( !ttree )
        error( "current ttree not set!", __FUNCTION__ );

    if ( !tfile )
        error( "current tfile not set!", __FUNCTION__ );

    if ( !fileIter )
        error( "fileIter not set!", __FUNCTION__ );

    // Check whether I processed everything
    if ( allEventsInTreeProcessed() )
        return false;

    if ( allEventsInChainProcessed() )
        return false;

    // if fast mode do some extra
    if ( fastmode )
        ttree->LoadTree( indexOfEventInTTree );

    // Set the event index in TREECLASS
    treeclass->GetEntry( indexOfEventInTTree );
    // Increment the counter for this ttree
    ++indexOfEventInTTree;
    // Increment the counter for the entire tchain
    ++nEventsProcessed;
    // Print progress
    printProgressBar();
    // If all fine return true
    return true;
}

//_________________________________________________________________________________________________
template <class TREECLASS>
bool RooUtil::Looper<TREECLASS>::nextEvent()
{
    // If no tree it means this is the beginning of the loop.
    if ( !ttree )
    {
        //        std::cout << " I think this is the first tree " << std::endl;
        // Load the next tree if it returns true, then proceed to next event in tree.
        while ( nextTree() )
        {
            // If the next event in tree was successfully loaded return true, that it's good.
            if ( nextEventInTree() )
            {
                //                std::cout << " I think this is the first event in first tree" << std::endl;
                return true;
            }
            // If the first event in this tree was not good, continue to the next tree
            else
                continue;
        }

        // If looping over all trees, we fail to find first event that's good,
        // return false and call it quits.
        // At this point it will exit the loop without processing any events.
        //        printProgressBar();
        return false;
    }
    // If tree exists, it means that we're in the middle of a loop
    else
    {
        // If next event is successfully loaded proceed.
        if ( nextEventInTree() )
            return true;
        // If next event is not loaded then check why.
        else
        {
            // If failed because it was the last event in the whole chain to process, exit the loop.
            // You're done!
            if ( allEventsInChainProcessed() )
            {
                //                printProgressBar();
                return false;
            }
            // If failed because it's last in the tree then load the next tree and the event
            else if ( allEventsInTreeProcessed() )
            {
                // Load the next tree if it returns true, then proceed to next event in tree.
                while ( nextTree() )
                {
                    // If the next event in tree was successfully loaded return true, that it's good.
                    if ( nextEventInTree() )
                        return true;
                    // If the first event in this tree was not good, continue to the next tree
                    else
                        continue;
                }

                // If looping over all trees, we fail to find first event that's good,
                // return false and call it quits.
                // Again you're done!
                //                printProgressBar();
                return false;
            }
            else
            {
                // Why are you even here?
                // spit error and return false to avoid warnings
                error( "You should not be here! Please contact philip@physics.ucsd.edu", __FUNCTION__ );
                return false;
            }
        }
    }
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::setFileList()
{
    if ( !fileIter )
    {
        listOfFiles = tchain->GetListOfFiles();
        fileIter = new TObjArrayIter( listOfFiles );
    }
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::setNEventsToProcess()
{
    if ( tchain )
    {
        nEventsTotalInChain = tchain->GetEntries();

        if ( nEventsToProcess < 0 )
            nEventsToProcess = nEventsTotalInChain;

        if ( nEventsToProcess > ( int ) nEventsTotalInChain )
        {
            print( TString::Format(
                        "Asked to process %d events, but there aren't that many events",
                        nEventsToProcess ) );
            nEventsToProcess = nEventsTotalInChain;
        }

        print( TString::Format( "Total Events in this Chain to process = %d", nEventsToProcess ) );
    }
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::initProgressBar()
{
    /// Init progress bar
    my_timer.Start();
    bar_id = 0;
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::printProgressBar(bool force)
{

    if (silent)
        return;

    /// Print progress bar

    int entry = nEventsProcessed;
    int totalN = nEventsToProcess;

    if ( totalN < 20 )
        totalN = 20;

    // Progress bar
    if ( entry > totalN )
    {
//        printf( "Why are you here?\n" );
    }
    else if ( entry == totalN )
    {
        Double_t elapsed = my_timer.RealTime();
        Double_t rate;

        if ( elapsed != 0 )
            rate = entry / elapsed;
        else
            rate = -999;

        const int mins_in_hour = 60;
        const int secs_to_min = 60;
        Int_t input_seconds = elapsed;
        Int_t seconds = input_seconds % secs_to_min;
        Int_t minutes = input_seconds / secs_to_min % mins_in_hour;
        Int_t hours   = input_seconds / secs_to_min / mins_in_hour;

        printf( "\rRooUtil::" );
        printf( "+" );
        printf( "|====================" );

        //for ( int nb = 0; nb < 20; ++nb )
        //{
        //  printf("=");
        //}

        printf( "| %.1f %% (%d/%d) with  [avg. %d Hz]   Total Time: %.2d:%.2d:%.2d         \n", 100.0, entry, totalN,
                ( int )rate, hours, minutes, seconds );
        fflush( stdout );
    }
    //else if ( entry % ( ( ( int ) print_rate ) ) < (0.3) * print_rate || force )
    else if ( entry % ( ( ( int ) print_rate ) ) == 0 || force )
    {

        // sanity check
        if ( entry >= totalN +
                10 ) // +2 instead of +1 since, the loop might be a while loop where to check I got a bad event the index may go over 1.
        {
            TString msg = TString::Format( "%d %d", entry, totalN );
            RooUtil::print( msg, __FUNCTION__ );
            RooUtil::error( "Total number of events processed went over max allowed! Check your loop boundary conditions!!",
                    __FUNCTION__ );
        }

        int nbars = entry / ( totalN / 20 );
        Double_t elapsed = my_timer.RealTime();
        Double_t rate;

        if ( elapsed != 0 )
            rate = entry / elapsed;
        else
            rate = -999;

        Double_t percentage = entry / ( totalN * 1. ) * 100;
        const int mins_in_hour = 60;
        const int secs_to_min = 60;
        Int_t input_seconds = ( totalN - entry ) / rate;
        Int_t seconds = input_seconds % secs_to_min;
        Int_t minutes = input_seconds / secs_to_min % mins_in_hour;
        Int_t hours   = input_seconds / secs_to_min / mins_in_hour;

        print_rate = ( int )( rate / 5 ) + 1;

        printf( "RooUtil:: " );

        if ( bar_id % 4 == 3 )
            printf( "-" );

        if ( bar_id % 4 == 2 )
            printf( "/" );

        if ( bar_id % 4 == 1 )
            printf( "|" );

        if ( bar_id % 4 == 0 )
            printf( "\\" );

        printf( "|" );
        bar_id ++;

        for ( int nb = 0; nb < 20; ++nb )
        {
            if ( nb < nbars )
                printf( "=" );
            else
                printf( "." );
        }

        printf( "| %.1f %% (%d/%d) with  [%d Hz]   ETA %.2d:%.2d:%.2d         \r", percentage, entry + 1, totalN, ( int )rate,
                hours, minutes, seconds );
        fflush( stdout );

    }

    my_timer.Start( kFALSE );
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::setSkim( TString ofilename )
{
    skimfilename = ofilename;
    doskim = true;
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::createSkimTree()
{
    skimfile = new TFile( skimfilename, "recreate" );
    TObjArray* toa = ttree->GetListOfBranches();

    if ( skimbrfiltpttn.size() > 0 )
    {
        ttree->SetBranchStatus( "*", 0 );

        for ( auto& pttn : skimbrfiltpttn )
        {
            for ( const auto& brobj : *toa )
            {
                TString brname = brobj->GetName();

                if ( pttn.Contains( "*" ) )
                {
                    TString modpttn = pttn;
                    modpttn.ReplaceAll( "*", "");
                    if ( brname.Contains( modpttn ) && brname.BeginsWith( modpttn ) )
                    {
                        // std::cout << brname << std::endl;
                        ttree->SetBranchStatus( brname + "*", 1 );
                    }
                }
                else
                {
                    if ( brname.EqualTo( pttn ) )
                    {
                        // std::cout << brname << std::endl;
                        ttree->SetBranchStatus( brname, 1 );
                    }
                }
            }
        }
    }

    skimtree = ttree->CloneTree( 0 );
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::copyAddressesToSkimTree()
{
    ttree->CopyAddresses( skimtree );
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::fillSkim()
{
    treeclass->LoadAllBranches();
    skimtree->Fill();
    nEventsSkimmed++;
}

//_________________________________________________________________________________________________
template <class TREECLASS>
void RooUtil::Looper<TREECLASS>::saveSkim()
{
    double frac_skimmed = ( double ) nEventsSkimmed / ( double ) nEventsProcessed * 100;
    RooUtil::print( Form( "Skimmed events %d out of %d. [%f%%]", nEventsSkimmed, nEventsProcessed, frac_skimmed ) );
    skimtree->GetCurrentFile()->cd();
    skimtree->Write();
    //    skimfile->Close();
}

#endif
