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
LV RooUtil::Calc::getLV(float pt, float eta, float phi, float m)
{
    TLorentzVector tmp;
    tmp.SetPtEtaPhiM(pt, eta, phi, m);
    return getLV(tmp);
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
LV RooUtil::Calc::getNeutrinoP4(const LV& lep, const float& met_pt, const float& met_phi, float mw, bool getsol2, bool invertpz)
{
    float pz = getNeutrinoPz(lep, met_pt, met_phi, mw, getsol2);
    if (invertpz)
        pz = -pz;
    LV neutrino;
    using namespace TMath;
    float E = Sqrt(Power(met_pt*Cos(met_phi),2) + Power(met_pt*Sin(met_phi),2) + pz*pz);
    neutrino.SetPxPyPzE(met_pt*TMath::Cos(met_phi), met_pt*TMath::Sin(met_phi), pz, E);
    return neutrino;
}

//_________________________________________________________________________________________________
float RooUtil::Calc::getNeutrinoPzDet(const LV& lep, const float& met_pt, const float& met_phi, float mw)
{
    // Pz reconstruction for WW->lvjj from https://arxiv.org/pdf/1503.04677.pdf
    float lx = lep.px();
    float ly = lep.py();
    float lz = lep.pz();
    TLorentzVector met;
    met.SetPtEtaPhiM(met_pt, 0, met_phi, 0);
    float vx = met.Px();
    float vy = met.Py();

    using namespace TMath;
    float det = (Power(lx,2) + Power(ly,2) + Power(lz,2))* (Power(mw,4) - 4*Power(ly*vx - lx*vy,2) + 4*Power(mw,2)*(lx*vx + ly*vy));

    return det;
}

//_________________________________________________________________________________________________
float RooUtil::Calc::getNeutrinoPz(const LV& lep, const float& met_pt, const float& met_phi, float mw, bool getsol2)
{
    // Pz reconstruction for WW->lvjj from https://arxiv.org/pdf/1503.04677.pdf
    float lx = lep.px();
    float ly = lep.py();
    float lz = lep.pz();
    TLorentzVector met;
    met.SetPtEtaPhiM(met_pt, 0, met_phi, 0);
    float vx = met.Px();
    float vy = met.Py();

    using namespace TMath;
    float det = getNeutrinoPzDet(lep, met_pt, met_phi, mw);

    // If imaginary then ignore imaginary component and take the real value only
    if (det < 0)
        det = 0;
    float sol1 = (lz*(Power(mw,2) + 2*lx*vx + 2*ly*vy) - Sqrt(det))/(2.*(Power(lx,2) + Power(ly,2)));
    float sol2 = (lz*(Power(mw,2) + 2*lx*vx + 2*ly*vy) + Sqrt(det))/(2.*(Power(lx,2) + Power(ly,2)));

    float ans  = fabs(sol1) < fabs(sol2) ? sol1 : sol2;
    float ans2 = fabs(sol1) < fabs(sol2) ? sol2 : sol1;

    if (getsol2)
        return ans2;
    else
        return ans;
}

/*
//_________________________________________________________________________________________________
//
//            axis_ref
//                /
//               /  ref_vec.Phi()
//              /
//            ref=============>  +x
//              \
//               \
//                \
//                 \
//               target
//
//  ================================
//
//         axis_ref
//             |
//             |
//             |
//            ref
//              \__
//                 \__
//                    target
//
//
// The function rotates the axis_ref to be directly above ref and return target's TVector2
//
//
*/
TVector2 RooUtil::Calc::getEtaPhiVecRotated(const LV& target, const LV& ref, const LV& axis_ref)
{
    float deta = RooUtil::Calc::DeltaEta(axis_ref, ref);
    float dphi = RooUtil::Calc::DeltaPhi(axis_ref, ref);
    TVector2 ref_vec(deta, dphi);

    float target_deta = RooUtil::Calc::DeltaEta(target, ref);
    float target_dphi = RooUtil::Calc::DeltaEta(target, ref);
    TVector2 target_vec(target_deta, target_dphi);

    // The rotation can be thought of as "rotate it to align with +x axis, then add 90 degrees"
    return target_vec.Rotate(-ref_vec.Phi() + TMath::Pi() / 2.);
}

float RooUtil::Calc::getRho(const LV& ref, const LV& target)
{
/*
            phi
             ^
             | target
             |   /
             |  /
             | / "rho"
            ref-------------> eta

*/

    float dy = DeltaPhi(ref, target);
    float dx = DeltaEta(ref, target);
    return TMath::ATan(dy / dx);
}

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
