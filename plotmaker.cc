//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef plotmaker_cc
#define plotmaker_cc

#include "TCanvas.h"
#include "TColor.h"
#include "TPad.h"
#include "TH2.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH1F.h"
#include "THStack.h"
#include "TList.h"
#include "TCollection.h"
#include "TString.h"
#include "TLatex.h"
#include "TMath.h"
#include "TObjString.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TGaxis.h"

#include "RooStats/NumberCountingUtils.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <string>
#include <deque>

using namespace std;

typedef std::pair<TH1*, TH1*> Hist;
typedef std::vector<Hist> Hists;

// ========================
// Additional color options
// ========================
std::vector<TColor*> colors = {
new TColor(11005, 103 / 255., 0   / 255., 31  / 255.)
,new TColor(11004, 178 / 255., 24  / 255., 43  / 255.)
,new TColor(11003, 214 / 255., 96  / 255., 77  / 255.)
,new TColor(11002, 244 / 255., 165 / 255., 130 / 255.)
,new TColor(11001, 253 / 255., 219 / 255., 199 / 255.)
,new TColor(11000, 247 / 255., 247 / 255., 247 / 255.)
,new TColor(11011, 209 / 255., 229 / 255., 240 / 255.)
,new TColor(11012, 146 / 255., 197 / 255., 222 / 255.)
,new TColor(11013, 67  / 255., 147 / 255., 195 / 255.)
,new TColor(11014, 33  / 255., 102 / 255., 172 / 255.)
,new TColor(11015, 5   / 255., 48  / 255., 97  / 255.)

,new TColor(3001, 239 / 255., 138 / 255., 98  / 255.)
,new TColor(3000, 247 / 255., 247 / 255., 247 / 255.)
,new TColor(3011, 103 / 255., 169 / 255., 207 / 255.)

,new TColor(5001, 251 / 255., 180 / 255., 174 / 255.)
,new TColor(5002, 179 / 255., 205 / 255., 227 / 255.)
,new TColor(5003, 204 / 255., 235 / 255., 197 / 255.)
,new TColor(5004, 222 / 255., 203 / 255., 228 / 255.)
,new TColor(5005, 254 / 255., 217 / 255., 166 / 255.)

,new TColor(7000, 0   / 255., 0   / 255., 0   / 255.)
,new TColor(7001, 213 / 255., 94  / 255., 0   / 255.) // r
,new TColor(7002, 230 / 255., 159 / 255., 0   / 255.) // o
,new TColor(7003, 240 / 255., 228 / 255., 66  / 255.) // y
,new TColor(7004, 0   / 255., 158 / 255., 115 / 255.) // g
,new TColor(7005, 0   / 255., 114 / 255., 178 / 255.) // b
,new TColor(7006, 86  / 255., 180 / 255., 233 / 255.) // k
,new TColor(7007, 204 / 255., 121 / 255., 167 / 255.) // p
,new TColor(7011, 110 / 255., 54  / 255., 0   / 255.) // alt r
,new TColor(7012, 161 / 255., 117 / 255., 0   / 255.) // alt o
,new TColor(7013, 163 / 255., 155 / 255., 47  / 255.) // alt y
,new TColor(7014, 0   / 255., 102 / 255., 79  / 255.) // alt g
,new TColor(7015, 0   / 255., 93  / 255., 135 / 255.) // alt b
,new TColor(7016, 153 / 255., 153 / 255., 153 / 255.) // alt k
,new TColor(7017, 140 / 255., 93  / 255., 119 / 255.) // alt p

,new TColor(9001, 60  / 255., 186 / 255., 84  / 255.)
,new TColor(9002, 244 / 255., 194 / 255., 13  / 255.)
,new TColor(9003, 219 / 255., 50  / 255., 54  / 255.)
,new TColor(9004, 72  / 255., 133 / 255., 237 / 255.)

// Color schemes from Hannsjoerg for WWW analysis
,new TColor(2001, 91  / 255., 187 / 255., 241 / 255.) //light-blue
,new TColor(2002, 60  / 255., 144 / 255., 196 / 255.) //blue
,new TColor(2003, 230 / 255., 159 / 255., 0   / 255.) //orange
,new TColor(2004, 180 / 255., 117 / 255., 0   / 255.) //brown
,new TColor(2005, 245 / 255., 236 / 255., 69  / 255.) //yellow
,new TColor(2006, 215 / 255., 200 / 255., 0   / 255.) //dark yellow
,new TColor(2007, 70  / 255., 109 / 255., 171 / 255.) //blue-violet
,new TColor(2008, 70  / 255., 90  / 255., 134 / 255.) //violet
,new TColor(2009, 55  / 255., 65  / 255., 100 / 255.) //dark violet
,new TColor(2010, 120 / 255., 160 / 255., 0   / 255.) //light green
,new TColor(2011, 0   / 255., 158 / 255., 115 / 255.) //green
,new TColor(2012, 204 / 255., 121 / 255., 167 / 255.) //pink?
};

// =============
// global option
// =============
std::vector<TString> options_array;
std::map<TString, TString> options;

// ==================
// global th1 handler
// ==================
std::vector<TH1*> new_hists;

// ============================
// global TPad property handler
// ============================
std::map<TPad*, bool> map_pad_drawn;

//_________________________________________________________________________________________________
/* Convert double to string with specified number of places after the decimal. */
std::string prd( const double x, const int decDigits )
{
    stringstream ss;
    ss << fixed;
    ss.precision( decDigits ); // set # places after decimal
    ss << x;
    return ss.str();
}

//_________________________________________________________________________________________________
/* Convert double to string with specified number of places after the decimal
      and left padding. */
std::string prd( const double x, const int decDigits, const int width )
{
    stringstream ss;
    ss << fixed << right;
    ss.fill( ' ' );      // fill space around displayed #
    ss.width( width );   // set  width around displayed #
    ss.precision( decDigits ); // set # places after decimal
    ss << x;
    return ss.str();
}

//_________________________________________________________________________________________________
/*! Center-aligns string within a field of width w. Pads with blank spaces
      to enforce alignment. */
std::string center( const string s, const int w )
{
    stringstream ss, spaces;
    int padding = w - s.size();                 // count excess room to pad
    
    for ( int i = 0; i < padding / 2; ++i )
        spaces << " ";
        
    ss << spaces.str() << s << spaces.str();    // format with padding
    
    if ( padding > 0 && padding % 2 != 0 )      // if odd #, add 1 space
        ss << " ";
        
    return ss.str();
}

//_________________________________________________________________________________________________
//Parse Parameters from options input string
inline std::vector <TString> GetParms( std::string blah )
{
    int a = -1;
    int length = blah.length();
    std::vector <TString> options_;
    
    while ( a < length )
    {
        int tempn = a;
        a = blah.find( "--", tempn + 1 );
        
        if ( a <= tempn )
            break;
            
        int b = blah.find( "--", a + 3 ) - 1;
        unsigned int myLength = b - a - 2;
        std::string mySubstring;
        
        if ( a + 2 + myLength > blah.length() )
            mySubstring = blah.substr( a + 2 );
        else
            mySubstring = blah.substr( a + 2, b - a - 2 );
            
        options_.push_back( TString( mySubstring ) );
    }
    
    return options_;
}

//_________________________________________________________________________________________________
double getNonZeroMininimum(TH1* h)
{
    float nonzeromin = -999;
    for (Int_t ibin = 1; ibin <= h->GetNbinsX(); ++ibin)
    {
        float binc = h->GetBinContent(ibin);
        if ( (binc < nonzeromin && binc > 0) || ( nonzeromin == -999 ) )
            nonzeromin = binc;
    }
    return nonzeromin;
}

//_________________________________________________________________________________________________
TString getDefaultOpt(TString key, TString default_val)
{
    return options[key].IsNull() ? default_val : options[key];
}

