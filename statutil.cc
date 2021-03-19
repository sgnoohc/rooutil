#include "statutil.h"

Bool_t RooUtil::StatUtil::FixBackgrounds = false;
Bool_t RooUtil::StatUtil::DrawGraph = false;
Bool_t RooUtil::StatUtil::Verbose = false;

//
// parameters
//
Double_t RooUtil::StatUtil::mu_cen;
Double_t RooUtil::StatUtil::mu_unc;
Double_t RooUtil::StatUtil::beta;

// log-normal parameters
Double_t RooUtil::StatUtil::beta_W;
Double_t RooUtil::StatUtil::beta_A;

void RooUtil::StatUtil::setFixBackgrounds(Bool_t v)
{
    FixBackgrounds = v;
}

void RooUtil::StatUtil::setDrawGraph(Bool_t v)
{
    DrawGraph = v;
}

void RooUtil::StatUtil::setVerbose(Bool_t v)
{
    Verbose = v;
}

//
// Data
//      S = signal in channel n from source m.
//      B = total background in channel n
//      N = observed yield in channel n
//
Double_t RooUtil::StatUtil::S;
Double_t RooUtil::StatUtil::B;
Double_t RooUtil::StatUtil::N;

//
// Find the sigma parameter needed for the log-normal distribution.
Double_t RooUtil::StatUtil::findW(Double_t Z)
{
    Double_t Wbeg = Z / 2.;
    Double_t Wend = Z * 2.;
    const Int_t nS = 100000;
    Double_t dW = (Wend - Wbeg) / Double_t(nS);
    Double_t diffSmallest = 1.e20;
    Double_t Wbest = Z;
    for (Double_t x = Wbeg; x < Wend; x += dW)
    {
        Double_t y = sqrt(exp(3. * x * x) * (exp(x * x) - 1.));
        Double_t d = fabs(y - Z);
        if (d < diffSmallest)
        {
            diffSmallest = d;
            Wbest = x;
        }
    }
    return Wbest;
}




void RooUtil::StatUtil::setupData(Double_t S_, Double_t B_, Double_t BSyst_)
{
    if (Verbose)
        cout << "\nSetup data...\n\n";

    Double_t uncty;

    B = B_;
    S = S_;
    N = B + 1e-5 * S_;

    if (Verbose)
        cout << "\nConstraint data: \n";

    uncty = BSyst_;
    beta_W = findW(uncty);     beta_A = pow(uncty,2);
    if (Verbose)
        printf("1-bin\tB= %6.2f   uncty= %6.2f    W: %7.3f A: %7.4f\n", B, uncty, beta_W, beta_A);

}




Double_t RooUtil::StatUtil::termEval(Double_t S_, Double_t B_, Double_t N_)
{
  Double_t val = (S_ + B_) - N * log(S_ + B_);
  return val;
}




//======================================================================
Double_t RooUtil::StatUtil::NLLFunS(Double_t mu_, Double_t beta_)
{
  Double_t S_ = mu_ * S;
  Double_t B_ = beta_ * B;
  Double_t term = termEval(S_,B_,N);
  return term;
}


//======================================================================
// constraint term
Double_t RooUtil::StatUtil::NLLFunB(Double_t beta_, Double_t W_, Double_t A_)
{
    Double_t term = (log(beta_) - A_) / W_;
    Double_t val = beta_ + 0.5 * pow(term, 2);
    return val;
}


Double_t RooUtil::StatUtil::NLLFun(Double_t mu_, Double_t beta_)
{
    Double_t NLL = NLLFunS(mu_, beta_);
    Double_t NLLB = NLLFunB(beta_, beta_W, beta_A);
    Double_t val = NLL + NLLB;
    return val;
}

Double_t RooUtil::StatUtil::NLLFunVec( Double_t pvec[nP] ) {
  Double_t val = NLLFun(pvec[0], pvec[1]);
  return val;
}



void RooUtil::StatUtil::fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
    Double_t mu_ = par[0];
    Double_t beta_ = par[1];
    f = NLLFun(mu_, beta_);
}

