#!/bin/env python

#  .
# ..: P.Chang, philip@physics.ucsd.edu

# ================================================================
# Wrapper script to plottery. (https://github.com/aminnj/plottery)
# ================================================================

import ROOT as r
from plottery import plottery as p
from plottery import utils as u
import math
import sys
import uuid
import os
sys.path.append("{0}/syncfiles/pyfiles".format(os.path.realpath(__file__).rsplit("/",1)[0]))
from pytable import *
from errors import E

# ================================================================
# New TColors
# ================================================================
mycolors = []
mycolors.append(r.TColor(11005 , 103 / 255. , 0   / 255. , 31  / 255.))
mycolors.append(r.TColor(11004 , 178 / 255. , 24  / 255. , 43  / 255.))
mycolors.append(r.TColor(11003 , 214 / 255. , 96  / 255. , 77  / 255.))
mycolors.append(r.TColor(11002 , 244 / 255. , 165 / 255. , 130 / 255.))
mycolors.append(r.TColor(11001 , 253 / 255. , 219 / 255. , 199 / 255.))
mycolors.append(r.TColor(11000 , 247 / 255. , 247 / 255. , 247 / 255.))
mycolors.append(r.TColor(11011 , 209 / 255. , 229 / 255. , 240 / 255.))
mycolors.append(r.TColor(11012 , 146 / 255. , 197 / 255. , 222 / 255.))
mycolors.append(r.TColor(11013 , 67  / 255. , 147 / 255. , 195 / 255.))
mycolors.append(r.TColor(11014 , 33  / 255. , 102 / 255. , 172 / 255.))
mycolors.append(r.TColor(11015 , 5   / 255. , 48  / 255. , 97  / 255.))

mycolors.append(r.TColor(3001 , 239 / 255. , 138 / 255. , 98  / 255.))
mycolors.append(r.TColor(3000 , 247 / 255. , 247 / 255. , 247 / 255.))
mycolors.append(r.TColor(3011 , 103 / 255. , 169 / 255. , 207 / 255.))

mycolors.append(r.TColor(5001 , 251 / 255. , 180 / 255. , 174 / 255.))
mycolors.append(r.TColor(5002 , 179 / 255. , 205 / 255. , 227 / 255.))
mycolors.append(r.TColor(5003 , 204 / 255. , 235 / 255. , 197 / 255.))
mycolors.append(r.TColor(5004 , 222 / 255. , 203 / 255. , 228 / 255.))
mycolors.append(r.TColor(5005 , 254 / 255. , 217 / 255. , 166 / 255.))

mycolors.append(r.TColor(7000 ,   0/255. ,   0/255. ,   0/255.))
mycolors.append(r.TColor(7001 , 213/255. ,  94/255. ,   0/255.)) #r
mycolors.append(r.TColor(7002 , 230/255. , 159/255. ,   0/255.)) #o
mycolors.append(r.TColor(7003 , 240/255. , 228/255. ,  66/255.)) #y
mycolors.append(r.TColor(7004 ,   0/255. , 158/255. , 115/255.)) #g
mycolors.append(r.TColor(7005 ,   0/255. , 114/255. , 178/255.)) #b
mycolors.append(r.TColor(7006 ,  86/255. , 180/255. , 233/255.)) #k
mycolors.append(r.TColor(7007 , 204/255. , 121/255. , 167/255.)) #p
mycolors.append(r.TColor(7011 , 110/255. ,  54/255. ,   0/255.)) #alt r
mycolors.append(r.TColor(7012 , 161/255. , 117/255. ,   0/255.)) #alt o
mycolors.append(r.TColor(7013 , 163/255. , 155/255. ,  47/255.)) #alt y
mycolors.append(r.TColor(7014 ,   0/255. , 102/255. ,  79/255.)) #alt g
mycolors.append(r.TColor(7015 ,   0/255. ,  93/255. , 135/255.)) #alt b
mycolors.append(r.TColor(7016 , 153/255. , 153/255. , 153/255.)) #alt k
mycolors.append(r.TColor(7017 , 140/255. ,  93/255. , 119/255.)) #alt p

mycolors.append(r.TColor(9001 ,  60/255. , 186/255. ,  84/255.))
mycolors.append(r.TColor(9002 , 244/255. , 194/255. ,  13/255.))
mycolors.append(r.TColor(9003 , 219/255. ,  50/255. ,  54/255.))
mycolors.append(r.TColor(9004 ,  72/255. , 133/255. , 237/255.))