//_________________________________________________________________________________________________
TString getOpt( TString key )
{

    // General rule of thumb on the option names
    // 0. If the option does not relate anything to ROOT, anything's fair game.
    //    But please be mindful of options name being not to obscure.
    // 1. Drop "Set" and see whether that is enough.
    // 2. If another object is called before calling "Set", place a lower case indicator in front.
    // 3. If following 1. and 2. there is a duplicate option name, (e.g. Ratio pane has same Maximum and Minimum)
    //    then, place a "category" name of user's choosing (e.g. "ratio", "error", etc.)
    //    and place a "_" delimiter.

    TString gFont = "42";
    TString gFontSize = "0.063";
    TString gLabelOffset = "0.0225";
    TString gTickLength = "-0.02";
    TString gNdiv = "505";

    if      ( key.EqualTo( "xTitle"                    )  ) return getDefaultOpt( key, "XVar"             ) ;
    else if ( key.EqualTo( "xTickLength"               )  ) return getDefaultOpt( key, gTickLength        ) ;
    else if ( key.EqualTo( "xTitleOffset"              )  ) return getDefaultOpt( key, "1.2"              ) ;
    else if ( key.EqualTo( "xLabelOffset"              )  ) return getDefaultOpt( key, gLabelOffset       ) ;
    else if ( key.EqualTo( "xTitleSize"                )  ) return getDefaultOpt( key, gFontSize          ) ;
    else if ( key.EqualTo( "xLabelSize"                )  ) return getDefaultOpt( key, gFontSize          ) ;
    else if ( key.EqualTo( "xTitleFont"                )  ) return getDefaultOpt( key, gFont              ) ;
    else if ( key.EqualTo( "xLabelFont"                )  ) return getDefaultOpt( key, gFont              ) ;
    else if ( key.EqualTo( "xNdivisions"               )  ) return getDefaultOpt( key, gNdiv              ) ;
    else if ( key.EqualTo( "xNbin"                     )  ) return getDefaultOpt( key, ""                 ) ;

    else if ( key.EqualTo( "yTitle"                    )  ) return getDefaultOpt( key, "YVar"             ) ;
    else if ( key.EqualTo( "yTickLength"               )  ) return getDefaultOpt( key, gTickLength        ) ;
    else if ( key.EqualTo( "yTitleOffset"              )  ) return getDefaultOpt( key, "2.1"              ) ;
    else if ( key.EqualTo( "yLabelOffset"              )  ) return getDefaultOpt( key, gLabelOffset       ) ;
    else if ( key.EqualTo( "yTitleSize"                )  ) return getDefaultOpt( key, gFontSize          ) ;
    else if ( key.EqualTo( "yLabelSize"                )  ) return getDefaultOpt( key, gFontSize          ) ;
    else if ( key.EqualTo( "yTitleFont"                )  ) return getDefaultOpt( key, gFont              ) ;
    else if ( key.EqualTo( "yLabelFont"                )  ) return getDefaultOpt( key, gFont              ) ;
    else if ( key.EqualTo( "yNdivisions"               )  ) return getDefaultOpt( key, gNdiv              ) ;

    else if ( key.EqualTo( "MaximumMultiplier"         )  ) return getDefaultOpt( key, "1.2"              ) ;

    else if ( key.EqualTo( "Minimum"                   )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "Maximum"                   )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "MinimumLogy"               )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "MaximumLogy"               )  ) return getDefaultOpt( key, ""                 ) ;

    else if ( key.EqualTo( "error_FillColor"           )  ) return getDefaultOpt( key, "1"                ) ;
    else if ( key.EqualTo( "error_FillStyle"           )  ) return getDefaultOpt( key, "3245"             ) ;

    else if ( key.EqualTo( "ratio_xTitle"              )  ) return getDefaultOpt( key, getOpt( "xTitle" ) ) ;
    else if ( key.EqualTo( "ratio_yTitle"              )  ) return getDefaultOpt( key, getOpt( "reverseRatio" ).IsNull() ? "Data / MC" : "MC / Data" ) ;
    else if ( key.EqualTo( "ratio_Minimum"             )  ) return getDefaultOpt( key, "0.7"              ) ;
    else if ( key.EqualTo( "ratio_Maximum"             )  ) return getDefaultOpt( key, "1.3"              ) ;
    else if ( key.EqualTo( "ratio_yNdivisions"         )  ) return getDefaultOpt( key, gNdiv              ) ;
    else if ( key.EqualTo( "ratio_DrawOpt"             )  ) return getDefaultOpt( key, "ex0p"             ) ;

    else if ( key.EqualTo( "data_DrawOpt"              )  ) return getDefaultOpt( key, "ex0p"             ) ;

    else if ( key.EqualTo( "stack_DrawOpt"             )  ) return getDefaultOpt( key, "hist"             ) ;

    else if ( key.EqualTo( "legend_bkgDrawOpt"         )  ) return getDefaultOpt( key, "f"                ) ;
    else if ( key.EqualTo( "legend_NColumns"           )  ) return getDefaultOpt( key, "1"                ) ;
    else if ( key.EqualTo( "legend_EntrySeparation"    )  ) return getDefaultOpt( key, "0.02"             ) ;
    else if ( key.EqualTo( "legend_ColumnSeparation"   )  ) return getDefaultOpt( key, "0.0"              ) ;
    else if ( key.EqualTo( "legend_Margin"             )  ) return getDefaultOpt( key, "0.300"            ) ;

    else if ( key.EqualTo( "plotOutputName"            )  ) return getDefaultOpt( key, "test"             ) ;
    else if ( key.EqualTo( "autoStack"                 )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "showOverflow"              )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "showUnderflow"             )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printRatio"                )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printBkg"                  )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printTotalBkg"             )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printData"                 )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "sumDataHists"              )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "sumSigHists"               )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "reverseRatio"              )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "ratioPaneAtBottom"         )  ) return getDefaultOpt( key, "true"             ) ;
    else if ( key.EqualTo( "divideByBinWidth"          )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "scaleByLumi"               )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printYieldsTable"          )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printYieldsMinBin"         )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printYieldsMaxBin"         )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "noData"                    )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "noSyst"                    )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "systByDiff"                )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "onlyLog"                   )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "onlyLin"                   )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "legendOnMainPad"           )  ) return getDefaultOpt( key, "true"             ) ;
    else if ( key.EqualTo( "saveMainPad"               )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "MaxDigits"                 )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "getMaxFromData"            )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "publogo"                   )  ) return getDefaultOpt( key, "Supplementary"    ) ;
    else if ( key.EqualTo( "userhack0"                 )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "totalBkgFlatSyst"          )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printObsSignif"            )  ) return getDefaultOpt( key, ""                 ) ;
    else if ( key.EqualTo( "printExpSignif"            )  ) return getDefaultOpt( key, ""                 ) ;

    else
    {
        std::cout << "[plotmaker::] Unrecognized option! option = " << key << std::endl;
        exit(-1);
    }
}

//_________________________________________________________________________________________________
std::vector<double> getBinInfo( std::vector<TH1*> hists )
{
    std::vector<double> bininfo;
    bininfo.push_back( hists[0]->GetNbinsX() );
    bininfo.push_back( hists[0]->GetXaxis()->GetXmin() );
    bininfo.push_back( hists[0]->GetXaxis()->GetXmax() );
    return bininfo;
}

//_________________________________________________________________________________________________
void setNormSyst( TH2* hist, double fracsyst )
{
    hist->SetBinContent( 0, 0, fracsyst );
}

//_________________________________________________________________________________________________
double getNormSyst( TH2* hist )
{
    return hist->GetBinContent( 0, 0 );
}

//_________________________________________________________________________________________________
void showOverflow( TH1* hist )
{
    double overflow_content = hist->GetBinContent( hist->GetNbinsX() + 1 );
    double overflow_error   = hist->GetBinError  ( hist->GetNbinsX() + 1 );
    double lastbin_content = hist->GetBinContent( hist->GetNbinsX()  );
    double lastbin_error   = hist->GetBinError  ( hist->GetNbinsX()  );
    hist->SetBinContent( hist->GetNbinsX(), overflow_content + lastbin_content );
    hist->SetBinError  ( hist->GetNbinsX(), sqrt( pow( overflow_error, 2 ) + pow( lastbin_error, 2 ) ) );
}

//_________________________________________________________________________________________________
void showUnderflow( TH1* hist )
{
    double underflow_content = hist->GetBinContent( 0 );
    double underflow_error   = hist->GetBinError  ( 0 );
    double firstbin_content = hist->GetBinContent( 1  );
    double firstbin_error   = hist->GetBinError  ( 1  );
    hist->SetBinContent( hist->GetNbinsX(), underflow_content + firstbin_content );
    hist->SetBinError  ( hist->GetNbinsX(), sqrt( pow( underflow_error, 2 ) + pow( firstbin_error, 2 ) ) );
}

//_________________________________________________________________________________________________
void correctOverUnderflow( TH1* hist )
{
    if ( !getOpt( "showOverflow" ).IsNull() ) showOverflow( hist );
    if ( !getOpt( "showUnderflow" ).IsNull() ) showUnderflow( hist );
}

//_________________________________________________________________________________________________
void rebin( TH1* hist )
{
    if ( !getOpt( "xNbin" ).IsNull() )
    {
        int target_nbin = getOpt( "xNbin" ).Atoi();
        int current_nbin = hist->GetNbinsX();
        if ( current_nbin % target_nbin != 0 )
        {
            std::cout << "[plotmaker::] The target bin you asked target_nbin=";
            std::cout << target_nbin << " ";
            std::cout << "is not possible for a bin with current_nbin=";
            std::cout << current_nbin << std::endl;
            exit(-1);
        }
        int rebin_factor = current_nbin / target_nbin;
        hist->Rebin( rebin_factor );
    }
}

//_________________________________________________________________________________________________
void divideByBinWidth( TH1* hist )
{
    if ( !getOpt( "divideByBinWidth" ).IsNull() )
    {
        for ( int ibin = 1; ibin <= hist->GetNbinsX(); ibin++ )
        {
            hist->SetBinContent( ibin, hist->GetBinContent( ibin ) / hist->GetBinWidth( ibin ) );
            hist->SetBinError( ibin, hist->GetBinError( ibin ) / hist->GetBinWidth( ibin ) );
        }
    }
}

//_________________________________________________________________________________________________
TH1* cloneHist( TH1* obj, TString name="" )
{
    if ( name.IsNull() )
        name = obj->GetName();
    TH1* rtn = ( TH1* ) obj->Clone( name );
    rtn->SetTitle( "" );

    if ( !rtn->GetSumw2N() )
        rtn->Sumw2();

    rtn->SetDirectory( 0 );
    return rtn;
}

