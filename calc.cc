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

//_________________________________________________________________________________________________
float RooUtil::Calc::DeltaR(const LV& a, const LV& b)
{
    return ROOT::Math::VectorUtil::DeltaR(a, b);
}

//_________________________________________________________________________________________________
float RooUtil::Calc::DeltaEta(const LV& a, const LV& b)
{
    return a.eta() - b.eta();
}

//_________________________________________________________________________________________________
float RooUtil::Calc::DeltaPhi(const LV& a, const LV& b)
{
    return ROOT::Math::VectorUtil::DeltaPhi(a, b);
}

//_________________________________________________________________________________________________
void RooUtil::Calc::printTLV(const TLorentzVector& a)
{
    std::cout <<  " a.Pt(): " << a.Pt() <<  " a.Eta(): " << a.Eta() <<  " a.Phi(): " << a.Phi() <<  " a.M(): " << a.M() <<  " a.E(): " << a.E() <<  std::endl;
}

//_________________________________________________________________________________________________
void RooUtil::Calc::printLV(const LV& a)
{
    std::cout <<  " a.pt(): " << a.pt() <<  " a.eta(): " << a.eta() <<  " a.phi(): " << a.phi() <<  " a.mass(): " << a.mass() <<  " a.energy(): " << a.energy() <<  std::endl;
}

//eof
