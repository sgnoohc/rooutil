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

//_________________________________________________________________________________________________
// Two bounds are provided, and a point. computes the bin number
int RooUtil::Calc::calcBin2D(const std::vector<float>& xbounds, const std::vector<float>& ybounds, float xval, float yval)
{

//  -1
//
//   2  2.4
//             7    8    9   10   11   12
//   1  1.6
//             1    2    3    4    5    6
//   0  0.0
//           0   20   25   30   35   50   150
//           0    1    2    3    4    5    6    -1

//    1   2   3   4
//    5   6   7   8

    int cx = -1;
    int cy = -1;

    for (unsigned ix = 0; ix < xbounds.size(); ++ix)
    {
        // If the bound value is smaller than the xval I need to continue until the xbound is bigger than the given xval
        if (xbounds[ix] < xval)
            continue;

        // Set the cx (chosen x index) to ix
        cx = ix;
        break;
    }

    // If it reached the highest bound set it to the last one
    if (cx == -1) cx = xbounds.size() - 1;

    for (unsigned iy = 0; iy < ybounds.size(); ++iy)
    {
        // If the bound value is smaller than the xval I need to continue until the xbound is bigger than the given xval
        if (ybounds[iy] < yval)
            continue;

        // Set the cy (chosen y index) to iy
        cy = iy;
        break;
    }

    // If it reached the highest bound set it to the last one
    if (cy == -1) cy = ybounds.size() - 1;

    // If neither hit the value i want it failed to find one
    if (cx == 0 or cy == 0)
        return -1;

    return (cx - 1) + (cy - 1) * (xbounds.size()-1);
}

//eof