//_________________________________________________________________________________________________
TH1* histWithFullError( TH1* nominal, TH1* error )
{
    TH1D* nominal_with_full_error = ( TH1D* ) nominal->Clone( nominal->GetName() );
    nominal_with_full_error->SetTitle("");
    
    if ( !nominal_with_full_error->GetSumw2N() )
        nominal_with_full_error->Sumw2();
        
    nominal_with_full_error->SetDirectory( 0 );
    
    for ( int ibin = 0; ibin <= nominal->GetNbinsX() + 1; ibin++ )
    {
        if ( TString( nominal->GetTitle() ).Contains( "counter" ) && ibin == nominal->GetNbinsX() + 1 )
            continue;
        double content = nominal->GetBinContent( ibin );
        double nominal_hist_error = nominal->GetBinError( ibin );
        double additional_error = error ? error->GetBinContent( ibin ) : 0;
        if ( !getOpt( "systByDiff" ).IsNull() )
            additional_error = error ? fabs( additional_error - content ) : 0;
        if ( !getOpt( "noSyst" ).IsNull() )
            additional_error = 0;
        double all_error = error ? sqrt( pow( nominal_hist_error, 2 ) + pow( additional_error, 2 ) )
                           : nominal_hist_error;
        nominal_with_full_error->SetBinContent( ibin, content );
        nominal_with_full_error->SetBinError( ibin, all_error );
    }
    
    correctOverUnderflow( nominal_with_full_error );
    rebin( nominal_with_full_error );
    divideByBinWidth( nominal_with_full_error );
    
    return nominal_with_full_error;
}

//_________________________________________________________________________________________________
TH1* hist2DWithFullError( TH1* nominal, TH1* error )
{
    TH2D* nominal_with_full_error = ( TH2D* ) nominal->Clone( nominal->GetName() );
    nominal_with_full_error->SetTitle("");
    
    if ( !nominal_with_full_error->GetSumw2N() )
        nominal_with_full_error->Sumw2();
        
    nominal_with_full_error->SetDirectory( 0 );
    
    for ( int ibin = 0; ibin <= nominal->GetNbinsX() + 1; ibin++ )
    {
        for ( int jbin = 0; jbin <= nominal->GetNbinsY() + 1; jbin++ )
        {
            double content = nominal->GetBinContent( ibin, jbin );
            double nominal_hist_error = nominal->GetBinError( ibin, jbin );
            double additional_error = error ? error->GetBinContent( ibin, jbin ) : 0;
            if ( !getOpt( "systByDiff" ).IsNull() )
                additional_error = error ? fabs( additional_error - content ) : 0;
            double all_error = error ? sqrt( pow( nominal_hist_error, 2 ) + pow( additional_error, 2 ) )
                : nominal_hist_error;
            nominal_with_full_error->SetBinContent( ibin, jbin, content );
            nominal_with_full_error->SetBinError( ibin, jbin, all_error );
        }
    }
    
    return nominal_with_full_error;
}

//_________________________________________________________________________________________________
TH1* histWithFullError( Hist& hist_pair )
{
    return histWithFullError( hist_pair.first, hist_pair.second );
}

//_________________________________________________________________________________________________
std::vector<TH1*> histsWithFullError( Hists& hist_pairs )
{
    std::vector<TH1*> hists;
    for ( auto& hist_pair : hist_pairs )
    {
        hists.push_back( histWithFullError( hist_pair ) );
    }
    return hists;
}

//_________________________________________________________________________________________________
TH1* getSystByMaxDiff( TH1* nominal, std::vector<TH1*> systs, double normfracsyst=0 )
{
    TH1D* totalsyst = ( TH1D* ) cloneHist( nominal );
    TH1D* diff = ( TH1D* ) cloneHist( nominal );
    diff->Reset();

    for ( unsigned int isyst = 0; isyst < systs.size(); ++isyst )
    {
        TH1* thissyst = systs[isyst];
        for ( int ibin = 1; ibin <= thissyst->GetNbinsX(); ++ibin )
        {
            double systcontent = thissyst->GetBinContent( ibin );
            double nomicontent = nominal->GetBinContent( ibin );
            double thisdiff = fabs( systcontent - nomicontent );
            double currmaxdiff = diff->GetBinContent( ibin );
            double normsyst = normfracsyst > 0 ? nomicontent * normfracsyst : 0.;
            thisdiff = sqrt( pow( thisdiff, 2 ) + pow( normsyst, 2 ) );
            if ( currmaxdiff < thisdiff )
            {
                diff->SetBinContent( ibin, thisdiff );
                totalsyst->SetBinContent( ibin, nomicontent + thisdiff );
            }
        }
    }
//    return diff;
    return totalsyst;
}

//_________________________________________________________________________________________________
TH1* getSyst2DByMaxDiff( TH1* nominal, std::vector<TH1*> systs )
{
    TH2D* totalsyst = ( TH2D* ) cloneHist( nominal );
    TH2D* diff = ( TH2D* ) cloneHist( nominal );
    diff->Reset();

    for ( unsigned int isyst = 0; isyst < systs.size(); ++isyst )
    {
        TH1* thissyst = systs[isyst];
        for ( int ibin = 1; ibin <= thissyst->GetNbinsX(); ++ibin )
        {
            for ( int jbin = 1; jbin <= thissyst->GetNbinsY(); ++jbin )
            {
                double systcontent = thissyst->GetBinContent( ibin, jbin );
                double nomicontent = nominal->GetBinContent( ibin, jbin );
                double thisdiff = fabs( systcontent - nomicontent );
                double currmaxdiff = diff->GetBinContent( ibin, jbin );
                if ( currmaxdiff < thisdiff )
                {
                    diff->SetBinContent( ibin, jbin, thisdiff );
                    totalsyst->SetBinContent( ibin, jbin, nomicontent + thisdiff );
                }
            }
        }
    }
    return totalsyst;
}

//_________________________________________________________________________________________________
TH1* getSumHists( std::vector<TH1*> hists )
{
    TH1D* sum_hist = (TH1D*) hists[0]->Clone();
    if ( !sum_hist->GetSumw2N() )
        sum_hist->Sumw2();
    sum_hist->SetDirectory( 0 );
    sum_hist->Reset();

    for ( auto& hist : hists )
        sum_hist->Add( hist );

    return sum_hist;
}

//_________________________________________________________________________________________________
TH1* getTotalBkgHists( std::vector<TH1*> hists )
{
    if ( hists.size() == 0)
    {
        std::cout << "[plotmaker::] You asked for a total bkg hist when there are no bkg hists provided." << std::endl;
        exit(-1);
    }
    TH1* sum_hist = getSumHists( hists );
    sum_hist->SetName("Total Bkg.");
    if ( !gROOT->GetColor( 9999 ) )
        new TColor( 9999, 0.1, 0.2, 0.3, "", 0. ); // alpha = 0.5
    sum_hist->SetMarkerStyle( 1 );
    sum_hist->SetMarkerSize( 0 );
    sum_hist->SetMarkerColor( 9999 );
    sum_hist->SetLineColor( 1 );
    sum_hist->SetLineColor( 1 );
    sum_hist->SetFillColor( getOpt( "error_FillColor" ).Atoi() );
    sum_hist->SetFillStyle( getOpt( "error_FillStyle" ).Atoi() );

    if ( !getOpt( "printTotalBkg" ).IsNull() )
    {
        std::cout << "[plotmaker::] Printing total background histogram." << std::endl;
        sum_hist->Print("all");
    }

    if ( !getOpt( "totalBkgFlatSyst" ).IsNull() )
    {
        float frac = getOpt( "totalBkgFlatSyst" ).Atof();
        for ( int ibin = 0; ibin <= sum_hist->GetNbinsX() + 1; ++ibin )
        {
            float binc = sum_hist->GetBinContent( ibin );
            float bine = sum_hist->GetBinError( ibin );
            bine = sqrt( bine * bine + frac * binc * frac * binc );
            sum_hist->SetBinError( ibin, bine );
        }
    }

    return sum_hist;
}

//_________________________________________________________________________________________________
THStack* getStack( std::vector<TH1*> hists )
{
    THStack *stack = new THStack();
    stack->SetName( "stack" );
    stack->SetTitle( "" );

    for ( auto& hist : hists )
    {
        stack->Add( hist, "" );
        if ( !getOpt( "printBkg" ).IsNull() )
            hist->Print( "all" );
    }

    //
    // Stupid ROOT THStack not reflecting maximum and minimum of pointer TH1* from "GetHistogram()" 
    // So I have to add this here instead of "stylizeAxes"
    //
    if ( !getOpt( "MaximumMultiplier" ).IsNull() )
        stack->SetMaximum( stack->GetMaximum() * getOpt( "MaximumMultiplier" ).Atof() * 1.176 );
    if ( !getOpt( "Minimum" ).IsNull() ) stack->SetMinimum( getOpt( "Minimum" ).Atof() );
    if ( !getOpt( "Maximum" ).IsNull() ) stack->SetMaximum( getOpt( "Maximum" ).Atof() );

    return stack;
}

