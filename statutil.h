#ifndef STATUTIL_h
#define STATUTIL_h
//
//---------------------------
//
//     Fit to single bin
//
//---------------------------
//
//
// Adopted               by Philip Chang       UCSD            March 16, 2021
// Original code written by Michael Schmitt    Northwestern    November 1, 2019
//
//
//
//
//
#include <iomanip>
#include <math.h>
#include <iostream>
#include <string>
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TMinuit.h"
#include "TLatex.h"
#include "TLine.h"
#include "TGraph.h"
using namespace std;

#define nP 2

namespace RooUtil
{
    namespace StatUtil
    {
        //
        // Run-time options
        //
        extern Bool_t FixBackgrounds;
        extern Bool_t DrawGraph;
        extern Bool_t Verbose;
        //
        // parameters
        //
        extern Double_t mu_cen;
        extern Double_t mu_unc;
        extern Double_t beta;
        // log-normal parameters
        extern Double_t beta_W;
        extern Double_t beta_A;
        //
        // Data
        //      S = signal in channel n from source m.
        //      B = total background in channel n
        //      N = observed yield in channel n
        //
        extern Double_t S;
        extern Double_t B;
        extern Double_t N;

        void setFixBackgrounds(Bool_t);
        void setDrawGraph(Bool_t);
        void setVerbose(Bool_t);

        Double_t findW(Double_t Z);
        void setupData(Double_t S_, Double_t B_, Double_t BSyst_);
        Double_t termEval(Double_t S_, Double_t B_, Double_t N_);
        Double_t NLLFunS(Double_t mu_, Double_t beta_);
        Double_t NLLFunB(Double_t beta_, Double_t W_, Double_t A_);
        Double_t NLLFun(Double_t mu_, Double_t beta_);
        Double_t NLLFunVec( Double_t pvec[nP] );
        void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
        void doFit();
        float doScanSingle();
        float cut_and_count_95percent_limit(Double_t S_, Double_t B_, Double_t BSyst_, Bool_t verbose=false);

    }
}

#endif