# Color schemes from Hannsjoerg for WWW analysis
mycolors.append(r.TColor(2001 , 91  / 255. , 187 / 255. , 241 / 255.)) #light-blue
mycolors.append(r.TColor(2002 , 60  / 255. , 144 / 255. , 196 / 255.)) #blue
mycolors.append(r.TColor(2003 , 230 / 255. , 159 / 255. , 0   / 255.)) #orange
mycolors.append(r.TColor(2004 , 180 / 255. , 117 / 255. , 0   / 255.)) #brown
mycolors.append(r.TColor(2005 , 245 / 255. , 236 / 255. , 69  / 255.)) #yellow
mycolors.append(r.TColor(2006 , 215 / 255. , 200 / 255. , 0   / 255.)) #dark yellow
mycolors.append(r.TColor(2007 , 70  / 255. , 109 / 255. , 171 / 255.)) #blue-violet
mycolors.append(r.TColor(2008 , 70  / 255. , 90  / 255. , 134 / 255.)) #violet
mycolors.append(r.TColor(2009 , 55  / 255. , 65  / 255. , 100 / 255.)) #dark violet
mycolors.append(r.TColor(2010 , 120 / 255. , 160 / 255. , 0   / 255.)) #light green
mycolors.append(r.TColor(2011 , 0   / 255. , 158 / 255. , 115 / 255.)) #green
mycolors.append(r.TColor(2012 , 204 / 255. , 121 / 255. , 167 / 255.)) #pink?

default_colors = []
default_colors.extend(range(2001, 2013))
default_colors.extend(range(7001, 7018))




# ===============
# Histogram utils
# ===============

#______________________________________________________________________________________________________________________
def cloneTH1(obj, name=""):
    """
    Clone any TH1 object with the same name or new name.
    """
    if name == "":
        name = obj.GetName()
    rtn = obj.Clone(name)
    rtn.SetTitle("")
    if not rtn.GetSumw2N():
        rtn.Sumw2()
    rtn.SetDirectory(0)
    # https://root-forum.cern.ch/t/setbinlabel-causes-unexpected-behavior-when-handling-the-histograms/26202/2
    labels = rtn.GetXaxis().GetLabels()
    if labels:
        rtn.GetXaxis().SetRange(1, rtn.GetXaxis().GetNbins())
    return rtn;

#______________________________________________________________________________________________________________________
def get_total_hist(hists):
    """
    Sum all histograms and return a new copy of total bkg hist.
    """
    if len(hists) == 0:
        print "ERROR - the number of histograms are zero, while you asked me to sum them up."
    totalhist = cloneTH1(hists[0])
    totalhist.Reset()
    for hist in hists:
        totalhist.Add(hist)
    return totalhist

#______________________________________________________________________________________________________________________
def get_total_err_hist(hists):
    """
    Sum all histograms errors
    """
    if len(hists) == 0:
        print "ERROR - the number of histograms are zero, while you asked me to sum them up."
    totalhist = get_total_hist(hists)
    errhist = cloneTH1(totalhist)
    errhist.Reset()
    for i in xrange(0, totalhist.GetNbinsX() + 2):
        errhist.SetBinContent(i, totalhist.GetBinError(i))
    return errhist

#______________________________________________________________________________________________________________________
def getYaxisRange(hist):
    maximum = 0
    if hist:
        for ibin in xrange(0, hist.GetNbinsX()+2):
        #for ibin in xrange(1, hist.GetNbinsX()+1):
            c = hist.GetBinContent(ibin)
            e = hist.GetBinError(ibin)
            v = c + e
            if v > maximum:
                maximum = v

    return maximum

#______________________________________________________________________________________________________________________
def get_max_yaxis_range(hists):
    maximum = 0
    for hist in hists:
        v = getYaxisRange(hist)
        if v > maximum:
            maximum = v
    return maximum

#______________________________________________________________________________________________________________________
def get_max_yaxis_range_order_half_modded(maximum):
    firstdigit = int(str(maximum)[0])
    order = int(math.log10(maximum))
    if firstdigit <= 2:
        middle = (10.**(order - 1))
    else:
        middle = (10.**(order))
    return maximum + middle

#______________________________________________________________________________________________________________________
def remove_errors(hists):
    for hist in hists:
        for ibin in xrange(0, hist.GetNbinsX()+2):
            hist.SetBinError(ibin, 0)

#______________________________________________________________________________________________________________________
def rebin(hists, nbin):
    for hist in hists:
        if not hist: continue
        currnbin = hist.GetNbinsX()
        fac = currnbin / nbin
        if float(fac).is_integer() and fac > 0:
            hist.Rebin(fac)

#______________________________________________________________________________________________________________________
def single_divide_by_bin_width(hist):
    for ibin in xrange(1,hist.GetNbinsX()+2):
        hist.SetBinContent(ibin, hist.GetBinContent(ibin) / hist.GetBinWidth(ibin))
        hist.SetBinError(ibin, hist.GetBinError(ibin) / hist.GetBinWidth(ibin))

#______________________________________________________________________________________________________________________
def divide_by_bin_width(hists):
    for hist in hists:
        single_divide_by_bin_width(hist)