//_________________________________________________________________________________________________
void stylizeAxes( TH1* h, TPad* pad )
{
//    double pad_h = pad->GetAbsHNDC();
//    double pad_w = pad->GetAbsWNDC();
//
//    double WidthPixel =  pad->GetWw();

    if ( !getOpt( "MaximumMultiplier" ).IsNull() )
        h->SetMaximum( h->GetMaximum() * getOpt( "MaximumMultiplier" ).Atof() );
    if ( !getOpt( "Minimum" ).IsNull() ) h->SetMinimum( getOpt( "Minimum" ).Atof() );
    if ( !getOpt( "Maximum" ).IsNull() ) h->SetMaximum( getOpt( "Maximum" ).Atof() );
    h->SetDirectory( 0 );
    h->SetStats( 0 );
    h->SetFillColor( -1 );
    h->GetXaxis ( ) ->SetNdivisions  ( getOpt( "xNdivisions"  ) .Atoi( )   ) ;
    h->GetXaxis ( ) ->SetLabelFont   ( getOpt( "xLabelFont"   ) .Atoi( )   ) ;
    h->GetXaxis ( ) ->SetLabelOffset ( getOpt( "xLabelOffset" ) .Atof( )   ) ;
    h->GetXaxis ( ) ->SetLabelSize   ( getOpt( "xLabelSize"   ) .Atof( )   ) ;
    h->GetXaxis ( ) ->SetTitleSize   ( getOpt( "xTitleSize"   ) .Atof( )   ) ;
    h->GetXaxis ( ) ->SetTitle       ( getOpt( "xTitle"       )            ) ;
    h->GetXaxis ( ) ->SetTickLength  ( getOpt( "xTickLength"  ) .Atof( ) * 1.15 ) ;
    h->GetXaxis ( ) ->SetTitleOffset ( getOpt( "xTitleOffset" ) .Atof( )   ) ;
    h->GetXaxis ( ) ->SetTitleFont   ( getOpt( "xLabelFont"   ) .Atoi( )   ) ;
    h->GetYaxis ( ) ->SetNdivisions  ( getOpt( "yNdivisions"  ) .Atoi( )   ) ;
    h->GetYaxis ( ) ->SetLabelFont   ( getOpt( "yLabelFont"   ) .Atoi( )   ) ;
    h->GetYaxis ( ) ->SetLabelOffset ( getOpt( "yLabelOffset" ) .Atof( )   ) ;
    h->GetYaxis ( ) ->SetLabelSize   ( getOpt( "yLabelSize"   ) .Atof( )   ) ;
    h->GetYaxis ( ) ->SetTitleSize   ( getOpt( "yTitleSize"   ) .Atof( )   ) ;
    h->GetYaxis ( ) ->SetTitle       ( getOpt( "yTitle"       )            ) ;
    h->GetYaxis ( ) ->SetTickLength  ( getOpt( "yTickLength"  ) .Atof( ) * 1.15 ) ;
    h->GetYaxis ( ) ->SetTitleOffset ( getOpt( "yTitleOffset" ) .Atof( )   ) ;
    h->GetYaxis ( ) ->SetTitleFont   ( getOpt( "xLabelFont"   ) .Atoi( )   ) ;
//    // TODO: Come back below when we need to implement it
//    h->GetZaxis ( )->SetLabelFont   ( getOpt( "zLabelFont" ).Atoi() );
//    h->GetZaxis ( )->SetLabelSize   ( 0.035 );
//    h->GetZaxis ( )->SetTitleSize   ( 0.035 );
//    h->GetZaxis ( )->SetTitleFont   ( getOpt( "zTitleFont" ).Atoi() );
}

//_________________________________________________________________________________________________
void stylizeRatioAxes( TH1* h, TPad* pad )
{
//    double pad_h = pad->GetAbsHNDC();
//    double pad_w = pad->GetAbsWNDC();
//
//    double WidthPixel =  pad->GetWw();

    float sf = 0.7 / 0.3;

    h->SetTitle("");
    h->SetMinimum( getOpt( "ratio_Minimum" ).Atof() );
    h->SetMaximum( getOpt( "ratio_Maximum" ).Atof() );
    h->SetMarkerStyle( 19 );
    h->GetXaxis( ) ->SetNdivisions( getOpt( "xNdivisions" ).Atoi( ) );
    h->GetXaxis( ) ->SetLabelFont( getOpt( "xLabelFont" ) .Atoi( ) ) ;
    h->GetXaxis( ) ->SetLabelOffset( getOpt( "xLabelOffset" ).Atof() * sf ) ;
    h->GetXaxis( ) ->SetLabelSize( getOpt( "xLabelSize" ).Atof() * sf ) ;
    h->GetXaxis( ) ->SetTitleSize( getOpt( "xLabelSize" ).Atof() * sf ) ;
    h->GetXaxis( ) ->SetTickLength( getOpt( "xTickLength" ).Atof() * sf ) ;
    h->GetXaxis( ) ->SetTitleOffset( getOpt( "xTitleOffset" ).Atof() ) ;
    h->GetXaxis( ) ->SetTitleFont( getOpt( "xLabelFont" ).Atoi( ) );
    h->GetXaxis( ) ->SetTitle( getOpt( "ratio_xTitle" ) );
    h->GetYaxis( ) ->SetNdivisions( getOpt( "ratio_yNdivisions" ).Atoi( ) );
    h->GetYaxis( ) ->SetLabelFont( getOpt( "yLabelFont" ).Atoi( ) );
    h->GetYaxis( ) ->SetLabelOffset( getOpt( "yLabelOffset" ).Atof( ) );
    h->GetYaxis( ) ->SetLabelSize( getOpt( "yLabelSize" ).Atof() * sf ) ;
    h->GetYaxis( ) ->SetTitleSize( getOpt( "yTitleSize" ).Atof() * sf ) ;
    h->GetYaxis( ) ->SetTickLength( getOpt( "yTickLength" ).Atof() * ( 10. / 6. ) ) ;
    h->GetYaxis( ) ->SetTitleOffset( getOpt( "yTitleOffset" ).Atof() / sf ) ;
    h->GetYaxis( ) ->SetTitleFont( getOpt( "yTitleFont" ).Atoi() );
    h->GetYaxis( ) ->SetTitle( getOpt( "ratio_yTitle" ) );
//    // TODO: When we need to we'll implement this
//    h->GetZaxis( ) ->SetLabelFont( 42 ) ;
//    h->GetZaxis( ) ->SetLabelSize( 0.035 ) ;
//    h->GetZaxis( ) ->SetTitleSize( 0.035 ) ;
//    h->GetZaxis( ) ->SetTitleFont( 42 ) ;

}

//_________________________________________________________________________________________________
void draw( TH1* h, TString option, TPad* pad )
{
    pad->cd();

    if ( !map_pad_drawn[pad] )
    {
        h->Draw( option.Data() );
        stylizeAxes( h, pad );
        map_pad_drawn[pad] = true;
    }
    else
    {
        h->Draw( (option + "same").Data() );
    }
}

//_________________________________________________________________________________________________
void draw( THStack* h, TString option, TPad* pad )
{
    pad->cd();

    if ( !map_pad_drawn[pad] )
    {
        h->Draw( option.Data() );
        stylizeAxes( h->GetHistogram(), pad );
        map_pad_drawn[pad] = true;
    }
    else
    {
        h->Draw( (option + "same").Data() );
    }
}

//_________________________________________________________________________________________________
void drawData( TH1* h, TString option, TPad* pad )
{
    h->SetMarkerStyle(19);
    h->SetMarkerSize(1.2);
//    h->SetLineColor(1);

    if ( !getOpt( "printData" ).IsNull() )
        h->Print( "all" );

    pad->cd();

    if ( !map_pad_drawn[pad] )
    {
        h->Draw( option.Data() );
        stylizeAxes( h, pad );
        map_pad_drawn[pad] = true;
    }
    else
    {
        h->Draw( (option + "same").Data() );
    }
}

//_________________________________________________________________________________________________
void drawRatio( TH1* h, TString option, TPad* pad )
{
    h->SetMarkerStyle(19);
    h->SetMarkerSize(1.2);
//    h->SetLineColor(1);

    pad->cd();

    if ( !getOpt( "printRatio" ).IsNull() )
        h->Print("all");

    if ( !map_pad_drawn[pad] )
    {
        h->Draw( option.Data() );
        stylizeRatioAxes( h, pad );
        map_pad_drawn[pad] = true;
    }
    else
    {
        h->Draw( (option + "same").Data() );
    }
}

//_________________________________________________________________________________________________
void addToLegend( TH1* h, TLegend* legend, const char* option )
{
    TString name = h->GetName();
    if ( !name.Contains( "skip" ) )
        legend->AddEntry( h, name.Data(), option );
}

//_________________________________________________________________________________________________
std::vector<TH1*> getRatioHists( std::vector<TH1*> data_hists, std::vector<TH1*> bkg_hists )
{
    std::vector<TH1*> ratio_hists;
    
    for ( unsigned int ihist = 0; ihist < data_hists.size(); ++ihist )
    {
        TH1D* ratio = ( TH1D* ) data_hists[ihist]->Clone( Form( "ratio%d", ihist ) );
        
        if ( !ratio->GetSumw2N() )
            ratio->Sumw2();
            
        ratio->SetDirectory( 0 );
        
        if ( !getOpt( "reverseRatio" ).IsNull() )
            ratio->Divide( bkg_hists.size() == 1 ? bkg_hists[0] : bkg_hists[ihist], data_hists[ihist] );
        else
            ratio->Divide( data_hists[ihist], bkg_hists.size() == 1 ? bkg_hists[0] : bkg_hists[ihist] );

        ratio_hists.push_back( ratio );
    }
    
    return ratio_hists;
}

//_________________________________________________________________________________________________
std::vector<TH1*> getRatioHists( std::vector<TH1*> data_hists, TH1* bkg_hist )
{
    std::vector<TH1*> bkg_hists;
    bkg_hists.push_back( bkg_hist );

    return getRatioHists( data_hists, bkg_hists );
}

