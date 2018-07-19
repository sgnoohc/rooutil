//  .
// ..: P. Chang, philip@physics.ucsd.edu

#include "calc.h"

using namespace RooUtil;

//_________________________________________________________________________________________________
TLorentzVector RooUtil::Calc::getTLV(const LV& a)
{
    TLorentzVector r;
    r.SetPtEtaPhiM(a.pt(), a.eta(), a.phi(), a.mass());
    return r;
}

//_________________________________________________________________________________________________
LV RooUtil::Calc::getLV(const TLorentzVector& a)
{
    LV r;
    r.SetPxPyPzE(a.Px(), a.Py(), a.Pz(), a.E());
    return r;
}

//_________________________________________________________________________________________________
TVector3 RooUtil::Calc::boostVector(const LV& a)
{
    return getTLV(a).BoostVector();
}

//_________________________________________________________________________________________________
LV RooUtil::Calc::getBoosted(const LV& a, const TVector3& b)
{
    TLorentzVector tlv_r = getTLV(a);
    tlv_r.Boost(b);
    return getLV(tlv_r);
}

//_________________________________________________________________________________________________
void RooUtil::Calc::boost(LV& a, const TVector3& b)
{
    TLorentzVector tlv_r = getTLV(a);
    tlv_r.Boost(b);
    a = getLV(tlv_r);
}

//eof