#______________________________________________________________________________________________________________________
def move_overflow(hists):
    def func(hist):
        of_bc = hist.GetBinContent(hist.GetNbinsX()+1)
        of_be = hist.GetBinError(hist.GetNbinsX()+1)
        lb_bc = hist.GetBinContent(hist.GetNbinsX())
        lb_be = hist.GetBinError(hist.GetNbinsX())
        lb_bc_new = lb_bc + of_bc
        lb_be_new = math.sqrt(lb_be**2 + of_be**2)
        hist.SetBinContent(hist.GetNbinsX(), lb_bc_new)
        hist.SetBinError(hist.GetNbinsX(), lb_be_new)
        hist.SetBinContent(hist.GetNbinsX()+1, 0)
        hist.SetBinError(hist.GetNbinsX()+1, 0)
    if isinstance(hists, list):
        for hist in hists:
            func(hist)
    else:
        func(hists)
    return hists

#______________________________________________________________________________________________________________________
def apply_nf(hists, nfs):
    def func(hist, nfs):
        if isinstance(nfs, list) and len(nfs) == 0:
            pass
        elif isinstance(nfs, float):
            for i in xrange(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs)
                hist.SetBinError(i, be * nfs)
        elif len(nfs) == hist.GetNbinsX():
            for i in xrange(1, hist.GetNbinsX()+1):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs[i-1])
                hist.SetBinError(i, be * nfs[i-1])
        elif len(nfs) == hist.GetNbinsX()+2:
            for i in xrange(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs[i])
                hist.SetBinError(i, be * nfs[i])
        elif len(nfs) == 1:
            for i in xrange(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs[0][0])
                hist.SetBinError(i, be * nfs[0][0])
    if isinstance(hists, list):
        for hist in hists:
            func(hist, nfs)
    else:
        func(hists, nfs)
    return hists