//_________________________________________________________________________________________________
std::vector<TH1*> getSignifHists( std::vector<TH1*> data_hists, std::vector<TH1*> bkg_hists, bool doExp=false )
{
    std::vector<TH1*> signif_hists;
    
    for ( unsigned int ihist = 0; ihist < data_hists.size(); ++ihist )
    {
        TH1* signif = cloneHist( data_hists[ihist], Form( "signif%d", ihist ) );
        signif->Reset();

        THashList* labels = signif->GetXaxis()->GetLabels();
        if (labels)
            signif->GetXaxis()->SetRange(1, signif->GetXaxis()->GetNbins() ); 
        signif->GetXaxis()->SetCanExtend( false );

        for ( int ibin = 0; ibin <= signif->GetNbinsX() + 1; ++ibin )
        {
            double obs = data_hists[ihist]->GetBinContent( ibin );
            double bkg = bkg_hists.size() == 1 ? bkg_hists[0]->GetBinContent( ibin ) : bkg_hists[ihist]->GetBinContent( ibin );
            double err = bkg_hists.size() == 1 ? bkg_hists[0]->GetBinError( ibin ) : bkg_hists[ihist]->GetBinError( ibin );
            double fer = err / bkg;
            if ( doExp )
                signif->SetBinContent( ibin, RooStats::NumberCountingUtils::BinomialExpZ( obs, bkg, fer ) );
            else
                signif->SetBinContent( ibin, RooStats::NumberCountingUtils::BinomialObsZ( obs, bkg, fer ) );
        }
        
        signif_hists.push_back( signif );
    }
    
    return signif_hists;
}

//_________________________________________________________________________________________________
std::vector<TH1*> getSignifHists( std::vector<TH1*> data_hists, TH1* bkg_hist, bool doExp=false )
{
    std::vector<TH1*> bkg_hists;
    bkg_hists.push_back( bkg_hist );

    return getSignifHists( data_hists, bkg_hists, doExp );
}

//_________________________________________________________________________________________________
void drawLegend( std::vector<TH1*> data_hists, std::vector<TH1*> bkg_hists, std::vector<TH1*> sig_hists, TPad* pad )
{
    float xmin = getOpt( "legendOnMainPad" ).IsNull() ? 0    : 0.605;
    float xmax = getOpt( "legendOnMainPad" ).IsNull() ? 1    : 0.92;
    float ymin = getOpt( "legendOnMainPad" ).IsNull() ? 0    : 0.75;
    float ymax = getOpt( "legendOnMainPad" ).IsNull() ? 0.65 : 0.91;
    TLegend* leg = new TLegend( xmin, ymin, xmax, ymax, NULL, "brNDC" );
    leg->SetBorderSize( 0 );
    leg->SetLineColor( 0 );
    leg->SetLineStyle( 1 );
    leg->SetLineWidth( 1 );
    leg->SetFillColor( 0 );
    leg->SetFillStyle( 0 );
    leg->SetMargin( 0.13 );
    leg->SetEntrySeparation( 0.065 );
    if ( !getOpt( "legendOnMainPad" ).IsNull() )
        leg->SetEntrySeparation( 0.033 );
    leg->SetTextSize( 0.062 * 6. / 4. );
    if ( !getOpt( "legendOnMainPad" ).IsNull() )
        leg->SetTextSize( 0.034 );
    
    if ( getOpt( "noData" ).IsNull() )
    {
        for ( auto& data_hist : data_hists )
            addToLegend( data_hist, leg, "ep" );
    }
        
    std::reverse( std::begin( bkg_hists ), std::end( bkg_hists ) );
    for ( auto& bkg_hist : bkg_hists )
        addToLegend( bkg_hist, leg, getOpt( "legend_bkgDrawOpt" ) );
        
    for ( auto& sig_hist : sig_hists )
        addToLegend( sig_hist, leg, "l" );

    int totalentries = leg->GetNRows();
    leg->SetNColumns( getOpt( "legend_NColumns" ).Atoi() );
    leg->SetEntrySeparation( getOpt( "legend_EntrySeparation" ).Atof() );
    leg->SetColumnSeparation( getOpt( "legend_ColumnSeparation" ).Atof() );
    leg->SetMargin( getOpt( "legend_Margin" ).Atof() );
        
    int entries = leg->GetNRows();
    if ( getOpt( "legendOnMainPad" ).IsNull() )
        leg->SetY1( 0.65 - leg->GetEntrySeparation() * entries );
    else
        leg->SetY1( 0.75 - leg->GetEntrySeparation() * entries );

    if ( totalentries > 8 )
    {
        leg->SetNColumns( 3 );
        leg->SetX1( 0.405 );
    }
    if ( totalentries > 4 )
    {
        leg->SetNColumns( 2 );
        leg->SetX1( 0.405 );
    }
    pad->cd();

    if ( !getOpt( "userhack0" ).IsNull() )
    {
        TList* list = leg->GetListOfPrimitives();
        TLegendEntry* entry = 0;
        TIter next( list );
        for ( Int_t i = 0; i < list->GetSize(); i++ )
            entry = ( TLegendEntry * ) next();

        ((TH1D*) entry->GetObject())->SetFillStyle( 3245 );
        ((TH1D*) entry->GetObject())->SetFillColor( 1 );
    }

    leg->Draw();

}

//_________________________________________________________________________________________________
void drawLogos( TPad* pad )
{
    pad->cd();
    TLatex* texlumi = new TLatex( 0.580, 0.93, "35.9 fb^{-1} (13 TeV)" );
    texlumi->SetNDC();
    texlumi->SetTextFont( 42 );
    texlumi->SetTextSize( 0.049 );
    texlumi->SetLineWidth( 2 );
    texlumi->Draw();
    //TLatex* texcom = new TLatex( 0.265, 0.77, "13 TeV" );
    //texcom->SetNDC();
    //texcom->SetTextFont( 42 );
    //texcom->SetTextSize( 0.040 );
    //texcom->SetLineWidth( 2 );
    //texcom->Draw();
    TLatex* texcms = new TLatex( 0.265, 0.845, "CMS" );
    texcms->SetNDC();
    texcms->SetTextFont( 62 );
    texcms->SetTextSize( 0.060 );
    texcms->SetLineWidth( 2 );
    texcms->Draw();
    TLatex* texpub = new TLatex( 0.265, 0.81, getOpt( "publogo" ) );
    texpub->SetNDC();
    texpub->SetTextFont( 52 );
//    float xsize = texpub->GetXsize();
    texpub->SetTextSize( 0.049 * 0.5 / 1.335 );
    texpub->SetLineWidth( 2 );
    texpub->Draw();
    pad->Modified();
}

//_________________________________________________________________________________________________
void parseOptions( std::string options_string )
{
    options_array.clear();
    options.clear();
    options_array = GetParms( options_string );
    for ( auto& option : options_array )
    {
        TObjArray* oa = option.Tokenize(" ");
        if ( oa->GetEntries() == 1 )
        {
            TString key = ((TObjString*) oa->At(0))->GetString();
            key.ReplaceAll("\n","");
//            std::cout << ":" << key << ":" << "true" << ":" << std::endl; // for debugging
            options[key] = "true";
        }
        else
        {
            TString so = option.Strip(TString::kBoth);
            Ssiz_t delim_pos = so.First(" ");
            TString key = so(0, delim_pos);
            TString val = so(delim_pos+1, so.Length());
            key.ReplaceAll("\n","");
            val.ReplaceAll("\n","");
//            std::cout << ":" << key << ":" << val << ":" << std::endl; // for debugging
            options[key] = val;
        }
    }

}

//_________________________________________________________________________________________________
void replaceWithSummedHist( std::vector<TH1*>& hists )
{
    TH1* summed_hist = getSumHists( hists );
    hists.clear();
    hists.push_back( summed_hist );
}

//_________________________________________________________________________________________________
void clearGlobalSettings()
{
    options_array.clear();
    options.clear();
    map_pad_drawn.clear();
}

//_________________________________________________________________________________________________
void printYields( TH1* hist, int ibinmin, int ibinmax, std::ostream &yieldscout )
{
    yieldscout << center( hist->GetName(), 20 ) << " ,";
    for ( int ibin = ibinmin; ibin <= ibinmax; ++ibin )
    {
        yieldscout << prd( hist->GetBinContent( ibin ), 4, 9 );
        yieldscout << " +- ";
        yieldscout << prd( hist->GetBinError( ibin ), 4, 7 );
        if ( ibin < ibinmax )
            yieldscout << " ,";
        else if ( ibin == ibinmax )
            yieldscout << " \n";
    }
}