void RooUtil::StatUtil::doFit()
{
    if (Verbose)
        cout << "\nDo the fit...\n";
    //
    // Set up MINUIT
    //
    TMinuit *gMinuit = new TMinuit(nP);
    gMinuit->SetFCN(fcn);
    gMinuit->SetPrintLevel(-1);
    gMinuit->SetErrorDef(0.5); // NLL fit
    //
    // Prepare the fit
    //
    Double_t vstart[nP], step[nP];
    for (Int_t i = 0; i < nP; ++i)
    {
        vstart[i] = 1.;
        step[i] = 0.1;
    }
    vstart[0] = 0.; // This is a limit machinery
    Int_t ierflg = 0;
    gMinuit->mnparm(0, "mu", vstart[0], step[0], 0, 0, ierflg);
    gMinuit->mnparm(1, "beta", vstart[1], step[1], 0, 0, ierflg);
    if (ierflg != 0)
    {
        cout << "Abort.  Problem defining parameters for fit.\n";
        return;
    }
    //
    // Fix backgrounds nuisance parameters
    //
    if (FixBackgrounds)
    {
        for (Int_t i = 4; i < nP; ++i)
        {
            gMinuit->FixParameter(i);
        }
    }
    //
    // Do the fit
    //
    Double_t arglist[2] = {10000., 1.};
    gMinuit->SetPrintLevel(-1);
    gMinuit->mnexcm("MIGRAD", arglist, 2, ierflg);
    if (ierflg != 0)
    {
        cout << "After MIGRAD, ierflg = " << ierflg << endl;
    }
    //
    // Get the parameters
    //
    gMinuit->SetPrintLevel(-1);
    gMinuit->GetParameter(0, mu_cen, mu_unc);
    if (Verbose)
    {
        cout << "\n"
            << "\t\t\t\t-----------\n"
            << "\t\t\t\tFIT RESULTS\n"
            << "\t\t\t\t-----------\n\n";
        cout << "FixBackgrounds      = " << FixBackgrounds << endl;
        cout << endl;
        printf("mu = %8.5f +/- %7.5f\toverall mu\tprecision:%6.3f\n", mu_cen, mu_unc, mu_unc / mu_cen);
    }
    //
    Double_t betacen, betaunc;
    gMinuit->GetParameter(1, betacen, betaunc);
    if (Verbose)
        printf("\nbeta= %8.5f +/- %7.5f\t background\tprecision:%6.3f\n", betacen, betaunc, betaunc / betacen);

    //------------------------------
    // Calculate delta-NLL
    //------------------------------

    if (Verbose)
        cout << "\nDelta-NLL analysis:\n";
    Double_t parCen[nP], parUnc[nP], tmpCen[nP], tmpUnc[nP];
    Double_t val0, val1, vari, stdev, pval;

    for (Int_t k = 0; k < nP; ++k)
    {
        gMinuit->GetParameter(k, parCen[k], parUnc[k]);
    }
    val1 = NLLFunVec(parCen);
    for (Int_t k = 0; k < nP; ++k)
    {
        gMinuit->Release(k);
    }
    if (FixBackgrounds)
    {
        for (Int_t k = 4; k < nP; ++k)
        {
            gMinuit->FixParameter(k);
        }
    }
    gMinuit->mnparm(0, "mu", 0., step[0], 0, 0, ierflg);
    gMinuit->FixParameter(0);
    gMinuit->mnexcm("MIGRAD", arglist, 2, ierflg);
    if (ierflg != 0)
    {
        cout << "After MIGRAD, ierflg = " << ierflg << endl;
    }
    for (Int_t k = 0; k < nP; ++k)
    {
        gMinuit->GetParameter(k, tmpCen[k], tmpUnc[k]);
    }
    val0 = NLLFunVec(tmpCen);
    vari = 2. * (val0 - val1);
    stdev = sqrt(vari);
    pval = 0.5 * (1. + TMath::Erf(-stdev / sqrt(2.)));
    if (Verbose)
    {
        printf("  mu  \t2*DNLL = %7.4f\tst.dev.= %5.2f\tp-value:%10.3g\n", vari, stdev, pval);
        cout << " *** Warning.  Needs checking.   ***\n";
    }
}





