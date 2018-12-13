//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef calc_h
#define calc_h

// C/C++

// ROOT
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TVector2.h"
#include "Math/LorentzVector.h"
#include "Math/VectorUtil.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// LorentzVector typedef that we use very often
///////////////////////////////////////////////////////////////////////////////////////////////
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LV;

namespace RooUtil
{
    namespace Calc {
        TLorentzVector getTLV(const LV& a);
        LV getLV(const TLorentzVector& a);
        TVector3 boostVector(const LV& a);
        LV getBoosted(const LV& a, const TVector3& b);
        void boost(LV& a, const TVector3& b);
        float DeltaR(const LV& a, const LV& b);
        float DeltaEta(const LV& a, const LV& b);
        float DeltaPhi(const LV& a, const LV& b);
        LV getNeutrinoP4(const LV& lep, const float& met_pt, const float& met_phi, float mw=80.385, bool getsol2=false, bool invertpz=false);
        float getNeutrinoPz(const LV& lep, const float& met_pt, const float& met_phi, float mw=80.385, bool getsol2=false);
        float getNeutrinoPzDet(const LV& lep, const float& met_pt, const float& met_phi, float mw=80.385);
        TVector2 getEtaPhiVecRotated(const LV& target, const LV& ref, const LV& axis_ref);
        void printTLV(const TLorentzVector& a);
        void printLV(const LV& a);
        int calcBin2D(const std::vector<float>& xbounds, const std::vector<float>& ybounds, float xval, float yval);
    }
}

#endif