//_________________________________________________________________________________________________
bool printYieldsTable(
        std::vector<TH1*>& data_hists,
        std::vector<TH1*>& bkg_hists,
        std::vector<TH1*>& sig_hists,
        std::vector<TH1*>& ratio_hists,
        TString fname="" )
{

    if ( data_hists.size() == 0 && bkg_hists.size() == 0 && sig_hists.size() == 0 )
        return false;

    // https://stackoverflow.com/questions/366955/obtain-a-stdostream-either-from-stdcout-or-stdofstreamfile
    std::streambuf * buf;
    std::ofstream of;

    if ( !fname.IsNull() )
    {
        fname.ReplaceAll( ".png", "" );
        fname.ReplaceAll( ".pdf", "" );
        fname.ReplaceAll( ".C", "" );
        fname.ReplaceAll( ".txt", "" );
        of.open( ( fname + ".txt" ).Data() );
        buf = of.rdbuf();
    }
    else
    {
        buf = std::cout.rdbuf();
    }

    std::ostream yieldscout( buf );

    TH1* hist = 0;
    if ( data_hists.size() != 0 ) hist = data_hists[0];
    if ( bkg_hists.size()  != 0 ) hist = bkg_hists[0];
    if ( sig_hists.size()  != 0 ) hist = sig_hists[0];

    unsigned int ibinmin = 1;
    unsigned int ibinmax = hist->GetNbinsX();
    if ( !getOpt( "showUnderflow" ).IsNull() ) ibinmin = 0;
    if ( !getOpt( "showOverflow" ).IsNull() ) ibinmax ++;

    if ( !getOpt( "printYieldsMinBin" ).IsNull() )
        ibinmin = getOpt( "printYieldsMinBin" ).Atoi();
    if ( !getOpt( "printYieldsMaxBin" ).IsNull() )
        ibinmax = getOpt( "printYieldsMaxBin" ).Atoi();

    yieldscout << center( "name", 20 ) << " ,";

    // Putting column header
    for ( unsigned int ibin = ibinmin; ibin <= ibinmax; ++ibin )
    {
        yieldscout << center( Form( "bin%d", ibin ), 20 );
        if ( ibin < ibinmax )
            yieldscout << " ,";
        else if ( ibin == ibinmax )
            yieldscout << " \n";
    }

    for ( auto& bkg_hist  : bkg_hists )  printYields( bkg_hist , ibinmin, ibinmax, yieldscout );
    if ( bkg_hists.size() ) printYields( getTotalBkgHists( bkg_hists ), ibinmin, ibinmax, yieldscout );
    if ( getOpt( "noData" ).IsNull() ) for ( auto& data_hist : data_hists ) printYields( data_hist, ibinmin, ibinmax, yieldscout );
    if ( getOpt( "noData" ).IsNull() ) for ( auto& ratio_hist  : ratio_hists )  printYields( ratio_hist , ibinmin, ibinmax, yieldscout );
    if ( getOpt( "noData" ).IsNull() && !getOpt( "printObsSignif" ).IsNull() )
    {
        std::vector<TH1*> signif_hists = getSignifHists( data_hists, getTotalBkgHists( bkg_hists ) );
        for ( auto& signif_hist  : signif_hists )
            printYields( signif_hist , ibinmin, ibinmax, yieldscout );
    }
    for ( auto& sig_hist  : sig_hists )  printYields( sig_hist , ibinmin, ibinmax, yieldscout );
    if ( !getOpt( "printExpSignif" ).IsNull() )
    {
        std::vector<TH1*> signif_hists = getSignifHists( sig_hists, getTotalBkgHists( bkg_hists ), true );
        for ( auto& signif_hist  : signif_hists )
            printYields( signif_hist , ibinmin, ibinmax, yieldscout );
    }

    if ( !fname.IsNull() )
    {
        of.flush();
        of.close();
        gSystem->Exec( TString::Format( "chmod 755 %s", ( fname + ".txt" ).Data() ) );
    }

    return true;
}

//_________________________________________________________________________________________________
// http://www.cplusplus.com/forum/beginner/171037/
void transpose_CSV( const std::string& filename )
{
    typedef std::vector <std::string> record;
    std::deque <record> table;
    std::size_t cols = 0;

    // read the file
    {
        std::ifstream f( filename );
        std::string s;

        while ( std::getline( f, s ) )
        {
            record r;
            std::istringstream ss( s );
            std::string cell;

            while ( std::getline( ss, cell, ',' ) )
                r.emplace_back( cell );

            table.emplace_back( r );
            cols = std::max <std::size_t> ( cols, r.size() );
        }
    }

    // write the file, transposing (col <--> row)
    {
        std::ofstream f( filename );

        for ( std::size_t col = 0; col < cols; col++ )
        {
            f << table[ 0 ][ col ];

            for ( std::size_t row = 1; row < table.size(); row++ )
            {
                f << ",";

                if ( col < table[ row ].size() )
                    f << table[ row ][ col ];
            }

            f << "\n";
        }
    }
}

//_________________________________________________________________________________________________
void setMaximum(
        std::vector<TH1*> data_hists,
        std::vector<TH1*> bkg_hists,
        std::vector<TH1*> sig_hists )
{
    // Create allhists vector
    std::vector<TH1*> allhists = bkg_hists;
    if ( getOpt( "noData" ).IsNull() )
        allhists.insert( allhists.end(), data_hists.begin(), data_hists.end() );
    allhists.insert( allhists.end(), sig_hists.begin(), sig_hists.end() );
    TH1* totalhist = getTotalBkgHists( bkg_hists );
    allhists.push_back( totalhist );
    // Compute max
    float max = 0;
    for ( auto& hist : allhists )
        if ( max < ( hist->GetMaximum() + hist->GetBinError( hist->GetMaximumBin() ) ) )
            max = hist->GetMaximum() + hist->GetBinError( hist->GetMaximumBin() );
    float setmax = 1.8 * max;
    if ( getOpt( "Maximum" ).IsNull() ) options["Maximum"] = TString::Format( "%f", setmax );
    // Max of smallest histogram
    auto min_hist = *(std::min_element(allhists.begin(), allhists.end(), [](TH1* h1, TH1* h2) { return h1->Integral() < h2->Integral(); }));
    float maxofmin = min_hist->GetMaximum();
    if (maxofmin < 0)
        std::cout << min_hist->GetName() << std::endl;
    if (bkg_hists.size() == 1)
        maxofmin = getNonZeroMininimum(bkg_hists[0]);
    options["MinimumLogy"] = TString::Format( "%f", maxofmin * 0.5 );
    options["MaximumLogy"] = TString::Format( "%f", pow(10, 1.8*(TMath::Log10( max ) - TMath::Log10( maxofmin * 0.5 )) + TMath::Log10( maxofmin * 0.5 )) );
    std::cout << " " << options["MinimumLogy"] << " " << options["MaximumLogy"] << std::endl;
    if ( setmax < 100 )
        options["yTitleOffset"] = "1.4";
    else if ( setmax < 1000 )
        options["yTitleOffset"] = "1.75";
    else
        options["yTitleOffset"] = "2.1";
}

//_________________________________________________________________________________________________
void save( TPad* c,
        TString fname,
        std::vector<TH1*> data_hists = std::vector<TH1*>(),
        std::vector<TH1*> bkg_hists = std::vector<TH1*>(),
        std::vector<TH1*> sig_hists = std::vector<TH1*>(),
        std::vector<TH1*> ratio_hists = std::vector<TH1*>()
        )
{
    if ( printYieldsTable( data_hists, bkg_hists, sig_hists, ratio_hists, fname ) )
    {
        TString csvfilename = fname;
        csvfilename.ReplaceAll( ".png", "" );
        csvfilename.ReplaceAll( ".pdf", "" );
        csvfilename.ReplaceAll( ".C", "" );
        csvfilename.ReplaceAll( ".txt", "" );
        transpose_CSV( ( csvfilename + ".txt" ).Data() );
    }
    c->SaveAs( fname );
    gSystem->Chmod( fname, 755 );
}

//_________________________________________________________________________________________________
Hists makeHistsFromTH2( std::vector<TH2*> hists )
{
    // Final output instance
    Hists hist_pair_out;

    // Nominal histogram
    std::vector<TH1*> nomhists;
    // Total syst histogram
    std::vector<TH1*> systhists;

    // Loop over the input TH2's
    for ( unsigned int ihist = 0; ihist < hists.size(); ++ihist )
    {
        TH2* hist = hists[ihist];
        std::vector<TH1*> thissysts;
        for ( int ibin = 1; ibin <= hist->GetNbinsY(); ++ibin )
        {
            TString name = hist->GetName();
            // https://root-forum.cern.ch/t/setbinlabel-causes-unexpected-behavior-when-handling-the-histograms/26202/2
            THashList* labels = hist->GetXaxis()->GetLabels();
            if (labels)
                hist->GetXaxis()->SetRange(1, hist->GetXaxis()->GetNbins() ); 
            if ( ibin != 1 )
                name += Form( "syst%d", ibin - 1 );
            TH1D* projhist = hist->ProjectionX( name.Data(), ibin, ibin );
            projhist->SetDirectory( 0 );
            if ( ibin == 1 )
            {
                nomhists.push_back( projhist );
            }
            else
            {
                // If the histograms are booked but nothing is filled,
                // it's likely that the systematics didn't apply to this specific histogram.
                // In such case, we don't want to erroneously blow up the error.
                if ( projhist->Integral() != 0 )
                    thissysts.push_back( projhist );
            }
        }
        TH1* totalsyst = getSystByMaxDiff( nomhists.back(), thissysts, getNormSyst( hist ) );
        systhists.push_back( totalsyst );
    }

    for ( unsigned int ihist = 0; ihist < nomhists.size(); ++ihist )
        hist_pair_out.push_back( std::pair<TH1*, TH1*>( nomhists[ihist], systhists[ihist] ) );

    return hist_pair_out;
}