float RooUtil::StatUtil::doScanSingle()
{
    if (Verbose)
        cout << "\n\nDo scans for signal strength...\n\n";

    const Int_t NVMax = 10000;
    Double_t muV[NVMax], NLLV[NVMax];
    Int_t NV;
    Double_t NLLmin = 1.e10;
    const Double_t dmux = 0.01;
    Double_t parCen[nP], parUnc[nP];

    TMinuit *gMinuit = new TMinuit(nP);
    gMinuit->SetFCN(fcn);
    gMinuit->SetPrintLevel(-1);
    gMinuit->SetErrorDef(0.5); // NLL fit

    // --------------------------------------------------------------------------------
    // Scan mu
    if (Verbose)
        printf("\nScan mu............\n");
    Double_t mucen = 100.;
    NV = 0;
    NLLmin = 1.e10;
    for (Double_t mux = 100.; mux > -dmux / 2.; mux -= dmux)
    {
        gMinuit->SetPrintLevel(-1);
        Int_t ierflg = 0;
        Double_t vstart = 1.;
        Double_t step = 0.1;
        gMinuit->mnparm(0, "mu", mux, step, 0, 0, ierflg);
        gMinuit->mnparm(1, "beta", vstart, step, 0, 0, ierflg);
        for (Int_t k = 0; k < nP; ++k)
        {
            gMinuit->Release(k);
        }
        for (Int_t k = 0; k < 4; ++k)
        {
            gMinuit->FixParameter(k);
        }
        if (FixBackgrounds)
        {
            for (Int_t k = 4; k < nP; ++k)
            {
                gMinuit->FixParameter(k);
            }
        }
        Double_t arglist[100];
        gMinuit->mnexcm("MIGRAD", arglist, 2, ierflg);
        for (Int_t k = 0; k < nP; ++k)
        {
            gMinuit->GetParameter(k, parCen[k], parUnc[k]);
        }
        Double_t val = NLLFunVec(parCen);
        // std::cout <<  " val: " << val <<  " parCen[0]: " << parCen[0] <<  std::endl;
        muV[NV] = mux;
        if (fabs(mux) < dmux / 2.) muV[NV] = 0; // annoying feature when not quite zero.
        NLLV[NV] = val;
        NV++;
        if (val < NLLmin)
        {
            NLLmin = val;
            mucen = mux;
        }
    }
    for (Int_t i = 0; i < NV; ++i)
    {
        NLLV[i] = 2. * (NLLV[i] - NLLmin);
    } // note: convert to 2DNLL
    TGraph *GR0 = new TGraph(NV, muV, NLLV);
    //  GR0->Print("all");
    Double_t NLLupmin = 1.e10;
    Double_t NLLdnmin = 1.e10;
    Double_t muup = 10.;
    Double_t mudn = -10.;
    for (Int_t i = 0; i < NV; ++i)
    {
        if (muV[i] > mucen)
        {
            if (fabs(NLLV[i] - 1.) < NLLupmin)
            {
                NLLupmin = fabs(NLLV[i] - 1.);
                muup = muV[i];
            }
        }
        else
        {
            if (fabs(NLLV[i] - 1.) < NLLdnmin)
            {
                NLLdnmin = fabs(NLLV[i] - 1.);
                mudn = muV[i];
            }
        }
    }
    if (Verbose)
    {
        printf("mucen:\t%6.3f\tNLLmin= %f\n", mucen, NLLmin);
        printf("68 percent CI:\t%6.3f\t%6.3f\n", mudn, muup);
        printf("error bars:   \t%6.3f\t%6.3f\n", mudn - mucen, muup - mucen);
    }
    NLLupmin = 1.e10;
    NLLdnmin = 1.e10;
    Double_t muup2 = 10.;
    Double_t mudn2 = -10.;
    for (Int_t i = 0; i < NV; ++i)
    {
        if (muV[i] > mucen)
        {
            if (fabs(NLLV[i] - 4.) < NLLupmin)
            {
                NLLupmin = fabs(NLLV[i] - 4.);
                muup2 = muV[i];
            }
        }
        else
        {
            if (fabs(NLLV[i] - 4.) < NLLdnmin)
            {
                NLLdnmin = fabs(NLLV[i] - 4.);
                mudn2 = muV[i];
            }
        }
    }
    if (Verbose)
        printf("95 percent CI:\t%6.3f\t%6.3f\n", mudn2, muup2);

    if (DrawGraph)
    {
        // Draw graph
        //
        gROOT->SetStyle("Plain");
        TCanvas *C0 = new TCanvas("C0", "mu0", 1000, 10, 800, 500);
        gStyle->SetOptStat(0);
        // C0->SetGridx(1);
        // C0->SetGridy(1);
        C0->SetBottomMargin(0.16);
        TH2D *dummy0 = new TH2D("dummy0", ";#mu;2 #Delta NLL", 10, 0., 2., 10, 0., 15.);
        dummy0->GetXaxis()->SetTitleSize(0.08);
        dummy0->GetXaxis()->SetTitleOffset(0.8);
        dummy0->GetYaxis()->SetTitleSize(0.06);
        dummy0->GetYaxis()->SetTitleOffset(0.6);
        dummy0->Draw();
        TLine* line1 = new TLine(0, 1, 2, 1);
        line1->SetLineStyle(2);
        line1->Draw();
        TLine* line1y = new TLine(muup, 0, muup, 1);
        line1y->SetLineStyle(2);
        line1y->Draw();
        TLine* line4 = new TLine(0, 4, 2, 4);
        line4->SetLineStyle(2);
        line4->Draw();
        TLine* line4y = new TLine(muup2, 0, muup2, 4);
        line4y->SetLineStyle(2);
        line4y->Draw();
        GR0->SetLineWidth(2);
        GR0->SetLineColor(8);
        GR0->Draw("curve same");
        TLatex *TEXT = new TLatex(1.5, 12., "WWW");
        TEXT->SetTextSize(0.07);
        TEXT->SetTextAlign(32);
        TEXT->SetTextFont(42);
        TEXT->DrawLatexNDC(0.8, 0.83, TString::Format("#mu = %6.3f_{%6.3f}^{+%6.3f} (68%% CL)", mucen, mudn, muup));
        TEXT->DrawLatexNDC(0.8, 0.71, TString::Format("#mu = %6.3f_{%6.3f}^{+%6.3f} (95%% CL)", mucen, mudn2, muup2));
        C0->Print("scan_mu0.pdf");
    }
    return muup2;
}

float RooUtil::StatUtil::cut_and_count_95percent_limit(Double_t S_, Double_t B_, Double_t BSyst_, Bool_t verbose)
{
    setVerbose(verbose);

    if (Verbose)
        cout << "\nPerform a global fit to single bin experiment...\n\n";

    setupData(S_, B_, BSyst_);

    if (Verbose)
    {
        printf("\nSummary of data:\n");
        printf("  total signal     = %7.3f\n", S);
        printf("  total background = %7.3f\n", B);
        printf("  observed         = %7.3f\n", N);
        cout << endl;
    }

    doFit();

    return doScanSingle();
}