#______________________________________________________________________________________________________________________
def apply_nf_w_error(hists, nfs):
    def func(hist, nfs):
        if isinstance(nfs, list) and len(nfs) == 0:
            pass
        elif isinstance(nfs, float):
            for i in xrange(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs)
                hist.SetBinError(i, be * nfs)
        elif len(nfs) == hist.GetNbinsX():
            for i in xrange(1, hist.GetNbinsX()+1):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                bfe = be / bc if bc != 0 else 0
                nf = nfs[i-1][0]
                ne = nfs[i-1][1]
                nfe = ne / nf if nf != 0 else 0
                nbc = bc * nf
                nbe = math.sqrt(bfe**2 + nfe**2) * nbc
                hist.SetBinContent(i, nbc)
                hist.SetBinError(i, nbe)
        elif len(nfs) == hist.GetNbinsX()+2:
            for i in xrange(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                bfe = be / bc if bc != 0 else 0
                nf = nfs[i][0]
                ne = nfs[i][1]
                nfe = ne / nf if nf != 0 else 0
                nbc = bc * nf
                nbe = math.sqrt(bfe**2 + nfe**2) * nbc
                hist.SetBinContent(i, nbc)
                hist.SetBinError(i, nbe)
        elif len(nfs) == 1:
            for i in xrange(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                bfe = be / bc if bc != 0 else 0
                nf = nfs[0][0]
                ne = nfs[0][1]
                nfe = ne / nf if nf != 0 else 0
                nbc = bc * nf
                nbe = math.sqrt(bfe**2 + nfe**2) * nbc
                hist.SetBinContent(i, nbc)
                hist.SetBinError(i, nbe)
    if isinstance(hists, list):
        for hist in hists:
            func(hist, nfs)
    else:
        func(hists, nfs)
    return hists

#______________________________________________________________________________________________________________________
def apply_nf_2d(hists, nfs):
    def func(hist, nfs):
        if isinstance(nfs, list) and len(nfs) == 0:
            pass
        elif len(nfs) == 1:
            #hist.Scale(nfs[0][0])
            for i in xrange(0, hist.GetNbinsX()+2):
                for j in xrange(0, hist.GetNbinsY()+2):
                    bc = hist.GetBinContent(i, j)
                    be = hist.GetBinError(i, j)
                    nf = nfs[0][0]
                    hist.SetBinContent(i, j, bc * nf)
                    hist.SetBinError(i, j, be * nf)
        elif len(nfs) == hist.GetNbinsX():
            for i in xrange(1, hist.GetNbinsX()+1):
                for j in xrange(0, hist.GetNbinsY()+2):
                    bc = hist.GetBinContent(i, j)
                    be = hist.GetBinError(i, j)
                    nf = nfs[i-1][0]
                    hist.SetBinContent(i, j, bc * nf)
                    hist.SetBinError(i, j, be * nf)
        else:
            print "WARNING - apply_nf_w_error_2d: something went wrong."
    if isinstance(hists, list):
        for hist in hists:
            func(hist, nfs)
    else:
        func(hists, nfs)
    return hists

#______________________________________________________________________________________________________________________
def apply_nf_w_error_2d(hists, nfs):
    def func(hist, nfs):
        if isinstance(nfs, list) and len(nfs) == 0:
            pass
        elif len(nfs) == 1:
            for i in xrange(0, hist.GetNbinsX()+2):
                for j in xrange(0, hist.GetNbinsY()+2):
                    bc = hist.GetBinContent(i, j)
                    be = hist.GetBinError(i, j)
                    bfe = be / bc if bc != 0 else 0
                    nf = nfs[0][0]
                    ne = nfs[0][1]
                    nfe = ne / nf if nf != 0 else 0
                    nbc = bc * nf
                    nbe = math.sqrt(bfe**2 + nfe**2) * nbc
                    hist.SetBinContent(i, j, nbc)
                    hist.SetBinError(i, j, nbe)
        elif len(nfs) == hist.GetNbinsX():
            for i in xrange(1, hist.GetNbinsX()+1):
                for j in xrange(0, hist.GetNbinsY()+2):
                    bc = hist.GetBinContent(i, j)
                    be = hist.GetBinError(i, j)
                    bfe = be / bc if bc != 0 else 0
                    nf = nfs[i-1][0]
                    ne = nfs[i-1][1]
                    nfe = ne / nf if nf != 0 else 0
                    nbc = bc * nf
                    nbe = math.sqrt(bfe**2 + nfe**2) * nbc
                    hist.SetBinContent(i, j, nbc)
                    hist.SetBinError(i, j, nbe)
        else:
            print "WARNING - apply_nf_w_error_2d: something went wrong."
    if isinstance(hists, list):
        for hist in hists:
            func(hist, nfs)
    else:
        func(hists, nfs)
    return hists


# =================
# Significance scan
# =================

#______________________________________________________________________________________________________________________
# S / sqrt(B) fom
def fom_SoverSqrtB(s, serr, b, berr, totals, totalb):
    if b > 0:
        return s / math.sqrt(b), 0
    else:
        return 0, 0

#______________________________________________________________________________________________________________________
# S / sqrt(B +sB^2) fom
def fom_SoverSqrtBwErr(s, serr, b, berr, totals, totalb):
    if b > 0:
        return r.RooStats.NumberCountingUtils.BinomialExpZ(s, b, float(berr / b)), 0
        #return s / math.sqrt(b + berr*berr), 0
    else:
        return 0, 0

#______________________________________________________________________________________________________________________
# S / sqrt(B) fom
def fom_acceptance(s, serr, b, berr, totals, totalb):
    if totals != 0:
        return s / totals, 0
    else:
        return 0, 0

#______________________________________________________________________________________________________________________
# For each signal and total background return scan from left/right of fom (figure of merit) func.
def plot_sigscan2d(sig, bkg, fom=fom_SoverSqrtB):
    nbin = sig.GetNbinsX()
    if nbin != bkg.GetNbinsX():
        print "Error - significance scan for the signal and background histograms have different size", nbin, bkg.GetNbinsX()
    scan = cloneTH1(sig)
    scan.Reset()
    xmin = scan.GetXaxis().GetBinLowEdge(1)
    xwidth = scan.GetXaxis().GetBinWidth(1)
    max_f = 0
    max_f_cut_low = 0
    max_f_cut_high = 0
    totalsig = sig.Integral(0, nbin + 1)
    totalbkg = bkg.Integral(0, nbin + 1)

    for i in xrange(1, nbin + 1):
        local_max_f = 0
        local_max_f_err = 0
        for j in xrange(i + 1, nbin + 1):
            sigerr = r.Double(0)
            sigint = sig.IntegralAndError(i, j, sigerr)
            bkgerr = r.Double(0)
            bkgint = bkg.IntegralAndError(i, j, bkgerr)
            f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
            if max_f < f:
                max_f = f
                max_f_cut_low = xmin + xwidth * (i - 1)
                max_f_cut_high = xmin + xwidth * j
            if local_max_f < f:
                local_max_f = f
                local_max_f_err = ferr
        scan.SetBinContent(i, local_max_f)
        scan.SetBinError(i, ferr)
    scan.SetName("{:.4f} ({:.4f},{:.4f})".format(max_f, max_f_cut_low, max_f_cut_high))
    return scan

#______________________________________________________________________________________________________________________
# For each signal and total background return scan from left/right of fom (figure of merit) func.
def plot_sigscan(sig, bkg, fom=fom_SoverSqrtB):
    nbin = sig.GetNbinsX()
    if nbin != bkg.GetNbinsX():
        print "Error - significance scan for the signal and background histograms have different size", nbin, bkg.GetNbinsX()
    leftscan = cloneTH1(sig)
    leftscan.Reset()
    xmin = leftscan.GetXaxis().GetBinLowEdge(1)
    xwidth = leftscan.GetXaxis().GetBinWidth(1)
    max_f = 0
    max_f_cut = 0
    totalsig = sig.Integral(0, nbin + 1)
    totalbkg = bkg.Integral(0, nbin + 1)
    for i in xrange(1, nbin + 1):
        sigerr = r.Double(0)
        sigint = sig.IntegralAndError(i, nbin + 1, sigerr)
        bkgerr = r.Double(0)
        bkgint = bkg.IntegralAndError(i, nbin + 1, bkgerr)
        f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
        leftscan.SetBinContent(i, f)
        leftscan.SetBinError(i, ferr)
        if max_f < f:
            max_f = f
            max_f_cut = xmin + xwidth * (i - 1)
    leftscan.SetName("#rightarrow {:.4f} ({:.4f})".format(max_f, max_f_cut))
    rightscan = cloneTH1(sig)
    rightscan.Reset()
    max_f = 0
    max_f_cut = 0
    for i in reversed(xrange(1, nbin + 1)):
        sigerr = r.Double(0)
        sigint = sig.IntegralAndError(0, i, sigerr)
        bkgerr = r.Double(0)
        bkgint = bkg.IntegralAndError(0, i, bkgerr)
        f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
        rightscan.SetBinContent(i, f)
        rightscan.SetBinError(i, ferr)
        if max_f < f:
            max_f = f
            max_f_cut = xmin + xwidth * i
    rightscan.SetName("#leftarrow {:.4f} ({:.4f})".format(max_f, max_f_cut))
    return leftscan, rightscan

#______________________________________________________________________________________________________________________
# For each signal and indvidiual background plus systematics
def plot_sigscan_w_syst(sig, bkgs, systs, fom=fom_SoverSqrtBwErr):

    bkg = get_total_hist(bkgs)

    if len(bkgs) != len(systs) and len(systs) > 0:
        print "Error - The provided systs list does not have the same number of entries as the bkgs", bkgs, systs

    nbin = sig.GetNbinsX()
    if nbin != bkg.GetNbinsX():
        print "Error - significance scan for the signal and background histograms have different size", nbin, bkg.GetNbinsX()
    leftscan = cloneTH1(sig)
    leftscan.Reset()
    xmin = leftscan.GetXaxis().GetBinLowEdge(1)
    xwidth = leftscan.GetXaxis().GetBinWidth(1)
    max_f = -999
    max_f_cut = 0
    totalsig = sig.Integral(0, nbin + 1)
    totalbkg = bkg.Integral(0, nbin + 1)
    sigaccept = 0
    for i in xrange(1, nbin + 1):
        sigerr = r.Double(0)
        sigint = sig.IntegralAndError(i, nbin + 1, sigerr)
        bkgerr = r.Double(0)
        bkgint = bkg.IntegralAndError(i, nbin + 1, bkgerr)
        count_s = E(sigint, sigerr)
        count_b = E(bkgint, bkgerr)
        counts = []
        for index, bg in enumerate(bkgs):
            e = r.Double(0)
            c = bg.IntegralAndError(i, nbin + 1, e)
            ne = math.sqrt(e*e + c*systs[index]*c*systs[index])
            counts.append(E(c, ne))
        count_b_w_syst = E(0, 0)
        for count in counts:
            count_b_w_syst = count_b_w_syst + count
        bkgerr = count_b_w_syst.err
        f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
        #print i, f
        leftscan.SetBinContent(i, f)
        leftscan.SetBinError(i, ferr)
        if max_f < f:
            max_f = f
            max_f_cut = xmin + xwidth * (i - 1)
            sigaccept = sigint / totalsig
    leftscan.SetName("#rightarrow {:.4f} ({:.4f} {:.4f})".format(max_f, max_f_cut, sigaccept))

    rightscan = cloneTH1(sig)
    rightscan.Reset()
    max_f = -999
    max_f_cut = 0
    for i in reversed(xrange(1, nbin + 1)):
        sigerr = r.Double(0)
        sigint = sig.IntegralAndError(0, i, sigerr)
        bkgerr = r.Double(0)
        bkgint = bkg.IntegralAndError(0, i, bkgerr)
        count_s = E(sigint, sigerr)
        count_b = E(bkgint, bkgerr)
        counts = []
        for index, bg in enumerate(bkgs):
            e = r.Double(0)
            c = bg.IntegralAndError(0, i, e)
            ne = math.sqrt(e*e + c*systs[index]*c*systs[index])
            counts.append(E(c, ne))
        count_b_w_syst = E(0, 0)
        for count in counts:
            count_b_w_syst = count_b_w_syst + count
        f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
        rightscan.SetBinContent(i, f)
        rightscan.SetBinError(i, ferr)
        if max_f < f:
            max_f = f
            max_f_cut = xmin + xwidth * (i - 1)
    rightscan.SetName("#leftarrow {:.4f} ({:.4f})".format(max_f, max_f_cut))

    return leftscan, rightscan

# ====================
# Yield table printing
# ====================

#______________________________________________________________________________________________________________________
def yield_str(hist, i, prec=3):
    e = E(hist.GetBinContent(i), hist.GetBinError(i))
    return e.round(prec)

#______________________________________________________________________________________________________________________
def print_yield_table_from_list(hists, outputname, prec=2):
    x = Table()
    if len(hists) == 0:
        return
    # add bin column
    x.add_column("Bin#", ["Bin{}".format(i) for i in xrange(1, hists[0].GetNbinsX()+1)])
    for hist in hists:
        x.add_column(hist.GetName(), [ yield_str(hist, i, prec) for i in xrange(1, hist.GetNbinsX()+1)])
    fname = outputname
    fname = os.path.splitext(fname)[0]+'.txt'
    x.print_table()
    x.set_theme_basic()
    f = open(fname, "w")
    f.write("".join(x.get_table_string()))

#______________________________________________________________________________________________________________________
def print_yield_table(hdata, hbkgs, hsigs, hsyst, options):
    hists = []
    hists.extend(hbkgs)
    hists.extend(hsigs)
    htotal = None
    if len(hbkgs) != 0:
        htotal = get_total_hist(hbkgs)
        htotal.SetName("Total")
        hists.append(htotal)
    if hdata and len(hbkgs) != 0:
        hratio = makeRatioHist(hdata, hbkgs)
        #hists.append(htotal)
        hists.append(hdata)
        hists.append(hratio)
    prec = 2
    if "yield_prec" in options:
        prec = options["yield_prec"]
        del options["yield_prec"]
    print_yield_table_from_list(hists, options["output_name"], prec)

def copy_nice_plot_index_php(options):
    plotdir = os.path.dirname(options["output_name"])
    if len(plotdir) == 0: plotdir = "./"
    os.system("cp {}/syncfiles/miscfiles/index.php {}/".format(os.path.realpath(__file__).rsplit("/",1)[0], plotdir))



# ====================
# The plottery wrapper
# ====================

#______________________________________________________________________________________________________________________
def plot_hist(data=None, bgs=[], sigs=[], syst=None, options={}, colors=[], sig_labels=[], legend_labels=[]):
    """
    Wrapper function to call Plottery.
    """

    # Sanity check. If no histograms exit
    if not data and len(bgs) == 0 and len(sigs) == 0:
        print "[plottery_wrapper] >>> Nothing to do!"
        return

    # If a blind option is set, blind the data histogram to None
    # The later step will set the histogram of data to all zero
    if "blind" in options:
        if options["blind"]:
            data = None
        del options["blind"]

    # "nbins" initiate rebinning
    if "nbins" in options:
        nbins = options["nbins"]
        rebin(sigs, nbins)
        rebin(bgs, nbins)
        rebin([data], nbins)
        del options["nbins"]

    if "divide_by_bin_width" in options:
        if options["divide_by_bin_width"]:
            divide_by_bin_width(sigs)
            divide_by_bin_width(bgs)
            divide_by_bin_width([data])
            if "yaxis_label" not in options:
                options["yaxis_label"] = "Events / Bin Width"
        del options["divide_by_bin_width"]

    # If data is none clone one hist and fill with 0
    didnothaveanydata = False
    if not data:
        didnothaveanydata = True
        if len(bgs) != 0:
            data = bgs[0].Clone("Data")
            data.Reset()
        elif len(sigs) != 0:
            data = sigs[0].Clone("Data")
            data.Reset()

    # Compute some arguments that are missing (viz. colors, sig_labels, legend_labels)
    hsig_labels = []
    if len(sig_labels) == 0:
        for hsig in sigs:
            hsig_labels.append(hsig.GetName())
    hcolors = colors
    if len(colors) == 0:
        for index, hbg in enumerate(bgs):
            hcolors.append(default_colors[index])
    hlegend_labels = []
    if len(legend_labels) == 0:
        for hbg in bgs:
            hlegend_labels.append(hbg.GetName())

    # Set maximum of the plot
    totalbkg = None
    if len(bgs) != 0:
        totalbkg = get_total_hist(bgs)
    maxmult = 1.8
    if "ymax_scale" in options:
        maxmult = options["ymax_scale"]
        del options["ymax_scale"]
    yaxismax = get_max_yaxis_range_order_half_modded(get_max_yaxis_range([data, totalbkg]) * maxmult)

    # Once maximum is computed, set the y-axis label location
    if yaxismax < 0.01:
        options["yaxis_title_offset"] = 1.8
    elif yaxismax < 0.1:
        options["yaxis_title_offset"] = 1.6
    elif yaxismax < 10.:
        options["yaxis_title_offset"] = 1.6
    elif yaxismax < 100:
        options["yaxis_title_offset"] = 1.2
    elif yaxismax < 1000:
        options["yaxis_title_offset"] = 1.45
    elif yaxismax < 10000:
        options["yaxis_title_offset"] = 1.6
    else:
        options["yaxis_title_offset"] = 1.8

    # Print histogram content for debugging
    #totalbkg.Print("all")
    #if len(sigs) > 0:
    #    sigs[0].Print("all")
    #for hbg in bgs:
    #    hbg.Print("all")

    # Print yield table if the option is turned on
    if "print_yield" in options:
        if options["print_yield"]:
            print_yield_table(data, bgs, sigs, syst, options)
        del options["print_yield"]

    # Inject signal option
    if "inject_signal" in options:
        if options["inject_signal"]:
            if len(sigs) > 0:
                data = sigs[0].Clone("test")
                data.Reset()
                for hsig in sigs:
                    data.Add(hsig)
                for hbkg in bgs:
                    data.Add(hbkg)
                for i in xrange(1, data.GetNbinsX() + 1):
                    data.SetBinError(i, 0)
                options["legend_datalabel"] = "Sig+Bkg"
        del options["inject_signal"]

    # If syst is not provided, compute one yourself from the bkg histograms
    if not syst:
        syst = get_total_err_hist(bgs)

    # The uncertainties are all accounted in the syst so remove all errors from bkgs
    remove_errors(bgs)

    # Here are my default options for plottery
    #if not "canvas_width"             in options: options["canvas_width"]              = 604
    #if not "canvas_height"            in options: options["canvas_height"]             = 728
    if not "canvas_width"                   in options: options["canvas_width"]                   = 454
    if not "canvas_height"                  in options: options["canvas_height"]                  = 553
    if not "yaxis_range"                    in options: options["yaxis_range"]                    = [0., yaxismax]
    if not "legend_ncolumns"                in options: options["legend_ncolumns"]                = 2 if len(bgs) >= 4 else 1
    if not "legend_alignment"               in options: options["legend_alignment"]               = "topright"
    if not "legend_smart"                   in options: options["legend_smart"]                   = True
    if not "legend_scalex"                  in options: options["legend_scalex"]                  = 0.8
    if not "legend_scaley"                  in options: options["legend_scaley"]                  = 0.8
    if not "legend_border"                  in options: options["legend_border"]                  = False
    if not "legend_percentageinbox"         in options: options["legend_percentageinbox"]         = False
    if not "hist_line_none"                 in options: options["hist_line_none"]                 = True
    if not "show_bkg_errors"                in options: options["show_bkg_errors"]                = False
    if not "ratio_range"                    in options: options["ratio_range"]                    = [0.7, 1.3]
    if not "ratio_name_size"                in options: options["ratio_name_size"]                = 0.13
    if not "ratio_name_offset"              in options: options["ratio_name_offset"]              = 0.6
    if not "ratio_xaxis_label_offset"       in options: options["ratio_xaxis_label_offset"]       = 0.06
    if not "ratio_yaxis_label_offset"       in options: options["ratio_yaxis_label_offset"]       = 0.03
    if not "ratio_xaxis_title_offset"       in options: options["ratio_xaxis_title_offset"]       = 1.60 if "xaxis_log" in options and options["xaxis_log"] else 1.40
    if not "ratio_xaxis_title_size"         in options: options["ratio_xaxis_title_size"]         = 0.13
    if not "ratio_label_size"               in options: options["ratio_label_size"]               = 0.13
    if not "canvas_tick_one_side"           in options: options["canvas_tick_one_side"]           = True
    if not "canvas_main_y1"                 in options: options["canvas_main_y1"]                 = 0.2
    if not "canvas_main_topmargin"          in options: options["canvas_main_topmargin"]          = 0.2 / 0.7 - 0.2
    if not "canvas_main_rightmargin"        in options: options["canvas_main_rightmargin"]        = 50. / 600.
    if not "canvas_main_bottommargin"       in options: options["canvas_main_bottommargin"]       = 0.2
    if not "canvas_main_leftmargin"         in options: options["canvas_main_leftmargin"]         = 130. / 600.
    if not "canvas_ratio_y2"                in options: options["canvas_ratio_y2"]                = 0.342
    if not "canvas_ratio_topmargin"         in options: options["canvas_ratio_topmargin"]         = 0.05
    if not "canvas_ratio_rightmargin"       in options: options["canvas_ratio_rightmargin"]       = 50. / 600.
    if not "canvas_ratio_bottommargin"      in options: options["canvas_ratio_bottommargin"]      = 0.4
    if not "canvas_ratio_leftmargin"        in options: options["canvas_ratio_leftmargin"]        = 130. / 600.
    if not "xaxis_title_size"               in options: options["xaxis_title_size"]               = 0.06
    if not "yaxis_title_size"               in options: options["yaxis_title_size"]               = 0.06
    if not "xaxis_title_offset"             in options: options["xaxis_title_offset"]             = 1.4 
    if not "yaxis_title_offset"             in options: options["yaxis_title_offset"]             = 1.4 
    if not "xaxis_label_size_scale"         in options: options["xaxis_label_size_scale"]         = 1.4
    if not "yaxis_label_size_scale"         in options: options["yaxis_label_size_scale"]         = 1.4
    if not "xaxis_label_offset_scale"       in options: options["xaxis_label_offset_scale"]       = 4.0
    if not "yaxis_label_offset_scale"       in options: options["yaxis_label_offset_scale"]       = 4.0
    if not "xaxis_tick_length_scale"        in options: options["xaxis_tick_length_scale"]        = -0.8
    if not "yaxis_tick_length_scale"        in options: options["yaxis_tick_length_scale"]        = -0.8
    if not "ratio_tick_length_scale"        in options: options["ratio_tick_length_scale"]        = -1.0
    if not "output_name"                    in options: options["output_name"]                    = "plots/plot.png"
    if not "cms_label"                      in options: options["cms_label"]                      = "Preliminary"
    if not "lumi_value"                     in options: options["lumi_value"]                     = "35.9"
    if not "bkg_err_fill_style"             in options: options["bkg_err_fill_style"]             = 3245
    if not "bkg_err_fill_color"             in options: options["bkg_err_fill_color"]             = 1
    if not "output_ic"                      in options: options["output_ic"]                      = 0
    if not "yaxis_moreloglabels"            in options: options["yaxis_moreloglabels"]            = False
    if not "yaxis_noexponents"              in options: options["yaxis_noexponents"]              = False
    if not "yaxis_exponent_offset"          in options: options["yaxis_exponent_offset"]          = -0.1
    if not "yaxis_exponent_vertical_offset" in options: options["yaxis_exponent_vertical_offset"] = 0.02
    if not "yaxis_ndivisions"               in options: options["yaxis_ndivisions"]               = 508
    if not "xaxis_ndivisions"               in options: options["xaxis_ndivisions"]               = 508
    if not "max_digits"                     in options: options["max_digits"]                     = 4
    if "no_ratio" in options:
        options["canvas_width"] = 566
        options["canvas_height"] = 553
        #options["canvas_width"] = (566 - 4) * 2 + 4
        #options["canvas_height"] = (553 - 28) * 2 + 28

    #if "no_ratio" in options:
    #    if options["no_ratio"]:
    #        options["canvas_ratio_y2"] = 0.0
    #        options["canvas_ratio_y1"] = 0.0
    #        options["canvas_ratio_x2"] = 0.0
    #        options["canvas_ratio_x1"] = 0.0
    #    del options["no_ratio"]

    # If you did not pass any data then set data back to None
    if didnothaveanydata:
        data = None

    # Call Plottery! I hope you win the Lottery!
    c1 = p.plot_hist(
            data          = data,
            bgs           = bgs,
            sigs          = sigs,
            syst          = syst,
            sig_labels    = hsig_labels,
            colors        = hcolors,
            legend_labels = hlegend_labels,
            options       = options
            )

    #c1.SaveAs("plots/plot.pdf")
    #c1.SaveAs("plots/plot.C")

    # Set permission
    os.system("chmod 644 {}".format(options["output_name"]))

    # Call nice plots
    copy_nice_plot_index_php(options)

#______________________________________________________________________________________________________________________
def plot_cut_scan(data=None, bgs=[], sigs=[], syst=None, options={}, colors=[], sig_labels=[], legend_labels=[]):
    hsigs = []
    hbgs = []
    if syst:
        leftscan, rightscan = plot_sigscan_w_syst(sigs[0], bgs, systs=syst)
    else:
        leftscan, rightscan = plot_sigscan(sigs[0], get_total_hist(bgs))
    hbgs.append(leftscan)
    hsigs.append(rightscan)
    hsigs.append(plot_sigscan2d(sigs[0], get_total_hist(bgs)))
    leftscan, rightscan = plot_sigscan(sigs[0], get_total_hist(bgs), fom_acceptance)
    hsigs.append(leftscan)
    hsigs.append(rightscan)
    options["bkg_err_fill_color"] = 0
    options["output_name"] = options["output_name"].replace(".png", "_cut_scan.png")
    plot_hist(data=None, sigs=hsigs, bgs=hbgs, syst=None, options=options, colors=colors, sig_labels=sig_labels, legend_labels=legend_labels)