//_________________________________________________________________________________________________
std::vector<TH1*> plotmaker(
    std::string options_string,
    Hists datas_pair_in,
    Hists bkgs_pair_in,
    Hists sigs_pair_in = Hists()
)
{
    // ~-~-~-~-~-~-~-~-~-~-~
    // Clear global settings
    // ~-~-~-~-~-~-~-~-~-~-~
    clearGlobalSettings();

    // ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
    // Parse options in to a global variable
    // ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
    parseOptions( options_string );
    
    // ~-~-~-~-~-~-~-~
    // gStyle settings
    // ~-~-~-~-~-~-~-~
    gStyle->SetOptStat( 0 );
    if ( !getOpt( "MaxDigits" ).IsNull() )
        TGaxis::SetMaxDigits( getOpt( "MaxDigits" ).Atoi() );

    // ~-~-~-~-~-~-~-~
    // The main canvas
    // ~-~-~-~-~-~-~-~
    // For some reason, even though I ask 800x824, it outputs 796x796 pdf or png files.
    // In x-direction 4 pixels are added
    // In y-direction 28 pixels are added
    double canvas_width  = !getOpt( "legendOnMainPad" ).IsNull() ? 604 : 1004;
    double canvas_height = !getOpt( "ratioPaneAtBottom" ).IsNull() ? 728 : 828;
    TCanvas* canvas = new TCanvas( "canvas", "canvas", 0, 0, canvas_width, canvas_height );
    canvas->SetHighLightColor( 2 );
    canvas->Range( 0, 0, 1, 1 );
    canvas->SetFillColor( -1 );
    canvas->SetBorderMode( 0 );
    canvas->SetBorderSize( 0 );
    canvas->SetFrameBorderMode( 0 );
    canvas->SetFrameBorderSize( 0 );
    
    // ~-~-~-~-~-~-~
    // The main TPad
    // ~-~-~-~-~-~-~
    // The bottom main pain with 70% height in y-axis
    double legendPadOffset = !getOpt( "legendOnMainPad" ).IsNull() ? 0 : 0.4;
    TPad* pad0 = 0;
    if ( !getOpt( "ratioPaneAtBottom" ).IsNull() )
        pad0 = new TPad( "pad0", "pad0", 0, 0.2, 1 - legendPadOffset, 1.0 );
    else
        pad0 = new TPad( "pad0", "pad0", 0, 0, 1 - legendPadOffset, 0.7 );
    pad0->Draw();
    pad0->cd();
    pad0->Range( -80, -25953.19, 320, 103812.8 );
    pad0->SetFillColor( -1 );
    pad0->SetFillStyle( 4000 );
    pad0->SetBorderMode( 0 );
    pad0->SetBorderSize( 2 );
    pad0->SetLeftMargin( 150. / 600. );
    pad0->SetRightMargin( 50. / 600. );
    pad0->SetBottomMargin( 0.2 );
    pad0->SetTopMargin( 0.2 / 0.7  - 0.2 );
    pad0->SetFrameBorderMode( 0 );
    pad0->SetFrameBorderSize( 0 );
    pad0->SetFrameBorderMode( 0 );
    pad0->SetFrameBorderSize( 0 );
    
    // ~-~-~-~-~-~-~-~
    // The Legend TPad
    // ~-~-~-~-~-~-~-~
    // TPad for the TLegend
    canvas->cd();
    TPad* pad1 = new TPad( "pad1", "pad1", 1 - 0.40, 0, 1, 1 );
    if ( getOpt( "legendOnMainPad" ).IsNull() )
        pad1->Draw();
    
    // ~-~-~-~-~-~-~-~
    // The Ratio TPad
    // ~-~-~-~-~-~-~-~
    canvas->cd();
    TPad* pad2 = 0;
    if ( !getOpt( "ratioPaneAtBottom" ).IsNull() )
        pad2 = new TPad( "pad2", "pad2", 0, 0.0, 1 - legendPadOffset, 0.343 );
    else
        pad2 = new TPad( "pad2", "pad2", 0, 0.8, 1 - legendPadOffset, 1 );
    pad2->Draw();
    pad2->cd();
    pad2->Range( -80, -0.82, 320, 1.98 );
    pad2->SetFillColor( 0 );
//    pad2->SetFillStyle( 4000 );
    pad2->SetBorderMode( 0 );
    pad2->SetBorderSize( 2 );
    pad2->SetGridy();
    pad2->SetLeftMargin( 150. / 600. );
    pad2->SetRightMargin( 50. / 600. );
    pad2->SetBottomMargin( 0.4 );
    pad2->SetTopMargin( 0.05 );
    pad2->SetFrameBorderMode( 0 );
    pad2->SetFrameBorderSize( 0 );
    pad2->SetFrameBorderMode( 0 );
    pad2->SetFrameBorderSize( 0 );

    // ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
    // Sanity check that I even have histograms to work with
    // ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
    if ( datas_pair_in.size() + bkgs_pair_in.size() + sigs_pair_in.size() == 0 )
    {
        std::cout << "[plotmaker::] No histograms were provided!" << std::endl;
        std::cout << "[plotmaker::] What the hell do you want me to do with no TH1's?" << std::endl;
        return std::vector<TH1*>();
    }
    
    // ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
    // Turn the pair into a more workable "list of histograms"
    // ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~
    std::vector<TH1*> data_hists = histsWithFullError( datas_pair_in );
    std::vector<TH1*> bkg_hists = histsWithFullError( bkgs_pair_in );
    std::vector<TH1*> sig_hists = histsWithFullError( sigs_pair_in );

    // ~-~-~-~-~-~
    // Set Maximum
    // ~-~-~-~-~-~
    setMaximum( data_hists, bkg_hists, sig_hists );
    
    // ~-~-~-~-~-~-~-~
    // Draw background
    // ~-~-~-~-~-~-~-~
    THStack* stack = 0;
    if ( bkg_hists.size() )
    {

        // ~-~-~-~-~-~-~-~-
        // auto stack order
        // ~-~-~-~-~-~-~-~-
        if ( !getOpt( "autoStack" ).IsNull() )
        {
            struct {
                bool operator() (TH1* a, TH1* b) const
                {
                    return a->Integral() < b->Integral();
                }
            } sortByIntegral;
            std::sort( bkg_hists.begin(), bkg_hists.end(), sortByIntegral );
        }

        // ~-~-~-~-~-~-~-~-~-
        // Main bkg stack plot
        // ~-~-~-~-~-~-~-~-~-
        stack = getStack( bkg_hists );
        draw( stack, getOpt( "stack_DrawOpt" ), pad0 );
        
        // ~-~-~-~-~-~-~-~-~-
        // Total bkg histogram
        // ~-~-~-~-~-~-~-~-~-
        TH1* total = getTotalBkgHists( bkg_hists );
        draw( total, "hist p", pad0 );
        draw( total, "e2p ", pad0 );
    }

    // ~-~-~-~-~-~
    // Draw signal
    // ~-~-~-~-~-~
    if ( sig_hists.size() )
    {
        for ( auto& sig_hist : sig_hists )
            draw( sig_hist, "hist", pad0 );

        if ( !getOpt( "sumSigHists" ).IsNull() )
        {
            TH1* sumsig = getSumHists( sig_hists );
            TString sumsigname = "";
            for ( auto& sig_hist : sig_hists )
                sumsigname += sig_hist->GetName();
            sumsig->SetName( sumsigname );
            sumsig->SetLineStyle( 2 );
            sig_hists.push_back( sumsig );
            draw( sumsig, "hist", pad0 );
        }
    }
    
    // ~-~-~-~-~
    // Draw data
    // ~-~-~-~-~
    if ( data_hists.size() && getOpt( "noData" ).IsNull() )
    {
        if ( !getOpt( "sumDataHists" ).IsNull() )
            replaceWithSummedHist( data_hists );

        for ( auto& data_hist : data_hists )
            drawData( data_hist, getOpt( "data_DrawOpt" ), pad0 );
    }

    // ~-~-~-~-~-~
    // Draw legend
    // ~-~-~-~-~-~
    drawLegend( data_hists, bkg_hists, sig_hists, !getOpt( "legendOnMainPad" ).IsNull() ? pad0 : pad1 );
    
    // ~-~-~-~-~-~-~-~-~-~-~-
    // Draw CMS/Lumi/COM logo
    // ~-~-~-~-~-~-~-~-~-~-~-
    drawLogos( pad0 );
    
    // ~-~-~-~-~-~-~-~
    // Draw Ratio plot
    // ~-~-~-~-~-~-~-~
    std::vector<TH1*> ratio_hists;
    if ( data_hists.size() != 0 && bkg_hists.size() != 0 && getOpt( "noData").IsNull() )
    {
        ratio_hists = getRatioHists( data_hists, getTotalBkgHists( bkg_hists ) );

        for (auto& ratio_hist : ratio_hists )
            drawRatio( ratio_hist, getOpt( "ratio_DrawOpt" ), pad2 );
    }

    // ~-~-~-~-~-~-~-~-~-
    // Print yields table
    // ~-~-~-~-~-~-~-~-~-
    if ( !getOpt( "printYieldsTable" ).IsNull() )
    {
        printYieldsTable( data_hists, bkg_hists, sig_hists, ratio_hists );
    }
    
    // ~-~-~-~-~-
    // Save plots
    // ~-~-~-~-~-
    if ( !getOpt( "onlyLin" ).IsNull() )
    {
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_liny.png", data_hists, bkg_hists, sig_hists, ratio_hists );
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_liny.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
    }
    else if ( !getOpt( "onlyLog" ).IsNull() )
    {
        stack->SetMaximum( getOpt( "MaximumLogy" ).Atof() );
        stack->SetMinimum( getOpt( "MinimumLogy" ).Atof() );
        pad0->SetLogy();
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_logy.png", data_hists, bkg_hists, sig_hists, ratio_hists );
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_logy.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
        stack->SetMaximum( getOpt( "Maximum" ).Atof() );
        stack->SetMinimum( 0 );
    }
    else
    {
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_liny.png", data_hists, bkg_hists, sig_hists, ratio_hists );
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_liny.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
        stack->SetMaximum( getOpt( "MaximumLogy" ).Atof() );
        stack->SetMinimum( getOpt( "MinimumLogy" ).Atof() );
        pad0->SetLogy();
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_logy.png", data_hists, bkg_hists, sig_hists, ratio_hists );
        save( canvas, getOpt( "plotOutputName" ) + "_ratio_logy.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
        stack->SetMaximum( getOpt( "Maximum" ).Atof() );
        stack->SetMinimum( 0 );
    }

    // ~-~-~-~-~-~-~-~-~-
    // Save partial plots
    // ~-~-~-~-~-~-~-~-~-
    if ( !getOpt( "saveMainPad" ).IsNull() )
    {
        if ( !getOpt( "onlyLin" ).IsNull() )
        {
            pad0->SetLogy(0);
            save( pad0, getOpt( "plotOutputName" ) + "_main_liny.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
            save( pad0, getOpt( "plotOutputName" ) + "_main_liny.png", data_hists, bkg_hists, sig_hists, ratio_hists );
        }
        else if ( !getOpt( "onlyLog" ).IsNull() )
        {
            stack->SetMaximum( getOpt( "MaximumLogy" ).Atof() );
            stack->SetMinimum( getOpt( "MinimumLogy" ).Atof() );
            pad0->SetLogy(1);
            save( pad0, getOpt( "plotOutputName" ) + "_main_logy.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
            save( pad0, getOpt( "plotOutputName" ) + "_main_logy.png", data_hists, bkg_hists, sig_hists, ratio_hists );
            stack->SetMaximum( getOpt( "Maximum" ).Atof() );
            stack->SetMinimum( 0 );
        }
        else
        {
            pad0->SetLogy(0);
            save( pad0, getOpt( "plotOutputName" ) + "_main_liny.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
            save( pad0, getOpt( "plotOutputName" ) + "_main_liny.png", data_hists, bkg_hists, sig_hists, ratio_hists );
            stack->SetMaximum( getOpt( "MaximumLogy" ).Atof() );
            stack->SetMinimum( getOpt( "MinimumLogy" ).Atof() );
            pad0->SetLogy(1);
            save( pad0, getOpt( "plotOutputName" ) + "_main_logy.pdf", data_hists, bkg_hists, sig_hists, ratio_hists );
            save( pad0, getOpt( "plotOutputName" ) + "_main_logy.png", data_hists, bkg_hists, sig_hists, ratio_hists );
            stack->SetMaximum( getOpt( "Maximum" ).Atof() );
            stack->SetMinimum( 0 );
        }
    }

    delete canvas;

    return ratio_hists;
}

//_________________________________________________________________________________________________
std::vector<TH1*> plotmaker(
    std::string options_string,
    TH1* data_in,
    Hists bkgs_pair_in,
    Hists sigs_pair_in = Hists()
)
{
    Hists datas_pair_in;
    datas_pair_in.push_back( std::pair<TH1*, TH1*>( data_in, 0 ) );
    
    return plotmaker( options_string, datas_pair_in, bkgs_pair_in, sigs_pair_in );
}

//_________________________________________________________________________________________________
std::vector<TH1*> plotmaker(
    std::string options_string,
    TH1* data_in,
    std::vector<TH1*> bkg_hists,
    std::vector<TH1*> sig_hists = std::vector<TH1*>()
)
{

    Hists datas_pair_in;
    datas_pair_in.push_back( std::pair<TH1*, TH1*>( data_in, 0 ) );
    
    Hists bkgs_pair_in;
    
    for ( auto& bkg_hist : bkg_hists )
        bkgs_pair_in.push_back( std::pair<TH1*, TH1*>( bkg_hist, 0 ) );
        
    Hists sigs_pair_in;
    
    for ( auto& sig_hist : sig_hists )
        sigs_pair_in.push_back( std::pair<TH1*, TH1*>( sig_hist, 0 ) );
        
    return plotmaker( options_string, datas_pair_in, bkgs_pair_in, sigs_pair_in );
}

//_________________________________________________________________________________________________
std::vector<TH1*> plotmaker(
    std::string options_string,
    TH1* data_in,
    TH1* bkg_in,
    std::vector<TH1*> sig_hists = std::vector<TH1*>()
)
{

    Hists datas_pair_in;
    datas_pair_in.push_back( std::pair<TH1*, TH1*>( data_in, 0 ) );
    
    Hists bkgs_pair_in;
    bkgs_pair_in.push_back( std::pair<TH1*, TH1*>( bkg_in, 0 ) );
    
    Hists sigs_pair_in;
    
    for ( auto& sig_hist : sig_hists )
        sigs_pair_in.push_back( std::pair<TH1*, TH1*>( sig_hist, 0 ) );
        
    return plotmaker( options_string, datas_pair_in, bkgs_pair_in, sigs_pair_in );
}

//_________________________________________________________________________________________________
std::vector<TH1*> plotmaker(
    std::string options_string,
    std::vector<TH1*> data_hists,
    std::vector<TH1*> bkg_hists,
    std::vector<TH1*> sig_hists = std::vector<TH1*>()
)
{

    Hists datas_pair_in;
    
    for ( auto& data_hist : data_hists )
        datas_pair_in.push_back( std::pair<TH1*, TH1*>( data_hist, 0 ) );
        
    Hists bkgs_pair_in;
    
    for ( auto& bkg_hist : bkg_hists )
        bkgs_pair_in.push_back( std::pair<TH1*, TH1*>( bkg_hist, 0 ) );
        
    Hists sigs_pair_in;
    
    for ( auto& sig_hist : sig_hists )
        sigs_pair_in.push_back( std::pair<TH1*, TH1*>( sig_hist, 0 ) );
        
    return plotmaker( options_string, datas_pair_in, bkgs_pair_in, sigs_pair_in );
}

//_________________________________________________________________________________________________
std::vector<TH1*> plotmaker(
    std::string options_string,
    std::vector<TH1*> data_hists,
    std::vector<TH1*> bkg_hists,
    std::vector<TH1*> sig_hists,
    std::vector<TH1*> data_hists_syst,
    std::vector<TH1*> bkg_hists_syst,
    std::vector<TH1*> sig_hists_syst
)
{

    Hists datas_pair_in;
    
    for ( unsigned int idata = 0; idata < data_hists.size(); ++idata )
        datas_pair_in.push_back( std::pair<TH1*, TH1*>( data_hists[idata], data_hists_syst[idata] ) );
        
    Hists bkgs_pair_in;
    
    for ( unsigned int ibkg = 0; ibkg < bkg_hists.size(); ++ibkg )
        bkgs_pair_in.push_back( std::pair<TH1*, TH1*>( bkg_hists[ibkg], bkg_hists_syst[ibkg] ) );
        
    Hists sigs_pair_in;
    
    for ( unsigned int isig = 0; isig < sig_hists.size(); ++isig )
        sigs_pair_in.push_back( std::pair<TH1*, TH1*>( sig_hists[isig], sig_hists_syst[isig] ) );
        
    return plotmaker( options_string, datas_pair_in, bkgs_pair_in, sigs_pair_in );
}

//_________________________________________________________________________________________________
std::vector<TH1*> plotmaker(
    std::string options_string,
    std::vector<TH2*> data_hists,
    std::vector<TH2*> bkg_hists,
    std::vector<TH2*> sig_hists = std::vector<TH2*>()
)
{
    Hists datas_pair_in = makeHistsFromTH2( data_hists );
    Hists bkgs_pair_in = makeHistsFromTH2( bkg_hists );
    Hists sigs_pair_in = makeHistsFromTH2( sig_hists );
    return plotmaker( options_string, datas_pair_in, bkgs_pair_in, sigs_pair_in );
}

////_________________________________________________________________________________________________
//// Get binning information from histogram
//std::vector<double> getBinInfo( Hists& datas_pair_in, Hists& bkgs_pair_in, Hists& sigs_pair_in )
//{
//    std::vector<double> bininfo;
//    
//    if ( datas_pair_in.size() != 0 )
//    {
//        bininfo.push_back( datas_pair_in[0].first->GetNbinsX() );
//        bininfo.push_back( datas_pair_in[0].first->GetXaxis()->GetXmin() );
//        bininfo.push_back( datas_pair_in[0].first->GetXaxis()->GetXmax() );
//        return bininfo;
//    }
//    else if ( bkgs_pair_in.size() != 0 )
//    {
//        bininfo.push_back( datas_pair_in[0].first->GetNbinsX() );
//        bininfo.push_back( datas_pair_in[0].first->GetXaxis()->GetXmin() );
//        bininfo.push_back( datas_pair_in[0].first->GetXaxis()->GetXmax() );
//        return bininfo;
//    }
//    else if ( sigs_pair_in.size() != 0 )
//    {
//        bininfo.push_back( datas_pair_in[0].first->GetNbinsX() );
//        bininfo.push_back( datas_pair_in[0].first->GetXaxis()->GetXmin() );
//        bininfo.push_back( datas_pair_in[0].first->GetXaxis()->GetXmax() );
//        return bininfo;
//    }
//    else
//    {
//        std::cout << "[plotmaker::] Failed retrieving histogram binning information!" << std::endl;
//        std::cout << "[plotmaker::] No histograms were found at all! WTH?" << std::endl;
//        return bininfo;
//    }
//}

#endif
