//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef printutil_cc
#define printutil_cc

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

namespace RooUtil
{

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Printing functions
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // No namespace given in order to minimize typing
    // (e.g. RooUtil::print v. RooUtil::NAMESPACE::print)
    void clearline(int numchar=100);
    void print    (TString msg="", const char* fname="", int flush_before=0, int flush_after=0);
    void error    (TString msg, const char* fname="", int is_error=1);
    void warning  (TString msg, const char* fname="");
    void announce (TString msg="", int quiet=0);
    void start    (int quiet=0, int sleep_time=0);
    void end      (int quiet=0);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// Printing utilities
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//_________________________________________________________________________________________________
void RooUtil::clearline(int numchar)
{
    printf("\r");
    for (int i=0;i<numchar;++i)
        printf(" ");
    printf("\r");
}

//_________________________________________________________________________________________________
void RooUtil::print(TString msg, const char* fname, int flush_before, int flush_after)
{
    /// printf replacement
    clearline();
    if (flush_before) printf("\n");
    if (strlen(fname) == 0) printf("RooUtil:: %s\n", msg.Data());
    else printf("RooUtil:: [in func %s()] %s\n", fname, msg.Data());
    if (flush_after) printf("\n");
    fflush(stdout);
}

//_________________________________________________________________________________________________
void RooUtil::warning(TString msg, const char* fname)
{
    /// warning message. Does not exit the program.
    print("WARNING - "+msg+".\n", fname);
}

//_________________________________________________________________________________________________
void RooUtil::error(TString msg, const char* fname, int is_error)
{
    /// Error & exit
    if (!is_error)
        return;
    //exit();
    print("ERROR - "+msg+", exiting.\n", fname);
    abort();
}

//_________________________________________________________________________________________________
void RooUtil::start(int q, int sleep_time)
{
    /// Fun start (from TM Hong's BaBar days)
    if (q)
        return;
    print(" _");
    print("/\\\\");
    print("\\ \\\\  \\__/ \\__/");
    print(" \\ \\\\ (oo) (oo)  Here we come!");
    print("  \\_\\\\/~~\\_/~~\\_");
    print(" _.-~===========~-._");
    print("(___________________)");
    print("      \\_______/");
    print();
    print("System info:");
    gSystem->Exec("hostname");
    gSystem->Exec("uname -a");
    gSystem->Exec("date");
    gSystem->Exec("whoami");
    gSystem->Exec("pwd");
    fflush(stdout);
    if (sleep_time>0)
        sleep(sleep_time);
}

//_________________________________________________________________________________________________
void RooUtil::announce(TString msg, Int_t q)
{
    /// Fun message presented by the moose
    if (q)
        return;
    // Random
    srand(time(NULL));      // Init rand seed
    Int_t   r = rand()%10+1;// Generate rand number
    Int_t   moose = r>4 ? 1 : 0;
    // Moose type
    TString eyes  = "open";
    if      (r==9) eyes = "closed";
    else if (r==8) eyes = "dead";
    else if (r==7) eyes = "small";
    else if (r==7) eyes = "sunny";
    else if (r==6) eyes = "calc";
    else if (r==4) eyes = "crazy";
    else if (r==3) eyes = "vampire";
    else if (r==2) eyes = "rich";
    else if (r==1) eyes = "sick";
    print();
    if      (msg.Length()>0) print("________________________________________");
    if      (msg.Length()>0) print(msg);
    if      (msg.Length()>0) print("--O-------------------------------------");
    if      (moose)          print("  O    \\_\\_    _/_/");
    if      (moose)          print("   O       \\__/");
    else                     print("   O       ^__^");
    if      (eyes=="open")   print("    o      (oo)\\_______");
    else if (eyes=="closed") print("    o      (==)\\_______");
    else if (eyes=="dead")   print("    o      (xx)\\_______");
    else if (eyes=="small")  print("    o      (..)\\_______");
    else if (eyes=="sunny")  print("    o      (66)\\_______");
    else if (eyes=="calc")   print("    o      (00)\\_______");
    else if (eyes=="crazy")  print("    o      (**)\\_______");
    else if (eyes=="vampire")print("    o      (@@)\\_______");
    else if (eyes=="rich")   print("    o      ($$)\\_______");
    else if (eyes=="sick")   print("    o      (++)\\_______");
    if (true)                print("     o     (__)\\       )\\/\\");
    if      (eyes=="dead")   print("            U  ||----w |");
    else if (eyes=="crazy")  print("            U  ||----w |");
    else if (eyes=="sick")   print("            U  ||----w |");
    else if (eyes=="vampire")print("            VV ||----w |");
    else                     print("               ||----w |");
    if (true)                print("               ||     ||");
    print();
    //sleep(0);
}

//_________________________________________________________________________________________________
void RooUtil::end(int q)
{
    /// Fun exit (from TM Hong's BaBar days)
    if (q)
        return;
    print();
    print("   \\__/    \\__/");
    print("   (oo)    (oo)");
    print("(\\//~~\\\\  //~~\\\\");
    print(" \\/\\__//  \\\\__//\\");
    print("   ||||    ||\\\\ Who cares!");
    print("__ |||| __ |||| ___");
    print("  (_)(_)  (_)(_)");
    print();
}

#endif
