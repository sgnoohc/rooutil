//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef stringutil_h
#define stringutil_h

// Adopted to RooUtil by P. Chang (UCSD)

// Originally written by
// T. M. Hong, tmhong@hep.upenn.edu
// P. Chang, pcchang2@illinois.edu
// B. Cerio, bcc11@phy.duke.edu
// https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/HiggsPhys/HSG3/WWDileptonAnalysisCode/HWWMVAShared/trunk/mvashared/src/MvaStringUtils.cxx

// std
#include <string>
#include <sstream>
#include <vector>

// ROOT
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"

namespace RooUtil
{
    namespace StringUtil
    {
        typedef std::vector<TString> vecTString;
        // --------------------------------------------------------------------------
        // MvaStringUtils.cxx : python-like string manipulations
        vecTString  filter(vecTString &vec, TString keyword);

        // -- Python-like functions
        void       rstrip     (TString &in,   TString separator="#"                                       );
        vecTString split      (TString in,    TString separator=" "                                       );
        TString    join       (vecTString in, TString joiner=":",                        Int_t rm_blanks=1);
        TString    sjoin      (TString in,    TString separator=" ", TString joiner=":", Int_t rm_blanks=1);
    }
}

#endif
