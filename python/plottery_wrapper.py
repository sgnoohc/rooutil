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
import tabletex
from errors import E
import errno    
import pyrootutil as ru
from ctypes import c_double

RooUtil_StatUtil_Loaded = False

# ================================================================
# New TColors
# ================================================================
mycolors = []
mycolors.append(r.TColor(11005 , 103 / 255. , 0   / 255. , 31  / 255.  , "color0001", 1)) 
mycolors.append(r.TColor(11004 , 178 / 255. , 24  / 255. , 43  / 255.  , "color0002", 1)) 
mycolors.append(r.TColor(11003 , 214 / 255. , 96  / 255. , 77  / 255.  , "color0003", 1)) 
mycolors.append(r.TColor(11002 , 244 / 255. , 165 / 255. , 130 / 255.  , "color0004", 1)) 
mycolors.append(r.TColor(11001 , 253 / 255. , 219 / 255. , 199 / 255.  , "color0005", 1)) 
mycolors.append(r.TColor(11000 , 247 / 255. , 247 / 255. , 247 / 255.  , "color0006", 1)) 
mycolors.append(r.TColor(11011 , 209 / 255. , 229 / 255. , 240 / 255.  , "color0007", 1)) 
mycolors.append(r.TColor(11012 , 146 / 255. , 197 / 255. , 222 / 255.  , "color0010", 1)) 
mycolors.append(r.TColor(11013 , 67  / 255. , 147 / 255. , 195 / 255.  , "color0011", 1)) 
mycolors.append(r.TColor(11014 , 33  / 255. , 102 / 255. , 172 / 255.  , "color0012", 1)) 
mycolors.append(r.TColor(11015 , 5   / 255. , 48  / 255. , 97  / 255.  , "color0013", 1)) 

mycolors.append(r.TColor(3001  , 239 / 255. , 138 / 255. , 98  / 255.  , "color0014", 1)) 
mycolors.append(r.TColor(3000  , 247 / 255. , 247 / 255. , 247 / 255.  , "color0015", 1)) 
mycolors.append(r.TColor(3011  , 103 / 255. , 169 / 255. , 207 / 255.  , "color0016", 1)) 

mycolors.append(r.TColor(5001  , 251 / 255. , 180 / 255. , 174 / 255.  , "color0017", 1)) 
mycolors.append(r.TColor(5002  , 179 / 255. , 205 / 255. , 227 / 255.  , "color0020", 1)) 
mycolors.append(r.TColor(5003  , 204 / 255. , 235 / 255. , 197 / 255.  , "color0021", 1)) 
mycolors.append(r.TColor(5004  , 222 / 255. , 203 / 255. , 228 / 255.  , "color0022", 1)) 
mycolors.append(r.TColor(5005  , 254 / 255. , 217 / 255. , 166 / 255.  , "color0023", 1)) 

mycolors.append(r.TColor(7000  , 0/255.     , 0/255.     , 0/255.      , "color0024", 1)) 
mycolors.append(r.TColor(7001  , 213/255.   , 94/255.    , 0/255.      , "color0025", 1)) #r
mycolors.append(r.TColor(7002  , 230/255.   , 159/255.   , 0/255.      , "color0026", 1)) #o
mycolors.append(r.TColor(7003  , 240/255.   , 228/255.   , 66/255.     , "color0027", 1)) #y
mycolors.append(r.TColor(7004  , 0/255.     , 158/255.   , 115/255.    , "color0030", 1)) #g
mycolors.append(r.TColor(7005  , 0/255.     , 114/255.   , 178/255.    , "color0031", 1)) #b
mycolors.append(r.TColor(7006  , 86/255.    , 180/255.   , 233/255.    , "color0032", 1)) #k
mycolors.append(r.TColor(7007  , 204/255.   , 121/255.   , 167/255.    , "color0033", 1)) #p
mycolors.append(r.TColor(7011  , 110/255.   , 54/255.    , 0/255.      , "color0034", 1)) #alt r
mycolors.append(r.TColor(7012  , 161/255.   , 117/255.   , 0/255.      , "color0035", 1)) #alt o
mycolors.append(r.TColor(7013  , 163/255.   , 155/255.   , 47/255.     , "color0036", 1)) #alt y
mycolors.append(r.TColor(7014  , 0/255.     , 102/255.   , 79/255.     , "color0037", 1)) #alt g
mycolors.append(r.TColor(7015  , 0/255.     , 93/255.    , 135/255.    , "color0040", 1)) #alt b
mycolors.append(r.TColor(7016  , 153/255.   , 153/255.   , 153/255.    , "color0041", 1)) #alt k
mycolors.append(r.TColor(7017  , 140/255.   , 93/255.    , 119/255.    , "color0042", 1)) #alt p

mycolors.append(r.TColor(9001  , 60/255.    , 186/255.   , 84/255.     , "color0043", 1)) 
mycolors.append(r.TColor(9002  , 244/255.   , 194/255.   , 13/255.     , "color0044", 1)) 
mycolors.append(r.TColor(9003  , 219/255.   , 50/255.    , 54/255.     , "color0045", 1)) 
mycolors.append(r.TColor(9004  , 72/255.    , 133/255.   , 237/255.    , "color0046", 1)) 

mycolors.append(r.TColor(2001  , 91  / 255. , 187 / 255. , 241 / 255.  , "color0047", 1)) #light-blue
mycolors.append(r.TColor(2002  , 60  / 255. , 144 / 255. , 196 / 255.  , "color0050", 1)) #blue
mycolors.append(r.TColor(2003  , 230 / 255. , 159 / 255. , 0   / 255.  , "color0051", 1)) #orange
mycolors.append(r.TColor(2004  , 180 / 255. , 117 / 255. , 0   / 255.  , "color0052", 1)) #brown
mycolors.append(r.TColor(2005  , 245 / 255. , 236 / 255. , 69  / 255.  , "color0053", 1)) #yellow
mycolors.append(r.TColor(2006  , 215 / 255. , 200 / 255. , 0   / 255.  , "color0054", 1)) #dark yellow
mycolors.append(r.TColor(2007  , 70  / 255. , 109 / 255. , 171 / 255.  , "color0055", 1)) #blue-violet
mycolors.append(r.TColor(2008  , 70  / 255. , 90  / 255. , 134 / 255.  , "color0056", 1)) #violet
mycolors.append(r.TColor(2009  , 55  / 255. , 65  / 255. , 100 / 255.  , "color0057", 1)) #dark violet
mycolors.append(r.TColor(2010  , 120 / 255. , 160 / 255. , 0   / 255.  , "color0060", 1)) #light green
mycolors.append(r.TColor(2011  , 0   / 255. , 158 / 255. , 115 / 255.  , "color0061", 1)) #green
mycolors.append(r.TColor(2012  , 204 / 255. , 121 / 255. , 167 / 255.  , "color0062", 1)) #pink?

mycolors.append(r.TColor(4001  , 49  / 255. , 76  / 255. , 26  / 255.  , "color0063", 1)) 
mycolors.append(r.TColor(4002  , 33  / 255. , 164 / 255. , 105  / 255. , "color0064", 1)) 
mycolors.append(r.TColor(4003  , 176 / 255. , 224 / 255. , 160 / 255.  , "color0065", 1)) 
mycolors.append(r.TColor(4004  , 210 / 255. , 245 / 255. , 200 / 255.  , "color0066", 1)) 
mycolors.append(r.TColor(4005  , 232 / 255. , 249 / 255. , 223 / 255.  , "color0067", 1)) 
mycolors.append(r.TColor(4006  , 253 / 255. , 156 / 255. , 207 / 255.  , "color0070", 1)) 
mycolors.append(r.TColor(4007  , 121 / 255. , 204 / 255. , 158 / 255.  , "color0071", 1)) 
mycolors.append(r.TColor(4008  , 158 / 255. , 0 / 255.   , 42 / 255.   , "color0072", 1)) 
mycolors.append(r.TColor(4009  , 176 / 255. , 0 / 255.   , 195 / 255.  , "color0073", 1)) 
mycolors.append(r.TColor(4010  , 20 / 255.  , 195 / 255. , 0 / 255.    , "color0074", 1)) 
mycolors.append(r.TColor(4011  , 145 / 255. , 2 / 255.   , 206 / 255.  , "color0075", 1)) 
mycolors.append(r.TColor(4012  , 255 / 255. , 0 / 255.   , 255 / 255.  , "color0076", 1)) 
mycolors.append(r.TColor(4013  , 243 / 255. , 85 / 255.  , 0 / 255.    , "color0077", 1)) 
mycolors.append(r.TColor(4014  , 157 / 255. , 243 / 255. , 130 / 255.  , "color0100", 1)) 
mycolors.append(r.TColor(4015  , 235 / 255. , 117 / 255. , 249 / 255.  , "color0101", 1)) 
mycolors.append(r.TColor(4016  , 90 / 255.  , 211 / 255. , 221 / 255.  , "color0102", 1)) 
mycolors.append(r.TColor(4017  , 85 / 255.  , 181 / 255. , 92 / 255.   , "color0103", 1)) 
mycolors.append(r.TColor(4018  , 172 / 255. , 50 / 255.  , 60 / 255.   , "color0104", 1)) 
mycolors.append(r.TColor(4019  , 42 / 255.  , 111 / 255. , 130 / 255.  , "color0105", 1)) 

mycolors.append(r.TColor(4020  , 240 / 255. , 155 / 255. , 205 / 255.  , "color0106", 1)) # ATLAS pink
mycolors.append(r.TColor(4021  , 77 / 255.  , 161 / 255. , 60 / 255.   , "color0107", 1)) # ATLAS green
mycolors.append(r.TColor(4022  , 87 / 255.  , 161 / 255. , 247 / 255.  , "color0110", 1)) # ATLAS blue
mycolors.append(r.TColor(4023  , 196 / 255. , 139 / 255. , 253 / 255.  , "color0111", 1)) # ATLAS darkpink
mycolors.append(r.TColor(4024  , 205 / 255. , 240 / 255. , 155 / 255.  , "color0112", 1)) # Complementary

mycolors.append(r.TColor(4101  , 102 / 255. , 102 / 255. , 204 / 255.  , "color0113", 1)) # ATLAS HWW / WW
mycolors.append(r.TColor(4102  , 89 / 255.  , 185 / 255. , 26 / 255.   , "color0114", 1)) # ATLAS HWW / DY
mycolors.append(r.TColor(4103  , 225 / 255. , 91 / 255.  , 226 / 255.  , "color0115", 1)) # ATLAS HWW / VV
mycolors.append(r.TColor(4104  , 103 / 255. , 236 / 255. , 235 / 255.  , "color0116", 1)) # ATLAS HWW / misid

mycolors.append(r.TColor(4201  , 16 / 255.  , 220 / 255. , 138 / 255.  , "color0117", 1)) # Signal complementary

mycolors.append(r.TColor(4305  , 0/255.     , 208/255.   , 145/255.    , "color0120", 1)) # green made up

mycolors.append(r.TColor(6001  , 24/255.    , 43/255.    , 73/255.     , "color0121", 1)) # UCSD Dark   Blue   Pantone 2767
mycolors.append(r.TColor(6002  , 0/255.     , 98/255.    , 155/255.    , "color0122", 1)) # UCSD Ocean  Blue   Pantone 3015
mycolors.append(r.TColor(6003  , 198/255.   , 146/255.   , 20/255.     , "color0123", 1)) # UCSD        Kelp   Pantone 1245
mycolors.append(r.TColor(6004  , 255/255.   , 205/255.   , 0/255.      , "color0124", 1)) # UCSD Bright Gold   Pantone 116
mycolors.append(r.TColor(6005  , 0/255.     , 198/255.   , 215/255.    , "color0125", 1)) # UCSD        Cyan   Pantone 3115
mycolors.append(r.TColor(6006  , 110/255.   , 150/255.   , 59/255.     , "color0126", 1)) # UCSD        Green  Pantone 7490
mycolors.append(r.TColor(6007  , 243/255.   , 229/255.   , 0/255.      , "color0127", 1)) # UCSD Bright Yellow Pantone 3945
mycolors.append(r.TColor(6008  , 252/255.   , 137/255.   , 0/255.      , "color0130", 1)) # UCSD        Orange Pantone 144


default_colors = []
default_colors.append(2005)
default_colors.append(2001)
default_colors.append(2003)
default_colors.append(2007)
default_colors.append(920)
default_colors.extend(range(2001, 2013))
default_colors.extend(range(7001, 7018))




#______________________________________________________________________________________________________________________
def makedir(dirpath):
    try:
        os.makedirs(dirpath)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(dirpath):
            pass
        else:
            raise


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
        print("ERROR - the number of histograms are zero, while you asked me to sum them up.")
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
        print("ERROR - the number of histograms are zero, while you asked me to sum them up.")
    totalhist = get_total_hist(hists)
    errhist = cloneTH1(totalhist)
    errhist.Reset()
    for i in range(0, totalhist.GetNbinsX() + 2):
        errhist.SetBinContent(i, totalhist.GetBinError(i))
    return errhist

#______________________________________________________________________________________________________________________
def normalize_by_first_bin(hists):
    def func(hist):
        norm = hist.GetBinContent(1)
        if norm != 0:
            hist.Scale(1./norm)
    if isinstance(hists, list):
        for hist in hists:
            func(hist)
    else:
        func(hists)
    return hists

#______________________________________________________________________________________________________________________
def add_diff_to_error(nomhist, errhist, errhistpairvar=None):
    """
    Add the difference between nomhist to errhist as an additional error to nomhist
    """
    if nomhist.GetNbinsX() != errhist.GetNbinsX(): print("ERROR - the nom hist and err hist have different dimension in X")
    if nomhist.GetNbinsY() != errhist.GetNbinsY(): print("ERROR - the nom hist and err hist have different dimension in Y")
    if nomhist.GetNbinsZ() != errhist.GetNbinsZ(): print("ERROR - the nom hist and err hist have different dimension in Z")

    if errhistpairvar:
        if nomhist.GetNbinsX() != errhistpairvar.GetNbinsX(): print("ERROR - the nom hist and err hist paired variation have different dimension in X")
        if nomhist.GetNbinsY() != errhistpairvar.GetNbinsY(): print("ERROR - the nom hist and err hist paired variation have different dimension in Y")
        if nomhist.GetNbinsZ() != errhistpairvar.GetNbinsZ(): print("ERROR - the nom hist and err hist paired variation have different dimension in Z")

    labels = nomhist.GetXaxis().GetLabels()
    if labels:
        nomhist.GetXaxis().SetRange(1, nomhist.GetXaxis().GetNbins())
        nomhist.GetYaxis().SetRange(1, nomhist.GetYaxis().GetNbins())
        nomhist.GetZaxis().SetRange(1, nomhist.GetZaxis().GetNbins())
        nomhist.GetXaxis().SetCanExtend(False)
        nomhist.GetYaxis().SetCanExtend(False)
        nomhist.GetZaxis().SetCanExtend(False)
        errhist.GetXaxis().SetRange(1, errhist.GetXaxis().GetNbins())
        errhist.GetYaxis().SetRange(1, errhist.GetYaxis().GetNbins())
        errhist.GetZaxis().SetRange(1, errhist.GetZaxis().GetNbins())
        errhist.GetXaxis().SetCanExtend(False)
        errhist.GetYaxis().SetCanExtend(False)
        errhist.GetZaxis().SetCanExtend(False)

    for iz in range(0, nomhist.GetNbinsZ()+2):
        for iy in range(0, nomhist.GetNbinsY()+2):
            for ix in range(0, nomhist.GetNbinsX()+2):
                nombc = nomhist.GetBinContent(ix, iy, iz)
                nombe = nomhist.GetBinError(ix, iy, iz)
                errbc = errhist.GetBinContent(ix, iy, iz)
                diff = nombc - errbc
                if errhistpairvar:
                    errbcpaired = errhistpairvar.GetBinContent(ix, iy, iz)
                    diffpaired = nombc - errbcpaired
                    if abs(diff) < abs(diffpaired):
                        diff = diffpaired
                newb = E(0, diff) + E(nombc, nombe)
                #print(newb.val, newb.err, diff, nombe, nombc)
                nomhist.SetBinContent(ix, iy, iz, newb.val)
                nomhist.SetBinError(ix, iy, iz, newb.err)
            if nomhist.GetDimension() == 1:
                return
        if nomhist.GetDimension() == 2:
            return

#______________________________________________________________________________________________________________________
def getYaxisRange(hist):
    maximum = 0
    if hist:
        for ibin in range(0, hist.GetNbinsX()+2):
        #for ibin in range(1, hist.GetNbinsX()+1):
            c = hist.GetBinContent(ibin)
            e = hist.GetBinError(ibin)
            v = c + e
            if v > maximum:
                maximum = v
    return maximum

#______________________________________________________________________________________________________________________
def getYaxisNonZeroMin(hist):
    minimum = 999999999999999999
    if hist:
        for ibin in range(1, hist.GetNbinsX()+1):
        #for ibin in range(1, hist.GetNbinsX()+1):
            c = hist.GetBinContent(ibin)
            e = hist.GetBinError(ibin)
            v = c + e
            if float(v) != float(0):
                if abs(v) < minimum:
                    minimum = abs(v)
    if minimum == 999999999999999999:
        minimum = 0.1
    return minimum

#______________________________________________________________________________________________________________________
def get_nonzeromin_yaxis_range(hists):
    minimum = 9999999999999999999
    for hist in hists:
        v = getYaxisNonZeroMin(hist)
        if v < minimum:
            minimum = v
    if minimum == 9999999999999999999:
        minimum = 0.1
    return minimum

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
    try:
        firstdigit = int(str(maximum)[0])
        maximum = max(maximum, 0.001)
        order = int(math.log10(maximum))
        if firstdigit <= 2:
            middle = (10.**(order - 1))
        else:
            middle = (10.**(order))
        return maximum + middle
    except:
        return "BLAH"

#______________________________________________________________________________________________________________________
def remove_errors(hists):
    for hist in hists:
        for ibin in range(0, hist.GetNbinsX()+2):
            hist.SetBinError(ibin, 0)

#______________________________________________________________________________________________________________________
def rebin(hists, nbin):
    for hist in hists:
        if not hist: continue
        currnbin = hist.GetNbinsX()
        fac = currnbin / nbin
        if float(fac).is_integer() and fac > 0:
            hist.Rebin(int(fac))

#______________________________________________________________________________________________________________________
def single_divide_by_bin_width(hist):
    for ibin in range(1,hist.GetNbinsX()+2):
        hist.SetBinContent(ibin, hist.GetBinContent(ibin) / hist.GetBinWidth(ibin))
        hist.SetBinError(ibin, hist.GetBinError(ibin) / hist.GetBinWidth(ibin))

#______________________________________________________________________________________________________________________
def divide_by_bin_width(hists):
    for hist in hists:
        single_divide_by_bin_width(hist)

#______________________________________________________________________________________________________________________
def flatten_th2(th2):
    nx = th2.GetNbinsX()
    ny = th2.GetNbinsY()
    th1 = r.TH1F(th2.GetName(), th2.GetTitle(), nx*ny, 0, nx*ny)
    for ix in range(nx):
        for iy in range(ny):
            bc = th2.GetBinContent(ix+1, iy+1)
            be = th2.GetBinError(ix+1, iy+1)
            #th1.SetBinContent(ix+1+(iy)*nx, bc)
            #th1.SetBinError(ix+1+(iy)*nx, be)
            th1.SetBinContent(iy+1+(ix)*ny, bc)
            th1.SetBinError(iy+1+(ix)*ny, be)
    return th1

#______________________________________________________________________________________________________________________
def remove_underflow(hists):
    def func(hist):
        hist.SetBinContent(0, 0)
        hist.SetBinError(0, 0)
    if isinstance(hists, list):
        for hist in hists:
            func(hist)
    else:
        func(hists)
    return hists

#______________________________________________________________________________________________________________________
def remove_overflow(hists):
    def func(hist):
        hist.SetBinContent(hist.GetNbinsX()+1, 0)
        hist.SetBinError(hist.GetNbinsX()+1, 0)
    if isinstance(hists, list):
        for hist in hists:
            func(hist)
    else:
        func(hists)
    return hists

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
            for i in range(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs)
                hist.SetBinError(i, be * nfs)
        elif len(nfs) == hist.GetNbinsX():
            for i in range(1, hist.GetNbinsX()+1):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs[i-1])
                hist.SetBinError(i, be * nfs[i-1])
        elif len(nfs) == hist.GetNbinsX()+2:
            for i in range(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs[i])
                hist.SetBinError(i, be * nfs[i])
        elif len(nfs) == 1:
            for i in range(0, hist.GetNbinsX()+2):
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
            for i in range(0, hist.GetNbinsX()+2):
                bc = hist.GetBinContent(i)
                be = hist.GetBinError(i)
                hist.SetBinContent(i, bc * nfs)
                hist.SetBinError(i, be * nfs)
        elif len(nfs) == hist.GetNbinsX():
            for i in range(1, hist.GetNbinsX()+1):
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
            for i in range(0, hist.GetNbinsX()+2):
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
            for i in range(0, hist.GetNbinsX()+2):
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
            for i in range(0, hist.GetNbinsX()+2):
                for j in range(0, hist.GetNbinsY()+2):
                    bc = hist.GetBinContent(i, j)
                    be = hist.GetBinError(i, j)
                    nf = nfs[0][0]
                    hist.SetBinContent(i, j, bc * nf)
                    hist.SetBinError(i, j, be * nf)
        elif len(nfs) == hist.GetNbinsX():
            for i in range(1, hist.GetNbinsX()+1):
                for j in range(0, hist.GetNbinsY()+2):
                    bc = hist.GetBinContent(i, j)
                    be = hist.GetBinError(i, j)
                    nf = nfs[i-1][0]
                    hist.SetBinContent(i, j, bc * nf)
                    hist.SetBinError(i, j, be * nf)
        else:
            print("WARNING - apply_nf_w_error_2d: something went wrong.")
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
            for i in range(0, hist.GetNbinsX()+2):
                for j in range(0, hist.GetNbinsY()+2):
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
            for i in range(1, hist.GetNbinsX()+1):
                for j in range(0, hist.GetNbinsY()+2):
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
            print("WARNING - apply_nf_w_error_2d: something went wrong.")
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
# 95% CL limit
def fom_limit(s, serr, b, berr, totals, totalb):
    global RooUtil_StatUtil_Loaded
    if os.path.exists("{0}/rooutil.so".format(os.path.realpath(__file__).rsplit("/",1)[0])) and not RooUtil_StatUtil_Loaded:
        r.gSystem.Load("{0}/rooutil.so".format(os.path.realpath(__file__).rsplit("/",1)[0]))
        r.gROOT.ProcessLine(".L {0}/rooutil.h".format(os.path.realpath(__file__).rsplit("/",1)[0]))
        RooUtil_StatUtil_Loaded = True

    if b > 0:
        print(s, b, 1. / r.RooUtil.StatUtil.cut_and_count_95percent_limit(s, b, berr / b), 0)
        return 1. / r.RooUtil.StatUtil.cut_and_count_95percent_limit(s, b, berr / b), 0
    else:
        return 0, 0

#______________________________________________________________________________________________________________________
# S / sqrt(B) fom
def fom_SoverB(s, serr, b, berr, totals, totalb):
    if b > 0:
        return s / b, 0
    else:
        return 0, 0

#______________________________________________________________________________________________________________________
# S / sqrt(B) fom
def fom_SoverSqrtSPlusB(s, serr, b, berr, totals, totalb):
    if s + b > 0:
        return s / math.sqrt(s + b), 0
    else:
        return 0, 0

#______________________________________________________________________________________________________________________
# S / sqrt(B) fom
def fom_SoverSqrtB(s, serr, b, berr, totals, totalb):
    if b > 0 and s > 0:
        # return s / math.sqrt(b), 0
        return math.sqrt(2 * ((s + b) * math.log(1 + s / b) - s)), 0
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
#def plot_sigscan2d(sig, bkg, fom=fom_SoverSqrtB):
def plot_sigscan2d(sig, bkg, fom=fom_SoverB):
    nbin = sig.GetNbinsX()
    if nbin != bkg.GetNbinsX():
        print("Error - significance scan for the signal and background histograms have different size", nbin, bkg.GetNbinsX())
    scan = cloneTH1(sig)
    scan.Reset()
    xmin = scan.GetXaxis().GetBinLowEdge(1)
    xwidth = scan.GetXaxis().GetBinWidth(1)
    max_f = 0
    max_f_cut_low = 0
    max_f_cut_high = 0
    totalsig = sig.Integral(0, nbin + 1)
    totalbkg = bkg.Integral(0, nbin + 1)

    for i in range(1, nbin + 1):
        local_max_f = 0
        local_max_f_err = 0
        for j in range(i + 1, nbin + 1):
            sigerr = c_double(0)
            sigint = sig.IntegralAndError(i, j, sigerr)
            bkgerr = c_double(0)
            bkgint = bkg.IntegralAndError(i, j, bkgerr)
            sigerr = sigerr.value
            bkgerr = bkgerr.value
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
    scan.SetName("{:.2f} ({:.2f},{:.2f})".format(max_f, max_f_cut_low, max_f_cut_high))
    return scan

#______________________________________________________________________________________________________________________
# For each signal and total background return scan from left/right of fom (figure of merit) func.
def plot_sigscan(sig, bkg, fom=fom_SoverSqrtB):
# def plot_sigscan(sig, bkg, fom=fom_limit):
# def plot_sigscan(sig, bkg, fom=fom_SoverB):
# def plot_sigscan(sig, bkg, fom=fom_SoverSqrtSPlusB):
    nbin = sig.GetNbinsX()
    if nbin != bkg.GetNbinsX():
        print("Error - significance scan for the signal and background histograms have different size", nbin, bkg.GetNbinsX())
    leftscan = cloneTH1(sig)
    leftscan.Reset()
    xmin = leftscan.GetXaxis().GetBinLowEdge(1)
    xwidth = leftscan.GetXaxis().GetBinWidth(1)
    max_f = 0
    max_f_cut = 0
    totalsig = sig.Integral(0, nbin + 1)
    totalbkg = bkg.Integral(0, nbin + 1)
    # print(totalsig, totalbkg)
    for i in range(1, nbin + 1):
        sigerr = c_double(0)
        sigint = sig.IntegralAndError(i, nbin + 1, sigerr)
        bkgerr = c_double(0)
        bkgint = bkg.IntegralAndError(i, nbin + 1, bkgerr)
        sigerr = sigerr.value
        bkgerr = bkgerr.value
        f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
        leftscan.SetBinContent(i, f)
        leftscan.SetBinError(i, ferr)
        if max_f < f:
            if fom == fom_acceptance:
                if f <= 0.98:
                    max_f = f
                    max_f_cut = xmin + xwidth * (i - 1)
            else:
                max_f = f
                max_f_cut = xmin + xwidth * (i - 1)
    # print(max_f)
    leftscan.SetName("#rightarrow {:.2f} ({:.2f})".format(max_f, max_f_cut))
    rightscan = cloneTH1(sig)
    rightscan.Reset()
    max_f = 0
    max_f_cut = 0
    for i in reversed(range(1, nbin + 1)):
        sigerr = c_double(0)
        sigint = sig.IntegralAndError(0, i, sigerr)
        bkgerr = c_double(0)
        bkgint = bkg.IntegralAndError(0, i, bkgerr)
        sigerr = sigerr.value
        bkgerr = bkgerr.value
        f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
        rightscan.SetBinContent(i, f)
        rightscan.SetBinError(i, ferr)
        if max_f < f:
            if fom == fom_acceptance:
                if f <= 0.98:
                    max_f = f
                    max_f_cut = xmin + xwidth * i
            else:
                max_f = f
                max_f_cut = xmin + xwidth * i
    rightscan.SetName("#leftarrow {:.2f} ({:.2f})".format(max_f, max_f_cut))
    return leftscan, rightscan

#______________________________________________________________________________________________________________________
# For each signal and indvidiual background plus systematics
def plot_sigscan_w_syst(sig, bkgs, systs, fom=fom_SoverSqrtBwErr):

    bkg = get_total_hist(bkgs)

    if len(bkgs) != len(systs) and len(systs) > 0:
        print("Error - The provided systs list does not have the same number of entries as the bkgs", bkgs, systs)

    nbin = sig.GetNbinsX()
    if nbin != bkg.GetNbinsX():
        print("Error - significance scan for the signal and background histograms have different size", nbin, bkg.GetNbinsX())
    leftscan = cloneTH1(sig)
    leftscan.Reset()
    xmin = leftscan.GetXaxis().GetBinLowEdge(1)
    xwidth = leftscan.GetXaxis().GetBinWidth(1)
    max_f = -999
    max_f_cut = 0
    totalsig = sig.Integral(0, nbin + 1)
    totalbkg = bkg.Integral(0, nbin + 1)
    sigaccept = 0
    for i in range(1, nbin + 1):
        sigerr = c_double(0)
        sigint = sig.IntegralAndError(i, nbin + 1, sigerr)
        bkgerr = c_double(0)
        bkgint = bkg.IntegralAndError(i, nbin + 1, bkgerr)
        sigerr = sigerr.value
        bkgerr = bkgerr.value
        count_s = E(sigint, sigerr)
        count_b = E(bkgint, bkgerr)
        counts = []
        for index, bg in enumerate(bkgs):
            e = c_double(0)
            c = bg.IntegralAndError(i, nbin + 1, e)
            e = e.value
            ne = math.sqrt(e*e + c*systs[index]*c*systs[index])
            counts.append(E(c, ne))
        count_b_w_syst = E(0, 0)
        for count in counts:
            count_b_w_syst = count_b_w_syst + count
        bkgerr = count_b_w_syst.err
        f, ferr = fom(sigint, sigerr, bkgint, bkgerr, totalsig, totalbkg)
        #print(i, f)
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
    for i in reversed(range(1, nbin + 1)):
        sigerr = c_double(0)
        sigint = sig.IntegralAndError(0, i, sigerr)
        bkgerr = c_double(0)
        bkgint = bkg.IntegralAndError(0, i, bkgerr)
        sigerr = sigerr.value
        bkgerr = bkgerr.value
        count_s = E(sigint, sigerr)
        count_b = E(bkgint, bkgerr)
        counts = []
        for index, bg in enumerate(bkgs):
            e = c_double(0)
            c = bg.IntegralAndError(0, i, e)
            e = e.value
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
def human_readable_sample_name(name):
    tmpname = name.replace("t#bar{t}", "tt")
    tmpname = tmpname.replace("W^{#pm}W^{#pm}", "ssWW")
    tmpname = tmpname.replace("^{#pm}", "")
    return tmpname

#______________________________________________________________________________________________________________________
def human_format(num):
    is_fraction = False
    if num < 1:
        is_fraction = True
    magnitude = 0
    while abs(num) >= 1000:
        magnitude += 1
        num /= 1000.0
    # add more suffixes if you need them
    if is_fraction:
        return '%.2g%s' % (num, ['', 'K', 'M', 'G', 'T', 'P'][magnitude])
    else:
        return '%.3g%s' % (num, ['', 'K', 'M', 'G', 'T', 'P'][magnitude])

#______________________________________________________________________________________________________________________
def yield_str(hist, i, prec=3, noerror=False, options={}):
    if prec == 0 and noerror:
        return str(int(hist.GetBinContent(i)))
    tmpval = hist.GetBinContent(i)
    precuse = prec
    if noerror:
        return "{{:.{}g}}".format(precuse).format(hist.GetBinContent(i))
    else:
        e = E(hist.GetBinContent(i), hist.GetBinError(i))
        if "human_format" in options:
            if options["human_format"]:
                # sep = u"\u00B1".encode("utf-8")
                sep = u'\u00B1'
                return "%s %s %s" % (human_format(e.val), sep, human_format(e.err))
            else:
                return e.round(precuse)
        else:
            # return e.round(precuse)
            # sep = u"\u00B1".encode("utf-8")
            sep = u'\u00B1'
            return "%s %s %s" % ('{{:.{}g}}'.format(precuse).format(e.val), sep, '{{:.{}g}}'.format(precuse).format(e.err))
#______________________________________________________________________________________________________________________
def yield_tex_str(hist, i, prec=3, noerror=False):
    tmp = yield_str(hist, i, prec, noerror)
    tmp = tmp.__str__()
    sep = '\xc2\xb1'
    tmp = tmp.replace(sep, "$\pm$")
    return tmp

#______________________________________________________________________________________________________________________
def print_yield_table_from_list(hists, outputname, prec=2, binrange=[], noerror=False, options={}):
    x = Table()
    if len(hists) == 0:
        return
    # add bin column
    labels = hists[0].GetXaxis().GetLabels()
    if "print_yield_bin_indices" in options:
        bins = options["print_yield_bin_indices"]
    else:
        bins = binrange if len(binrange) != 0 else (range(1, hists[0].GetNbinsX()+1) if labels else range(0, hists[0].GetNbinsX()+2))
    if labels:
        x.add_column("Bin#", [ hists[0].GetXaxis().GetBinLabel(i) for i in bins])
    else:
        x.add_column("Bin#", ["Bin{}".format(i) for i in bins])
    for hist in hists:
        x.add_column(hist.GetName(), [ yield_str(hist, i, prec, noerror, options) for i in bins])
    fname = outputname
    fname = os.path.splitext(fname)[0]+'.txt'
    x.print_table()
    x.set_theme_basic()

    # Write text version
    makedir(os.path.dirname(fname))
    f = open(fname, "w")
    f.write("".join(x.get_table_string()))

#______________________________________________________________________________________________________________________
def print_bin_label_tex_style(rstr):
    hasmath = False
    if "^" in rstr: hasmath = True
    if "{" in rstr: hasmath = True
    if "}" in rstr: hasmath = True
    if "(" in rstr: hasmath = True
    if ")" in rstr: hasmath = True
    if "#" in rstr:
        rstr = rstr.replace("#", "\\")
    if hasmath:
        return "$"+rstr+"$"
    else:
        return rstr

#______________________________________________________________________________________________________________________
def print_yield_tex_table_from_list(hists, outputname, prec=2, caption="PUT YOUR CAPTION HERE", noerror=False, content_only=True):
    x = Table()
    if len(hists) == 0:
        return
    # add bin column
    labels = hists[0].GetXaxis().GetLabels()
    if labels:
        x.add_column("", [print_bin_label_tex_style(hists[0].GetXaxis().GetBinLabel(i)) for i in range(1, hists[0].GetNbinsX()+1)])
    else:
        x.add_column("", ["Bin{}".format(i) for i in range(1, hists[0].GetNbinsX()+1)])
    for hist in hists:
        # print(name)
        name = hist.GetTitle()
        if name == "":
            name = hist.GetName()
        if '#' in name:
            name = name.replace("#", "\\")
            name = "$" + name + "$"
        if name == "data":
            name = "Data"
        x.add_column(name, [ yield_tex_str(hist, i, prec, noerror) for i in range(1, hist.GetNbinsX()+1)])
    fname = outputname
    fname = os.path.splitext(fname)[0]+'.tex'
    x.set_theme_basic()

    # Change style for easier tex conversion
    x.d_style["INNER_INTERSECT"] = ''
    x.d_style["OUTER_RIGHT_INTERSECT"] = ''
    x.d_style["OUTER_BOTTOM_INTERSECT"] = ''
    x.d_style["OUTER_BOTTOM_LEFT"] = ''
    x.d_style["OUTER_BOTTOM_RIGHT"] = ''
    x.d_style["OUTER_TOP_INTERSECT"] = ''
    x.d_style["OUTER_TOP_LEFT"] = ''
    x.d_style["OUTER_TOP_RIGHT"] = ''
    x.d_style["INNER_HORIZONTAL"] = ''
    x.d_style["OUTER_BOTTOM_HORIZONTAL"] = ''
    x.d_style["OUTER_TOP_HORIZONTAL"] = ''

    x.d_style["OUTER_LEFT_VERTICAL"] = ''
    x.d_style["OUTER_RIGHT_VERTICAL"] = ''

#        self.d_style["INNER_HORIZONTAL"] = '-'
#        self.d_style["INNER_INTERSECT"] = '+'
#        self.d_style["INNER_VERTICAL"] = '|'
#        self.d_style["OUTER_LEFT_INTERSECT"] = '|'
#        self.d_style["OUTER_RIGHT_INTERSECT"] = '+'
#        self.d_style["OUTER_BOTTOM_HORIZONTAL"] = '-'
#        self.d_style["OUTER_BOTTOM_INTERSECT"] = '+'
#        self.d_style["OUTER_BOTTOM_LEFT"] = '+'
#        self.d_style["OUTER_BOTTOM_RIGHT"] = '+'
#        self.d_style["OUTER_TOP_HORIZONTAL"] = '-'
#        self.d_style["OUTER_TOP_INTERSECT"] = '+'
#        self.d_style["OUTER_TOP_LEFT"] = '+'
#        self.d_style["OUTER_TOP_RIGHT"] = '+'

    content = [ x for x in ("".join(x.get_table_string())).split('\n') if len(x) > 0 ]

    # Write tex from text version table
    f = open(fname, 'w')
    content = tabletex.makeTableTeX(content, complete=False)
    header = """\\begin{table}[!htb]
\\caption{"""
    header += caption
    header +="""}
\\resizebox{1.0\\textwidth}{!}{
"""
    footer = """}
\\end{table}
"""
    if not content_only:
        f.write(header)
    f.write(content)
    if not content_only:
        f.write(footer)

#______________________________________________________________________________________________________________________
def print_yield_tex_table_from_list_v2(hists_summary, hists_individ, outputname, prec=2, caption="PUT YOUR CAPTION HERE", noerror=False, content_only=True):
    x = Table()
    if len(hists_summary) == 0:
        return
    # add bin column
    labels = hists_summary[0].GetXaxis().GetLabels()
    if labels:
        x.add_column("", [print_bin_label_tex_style(hists_summary[0].GetXaxis().GetBinLabel(i)) for i in range(1, hists_summary[0].GetNbinsX()+1)])
    else:
        x.add_column("", ["Bin{}".format(i) for i in range(1, hists_summary[0].GetNbinsX()+1)])
    for hist in hists_summary + hists_individ:
        # print(name)
        name = hist.GetTitle()
        if name == "":
            name = hist.GetName()
        if '#' in name:
            name = name.replace("#", "\\")
            name = "$" + name + "$"
        if name == "data":
            name = "Data"
        if name == "Total":
            name = "\\Ntotal"
        x.add_column(name, [ yield_tex_str(hist, i, 0 if name == "\\Nobs" else prec, True if name == "\\Nobs" else  noerror) for i in range(1, hist.GetNbinsX()+1)])
    fname = outputname
    fname = os.path.splitext(fname)[0]+'.tex'
    x.set_theme_basic()

    # Change style for easier tex conversion
    x.d_style["INNER_INTERSECT"] = ''
    x.d_style["OUTER_RIGHT_INTERSECT"] = ''
    x.d_style["OUTER_BOTTOM_INTERSECT"] = ''
    x.d_style["OUTER_BOTTOM_LEFT"] = ''
    x.d_style["OUTER_BOTTOM_RIGHT"] = ''
    x.d_style["OUTER_TOP_INTERSECT"] = ''
    x.d_style["OUTER_TOP_LEFT"] = ''
    x.d_style["OUTER_TOP_RIGHT"] = ''
    x.d_style["INNER_HORIZONTAL"] = ''
    x.d_style["OUTER_BOTTOM_HORIZONTAL"] = ''
    x.d_style["OUTER_TOP_HORIZONTAL"] = ''

    x.d_style["OUTER_LEFT_VERTICAL"] = ''
    x.d_style["OUTER_RIGHT_VERTICAL"] = ''

#        self.d_style["INNER_HORIZONTAL"] = '-'
#        self.d_style["INNER_INTERSECT"] = '+'
#        self.d_style["INNER_VERTICAL"] = '|'
#        self.d_style["OUTER_LEFT_INTERSECT"] = '|'
#        self.d_style["OUTER_RIGHT_INTERSECT"] = '+'
#        self.d_style["OUTER_BOTTOM_HORIZONTAL"] = '-'
#        self.d_style["OUTER_BOTTOM_INTERSECT"] = '+'
#        self.d_style["OUTER_BOTTOM_LEFT"] = '+'
#        self.d_style["OUTER_BOTTOM_RIGHT"] = '+'
#        self.d_style["OUTER_TOP_HORIZONTAL"] = '-'
#        self.d_style["OUTER_TOP_INTERSECT"] = '+'
#        self.d_style["OUTER_TOP_LEFT"] = '+'
#        self.d_style["OUTER_TOP_RIGHT"] = '+'

    content = [ x for x in ("".join(x.get_table_string())).split('\n') if len(x) > 0 ]

# \multirow{2}{*}{\onZCR}           & \multicolumn{2}{c}{Summary}                             & \multicolumn{7}{c}{Composition of \Ntotal}                                                                             & Purity (\%)    & \multirow{2}{*}{\SF{\ZZ}}  \\  \cline{2-3}\cline{4-10}\cline{11-11}
    customheader = "\\multirow{2}{*}{NAME} & \multicolumn{"+str(len(hists_summary))+"}{c}{Summary} & \multicolumn{"+str(len(hists_individ))+"}{c}{Composition of \\Ntotal} \\\\ \\cline{2-"+str(len(hists_summary)-1+2)+"}\\cline{"+str(len(hists_summary)+2)+"-"+str(len(hists_individ)-1+len(hists_summary)+2)+"}"

    # Write tex from text version table
    f = open(fname, 'w')
    content = tabletex.makeTableTeX(content, complete=False, customheader=customheader)
    header = """\\begin{table}[!htb]
\\caption{"""
    header += caption
    header +="""}
\\resizebox{1.0\\textwidth}{!}{
"""
    footer = """}
\\end{table}
"""
    if not content_only:
        f.write(header)
    f.write(content)
    if not content_only:
        f.write(footer)

#______________________________________________________________________________________________________________________
def print_yield_table(hdata, hbkgs, hsigs, hsyst, options):
    hists = []
    hists.extend(hbkgs)
    htotal = None
    if len(hbkgs) != 0:
        htotal = get_total_hist(hbkgs)
        htotal.SetName("Total")
        htotal.SetTitle("Total")
        hists.append(htotal)
    if hdata and len(hbkgs) != 0:
        #print(hdata)
        #hratio = makeRatioHist(hdata, hbkgs)
        hratio = hdata.Clone("Ratio")
        hratio.SetTitle("Ratio")
        hratio.Divide(htotal)
        #hists.append(htotal)
        hists.append(hdata)
        hists.append(hratio)
    hists.extend(hsigs)
    prec = 2
    if "yield_prec" in options:
        prec = options["yield_prec"]
        del options["yield_prec"]
    print_yield_table_from_list(hists, options["output_name"], prec, options=options)

    # Re arranging for tex
    histstex = []

    histstexsummary = []
    histstexindivid = []
    if len(hbkgs) == 0:
        histstexsummary = [] # do nothing
    else:
        if hdata:
            histstexsummary = [hists[-(len(hsigs)+1)], hists[-(len(hsigs)+2)], hists[-(len(hsigs)+3)]] # ratio data total
            histstexsummary[0].SetTitle("\\Nobs / \\Ntotal")
            histstexsummary[1].SetTitle("\\Nobs")
            histstexsummary[2].SetTitle("\\Ntotal")
        else:
            histstexsummary = [hists[-(len(hsigs)+1)]] # just total
    histstexsummary.extend(hsigs)
    histstexindivid.extend(hbkgs)
    print_yield_tex_table_from_list_v2(histstexsummary, histstexindivid, options["output_name"], prec, options["yield_table_caption"] if "yield_table_caption" in options else "PUT YOUR CAPTION HERE")
    if "yield_table_caption" in options: del options["yield_table_caption"]

def copy_nice_plot_index_php(options):
    plotdir = os.path.dirname(options["output_name"])
    if len(plotdir) == 0: plotdir = "./"
    os.system("cp {}/../misc/index.php {}/".format(os.path.realpath(__file__).rsplit("/",1)[0], plotdir))
    os.system("chmod 755 {}/index.php".format(plotdir))
#    os.system("cp {}/syncfiles/miscfiles/index.php {}/".format(os.path.realpath(__file__).rsplit("/",1)[0], plotdir))

def copy_nice_plot(plotdir):
    os.system("cp {}/syncfiles/miscfiles/index.php {}/".format(os.path.realpath(__file__).rsplit("/",1)[0], plotdir))

#______________________________________________________________________________________________________________________
def autobin(data, bgs):
    totalbkg = get_total_hist(bgs)

    accumulative = totalbkg.Clone("accumul")
    norm = accumulative.Integral() if accumulative.Integral() > 0 else 1
    accumulative.Scale(1. / norm)
    idx5 = -1
    idx95 = -1
    for i in range(1, accumulative.GetNbinsX()+2):
        intg = accumulative.Integral(0, i)
        if intg > 0.02 and idx5 < 0:
            idx5 = i
        if intg > 0.98 and idx95 < 0:
            idx95 = i
    minbin = -1
    if data:
        for i in range(idx5, idx95):
            bc = data.GetBinContent(i)
            if bc < minbin or minbin < 0:
                minbin = bc
    ndata = int(totalbkg.Integral(idx5, idx95))
    if ndata > 0:
        nbin = int(1 + 3.322 * math.log10(ndata))
    else:
        nbin = 4
    width = idx95 - idx5 + 1
    if data:
        frac = float(width) / float(data.GetNbinsX())
    else:
        frac = 1
    final_nbin = int(nbin / frac)
    if data:
        while data.GetNbinsX() % final_nbin != 0:
            if data.GetNbinsX() < final_nbin:
                return 4
            final_nbin += 1
    return final_nbin

# ====================
# The plottery wrapper
# ====================

#______________________________________________________________________________________________________________________
def plot_hist(data=None, bgs=[], sigs=[], syst=None, options={}, colors=[], sig_labels=[], legend_labels=[]):
    """
    Wrapper function to call Plottery.
    """

    # Set can extend turned off if label exists
    for h in bgs + sigs + [data] + [syst]:
        if h:
            labels = h.GetXaxis().GetLabels()
            if labels:
                h.SetCanExtend(False)

    # If print_all true, print all histogram content
    if "print_all" in options:
        if options["print_all"]:
            for bg in bgs: bg.Print("all")
            for sig in sigs: sig.Print("all")
            if data: data.Print("all")
        del options["print_all"]

    # Sanity check. If no histograms exit
    if not data and len(bgs) == 0 and len(sigs) == 0:
        print("[plottery_wrapper] >>> Nothing to do!")
        return

    # If a blind option is set, blind the data histogram to None
    # The later step will set the histogram of data to all zero
    if "blind" in options:
        if options["blind"]:
            data = None
            options["no_ratio"] = True
        del options["blind"]

    # signal scaling
    if "signal_scale" in options:
        if options["signal_scale"] == "auto":
            integral = get_total_hist(bgs).Integral()
            for sig in sigs:
                if sig.Integral() != 0:
                    sig.Scale(integral/sig.Integral())
                    sig.SetName(sig.GetName() + " [norm]")
            del options["signal_scale"]
        else:
            for sig in sigs:
                sig.Scale(options["signal_scale"])
                if options["signal_scale"] != 1:
                    if "hide_signal_scale" in options:
                        if options["hide_signal_scale"]:
                            pass
                        else:
                            sig.SetName(sig.GetName() + " [{:.2f}x]".format(float(options["signal_scale"])))
                    else:
                        sig.SetName(sig.GetName() + " [{:.2f}x]".format(float(options["signal_scale"])))
            del options["signal_scale"]
            if "hide_signal_scale" in options:
                del options["hide_signal_scale"]

    if "hide_signal_scale" in options:
        del options["hide_signal_scale"]

    # autobin
    if "autobin" in options and options["autobin"]:
        options["nbins"] = autobin(data, bgs)
        del options["autobin"]
    elif "autobin" in options:
        del options["autobin"]

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

    if "remove_underflow" in options:
        if options["remove_underflow"]:
            remove_underflow(sigs)
            remove_underflow(bgs)
            if data:
                remove_underflow([data])
        del options["remove_underflow"]

    if "remove_overflow" in options:
        if options["remove_overflow"]:
            remove_overflow(sigs)
            remove_overflow(bgs)
            if data:
                remove_overflow([data])
        del options["remove_overflow"]

    if "divide_by_first_bin" in options:
        if options["divide_by_first_bin"]:
            normalize_by_first_bin(sigs)
            normalize_by_first_bin(bgs)
            if data:
                normalize_by_first_bin([data])
        del options["divide_by_first_bin"]

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
    hsig_labels = sig_labels
    if len(sig_labels) == 0:
        for hsig in sigs:
            hsig_labels.append(hsig.GetName())
    hcolors = colors
    if len(colors) == 0:
        for index, hbg in enumerate(bgs):
            hcolors.append(default_colors[index])
    hlegend_labels = legend_labels
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
    yaxismax = get_max_yaxis_range_order_half_modded(get_max_yaxis_range([data, totalbkg] + sigs) * maxmult)
    if yaxismax == "BLAH":
        print(data.GetName())
        print(options)
        data.Print("all")
        for bg in bgs:
            bg.Print("all")
        for sg in sigs:
            sg.Print("all")
    yaxismin = get_nonzeromin_yaxis_range(bgs)
    #yaxismin = 1000

    if "yaxis_log" in options:
        if options["yaxis_log"] and ("yaxis_range" not in options or options["yaxis_range"] == []):
            options["yaxis_range"] = [yaxismin, 10000*(yaxismax-yaxismin)+yaxismax]
            print([yaxismin, 10000*(yaxismax-yaxismin)+yaxismax])

    # scale background to fit
    if "fit_bkg" in options:
        if options["fit_bkg"]:
            if not didnothaveanydata:
                btoterr = c_double()
                btot = totalbkg.IntegralAndError(0, totalbkg.GetNbinsX()+1, btoterr)
                dtoterr = c_double()
                dtot = data.IntegralAndError(0, data.GetNbinsX()+1, dtoterr)
                btoterr = btoterr.value
                dtoterr = dtoterr.value
                if btot != 0 and dtot != 0:
                    sf = dtot/btot
                    sferr = sf * math.sqrt((dtoterr / dtot)**2 + (btoterr / btot)**2)
                    for bg in bgs:
                        bg.Scale(sf)
                    options["extra_text"] = ["SF={:.2f}#pm{:.2f}".format(sf, sferr)]
                else:
                    print("Warning: fit_bkg option did nothing as either btot == 0 or dtot == 0")
        del options["fit_bkg"]

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

    if "bin_labels" in options:
        # Set can extend turned off if label exists
        for h in bgs + sigs + [data] + [syst]:
            if h:
                nbins = h.GetNbinsX()
                nlabels = len(options["bin_labels"])
                if nbins != nlabels:
                    print("Error: the bin_labels length do not match the histogram nbinsx")
                    continue
                else:
                    for i in range(nlabels):
                        h.GetXaxis().SetBinLabel(i + 1, options["bin_labels"][i])
                    h.SetCanExtend(False)
                    if "bin_labels_orientation" in options:
                        h.LabelsOption(options["bin_labels_orientation"])
                    else:
                        h.LabelsOption("h")
        if "bin_labels_orientation" in options:
            del options["bin_labels_orientation"]
        del options["bin_labels"]

    # Print yield table if the option is turned on
    if "print_yield" in options:
        if options["print_yield"]:
            print_yield_table(None if didnothaveanydata else data, bgs, sigs, syst, options)
        del options["print_yield"]

    if "human_format" in options:
        del options["human_format"]

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
                for i in range(1, data.GetNbinsX() + 1):
                    data.SetBinError(i, 0)
                options["legend_datalabel"] = "Sig+Bkg"
        del options["inject_signal"]

    # do KS test and add it to extra_text
    if "do_ks_test" in options:
        if options["do_ks_test"]:
            ksval = totalbkg.KolmogorovTest(data)
            options["extra_text"] = ["KS={:.2f}".format(ksval)]
        del options["do_ks_test"]

    # do smoothing
    if "do_smooth" in options:
        if options["do_smooth"]:
            for hsig in sigs:
                hsig.Smooth()
            for hbkg in bgs:
                hbkg.Smooth()
        del options["do_smooth"]

    if "print_mean" in options:
        if options["print_mean"]:
            mean = totalbkg.GetMean()
            try:
                options["extra_text"].append("mean={:.2f}".format(mean))
            except:
                options["extra_text"] = ["mean={:.2f}".format(mean)]
        del options["print_mean"]

    # If syst is not provided, compute one yourself from the bkg histograms
    if not syst:
        syst = get_total_err_hist(bgs)

    # The uncertainties are all accounted in the syst so remove all errors from bkgs
    remove_errors(bgs)

    # Get xaxis label from data, sig or bkg
    allhists = []
    for bg in bgs:
        allhists.append(bg)
    for sig in sigs:
        allhists.append(sig)
    if data:
        allhists.append(data)
    xaxis_label = allhists[0].GetXaxis().GetTitle()

    if "yaxis_range" in options and options["yaxis_range"] == []:
        del options["yaxis_range"]

    # Here are my default options for plottery
    #if not "canvas_width"             in options: options["canvas_width"]              = 604
    #if not "canvas_height"            in options: options["canvas_height"]             = 728
    if not "yaxis_log"                      in options: options["yaxis_log"]                      = False
    if not "canvas_width"                   in options: options["canvas_width"]                   = 454
    if not "canvas_height"                  in options: options["canvas_height"]                  = 553
    if not "yaxis_range"                    in options: options["yaxis_range"]                    = [0., yaxismax]
    if not "legend_ncolumns"                in options: options["legend_ncolumns"]                = 2 if len(bgs) >= 4 else 1
    if not "legend_alignment"               in options: options["legend_alignment"]               = "topright"
    #if not "legend_smart"                   in options: options["legend_smart"]                   = True if not options["yaxis_log"] else False
    if not "legend_smart"                   in options: options["legend_smart"]                   = True
    if not "legend_scalex"                  in options: options["legend_scalex"]                  = 0.8
    if not "legend_scaley"                  in options: options["legend_scaley"]                  = 0.8
    if not "legend_border"                  in options: options["legend_border"]                  = False
    if not "legend_rounded"                 in options: options["legend_rounded"]                 = False
    if not "legend_percentageinbox"         in options: options["legend_percentageinbox"]         = False
    if not "legend_opacity"                 in options: options["legend_opacity"]                 = 1
    if not "hist_line_none"                 in options: options["hist_line_none"]                 = False
    if not "hist_line_black"                in options: options["hist_line_black"]                = True
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
    if not "ratio_ndivisions"               in options: options["ratio_ndivisions"]               = 508
    if not "max_digits"                     in options: options["max_digits"]                     = 4
    if not "xaxis_label"                    in options: options["xaxis_label"]                    = xaxis_label
    if not "ratio_xaxis_title"              in options: options["ratio_xaxis_title"]              = xaxis_label
    if data == None:
        options["no_ratio"] = True
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

    # Set permission
    os.system("chmod 644 {}".format(options["output_name"]))

    options["output_name"] = options["output_name"].replace("pdf","png")
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

    # Set permission
    os.system("chmod 644 {}".format(options["output_name"]))

    # Call nice plots
    copy_nice_plot_index_php(options)

#______________________________________________________________________________________________________________________
def plot_cut_scan(data=None, bgs=[], sigs=[], syst=None, options={}, colors=[], sig_labels=[], legend_labels=[]):
    hsigs = []
    hbgs = []
    if syst:
        leftscan, rightscan = plot_sigscan_w_syst(sigs[0].Clone(), [bg.Clone() for bg in bgs], systs=syst)
    else:
        leftscan, rightscan = plot_sigscan(sigs[0].Clone(), get_total_hist(bgs).Clone())
    # leftscan.Print("all")
    if leftscan.GetBinContent(1) != 0:
        leftscan.Scale(1./leftscan.GetBinContent(1))
    if rightscan.GetBinContent(rightscan.GetNbinsX()) != 0:
        rightscan.Scale(1./rightscan.GetBinContent(rightscan.GetNbinsX()))
    leftscan.SetFillStyle(1)
    hbgs.append(leftscan.Clone())
    hsigs.append(rightscan.Clone())
    scan2d = plot_sigscan2d(sigs[0].Clone(), get_total_hist(bgs).Clone())
    if scan2d.GetBinContent(1) != 0:
        scan2d.Scale(1./scan2d.GetBinContent(1))
    # hsigs.append(scan2d.Clone())
    leftscan, rightscan = plot_sigscan(sigs[0].Clone(), get_total_hist(bgs).Clone(), fom_acceptance)
    hsigs.append(leftscan.Clone())
    hsigs.append(rightscan.Clone())
    options["bkg_err_fill_color"] = 0
    options["output_name"] = options["output_name"].replace(".png", "_cut_scan.png")
    options["output_name"] = options["output_name"].replace(".pdf", "_cut_scan.pdf")
    options["signal_scale"] = 1
    if "nbins" in options:
        del options["nbins"]
    plot_hist(data=None, sigs=hsigs, bgs=hbgs, syst=None, options=options, colors=colors, sig_labels=sig_labels, legend_labels=legend_labels)

#______________________________________________________________________________________________________________________
def plot_soverb_scan(data=None, bgs=[], sigs=[], syst=None, options={}, colors=[], sig_labels=[], legend_labels=[]):
    hsigs = []
    hbgs = []
    if syst:
        leftscan, rightscan = plot_sigscan_w_syst(sigs[0].Clone(), [bg.Clone() for bg in bgs], systs=syst)
    else:
        leftscan, rightscan = plot_sigscan(sigs[0].Clone(), get_total_hist(bgs).Clone(), fom=fom_SoverB)
    # leftscan.Print("all")
    if leftscan.GetBinContent(1) != 0:
        leftscan.Scale(1./leftscan.GetBinContent(1))
    if rightscan.GetBinContent(rightscan.GetNbinsX()) != 0:
        rightscan.Scale(1./rightscan.GetBinContent(rightscan.GetNbinsX()))
    leftscan.SetFillStyle(1)
    hbgs.append(leftscan.Clone())
    hsigs.append(rightscan.Clone())
    scan2d = plot_sigscan2d(sigs[0].Clone(), get_total_hist(bgs).Clone())
    if scan2d.GetBinContent(1) != 0:
        scan2d.Scale(1./scan2d.GetBinContent(1))
    hsigs.append(scan2d.Clone())
    leftscan, rightscan = plot_sigscan(sigs[0].Clone(), get_total_hist(bgs).Clone(), fom_acceptance)
    hsigs.append(leftscan.Clone())
    hsigs.append(rightscan.Clone())
    options["bkg_err_fill_color"] = 0
    options["output_name"] = options["output_name"].replace(".png", "_cut_scan.png")
    options["output_name"] = options["output_name"].replace(".pdf", "_cut_scan.pdf")
    options["signal_scale"] = 1
    plot_hist(data=None, sigs=hsigs, bgs=hbgs, syst=None, options=options, colors=colors, sig_labels=sig_labels, legend_labels=legend_labels)

#______________________________________________________________________________________________________________________
def plot_roc(fps=[],tps=[],legend_labels=[],colors=[],cutvals=[],scanreverse=[],options={},_persist=[]):

    #opts = Options(options, kind="graph")

    #style = utils.set_style()

    #c1 = r.TCanvas()
    #if opts["canvas_width"] and opts["canvas_height"]:
    #    width = opts["canvas_width"]
    #    height = opts["canvas_height"]
    #    c1 = r.TCanvas("c1", "c1", width, height)
    #_persist.append(c1) # need this to avoid segfault with garbage collection

    #pad_main = r.TPad("pad1","pad1",0.,0.,1.,1.)
    #if opts["canvas_main_topmargin"]: pad_main.SetTopMargin(opts["canvas_main_topmargin"])
    #if opts["canvas_main_rightmargin"]: pad_main.SetRightMargin(opts["canvas_main_rightmargin"])
    #if opts["canvas_main_bottommargin"]: pad_main.SetBottomMargin(opts["canvas_main_bottommargin"])
    #if opts["canvas_main_leftmargin"]: pad_main.SetLeftMargin(opts["canvas_main_leftmargin"])
    #if opts["canvas_tick_one_side"]: pad_main.SetTicks(0, 0)
    #pad_main.Draw()

    #pad_main.cd()

    map(u.move_in_overflows, tps)
    map(u.move_in_overflows, fps)

    #legend = get_legend(opts)

    # generalize later
    if len(tps) != len(fps):
        print(len(tps), len(fps))
        print(">>> number of true positive hists and false positive hists must match")
        sys.exit(-1)

    debug = False

    ## do your thing
    valpairs = []
    pointpairs = []
    ref_seff = 0
    ref_beff = 0
    for index, _ in enumerate(tps):

        sighist = tps[index]
        bkghist = fps[index]
        cutval = cutvals[index] if len(cutvals) == len(tps) else -999

        if debug: print("[DEBUG] >>> here", sighist.GetName(), bkghist.GetName())

        error = c_double() # TODO: THIS IS BUGGY!

        stot = sighist.IntegralAndError(0, sighist.GetNbinsX()+1, error)
        btot = bkghist.IntegralAndError(0, bkghist.GetNbinsX()+1, error)

        if debug: print('[DEBUG] >>>', stot, btot)
        if debug: print('[DEBUG] >>> sighist.GetMean()', sighist.GetMean())
        if debug: print('[DEBUG] >>> bkghist.GetMean()', bkghist.GetMean())

        x=[]
        y=[]
        cuteffset = False

        for i in range(0, sighist.GetNbinsX()+2):
            if len(scanreverse) > 0:
                doreverse = scanreverse[index]
            else:
                doreverse = False
            s = sighist.IntegralAndError(sighist.GetNbinsX()-i, sighist.GetNbinsX()+1, error)
            b = bkghist.IntegralAndError(sighist.GetNbinsX()-i, bkghist.GetNbinsX()+1, error)
            if doreverse:
                s = sighist.IntegralAndError(0, 1 + i, error.value)
                b = bkghist.IntegralAndError(0, 1 + i, error.value)
            #s = sighist.IntegralAndError(0, i, error)
            #b = bkghist.IntegralAndError(0, i, error)
            seff = s / stot
            beff = b / btot
            curval = sighist.GetXaxis().GetBinUpEdge(sighist.GetNbinsX()) - i * sighist.GetXaxis().GetBinWidth(1)
            if doreverse:
                curval = sighist.GetXaxis().GetBinUpEdge(1 + i)
            print(seff, beff, curval)
#            if abs(ref_seff - seff) < 0.03:
#                print abs(ref_seff - seff) < 0.03
#                print ref_seff
#                print cuteffset
#                print cutval == -999, cutval
            if abs(ref_seff - seff) < 0.01 and ref_seff > 0 and not cuteffset and cutval == -999:
#                print 'here'
                cuteffset = True
                legend_labels[index] = "({0:.2f}, {1:.4f}) @ {2} ".format(seff, beff, curval) + legend_labels[index] if len(legend_labels[index]) > 0 else ""
                pointpairs.append(([beff], [seff]))
            if not doreverse:
                if curval <= cutval and not cuteffset:
                    legend_labels[index] = "({0:.2f}, {1:.4f}) @ {2} ".format(seff, beff, curval) + legend_labels[index] if len(legend_labels[index]) > 0 else ""
                    pointpairs.append(([beff], [seff]))
                    cuteffset = True
                    if ref_seff == 0: ref_seff = seff
                    if ref_beff == 0: ref_beff = beff
            else:
                if curval >= cutval and not cuteffset:
                    legend_labels[index] = "({0:.2f}, {1:.4f}) @ {2} ".format(seff, beff, curval) + legend_labels[index] if len(legend_labels[index]) > 0 else ""
                    pointpairs.append(([beff], [seff]))
                    cuteffset = True
                    if ref_seff == 0: ref_seff = seff
                    if ref_beff == 0: ref_beff = beff
            if debug:
                if abs(sighist.GetBinLowEdge(i) - 0.25) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(sighist.GetBinLowEdge(i) - 0.15) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(sighist.GetBinLowEdge(i) - 0.10) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(sighist.GetBinLowEdge(i) - 0.07) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(beff - 0.07) < 0.02: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(beff - 0.04) < 0.02: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(seff - 0.91) < 0.02: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
            #if beff != 0:
            #    print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff), s, b, stot, btot)
            x.append(beff)
            y.append(seff)

        valpairs.append((x,y))

        #graph = ROOT.TGraph(len(x))
        #for index, i in enumerate(x):
        #    graph.SetPoint(index, x[index], y[index])

        #graph.SetTitle(legend_labels[index])
        #graph.SetName(legend_labels[index])
        #graph.SetMinimum(0.)
        #graph.SetMaximum(1)
        ##graph.GetXaxis().SetRangeUser(0.05,1)
        #graph.SetLineColor(colors[index])
        #graph.SetLineWidth(colors[index])
        ##graph.GetXaxis().SetTitle("Eff Background")
        ##graph.GetYaxis().SetTitle("Eff Signal")
        ##self.histmanager.set_histaxis_settings(graph, 1.0)
        ##from copy import deepcopy
        #graphs.append(graph)
        ##self.objs.append(deepcopy(graph))

    #ymin, ymax = 0., 1. # generally ROC curves are always between 0. to 1.

    #for index, graph in enumerate(graphs):
    #    if index == 0:
    #        if opts["yaxis_range"]:
    #            graph.SetMinimum(opts["yaxis_range"][0])
    #            graph.SetMaximum(opts["yaxis_range"][1])
    #            ymin, ymax = opts["yaxis_range"]
    #        graph.SetMinimum(ymin)
    #        graph.SetMaximum(ymax)
    #        graph.Draw("alp")
    #    else:
    #        graph.Draw("lp")

    #draw_cms_lumi(pad_main, opts)
    #handle_axes(pad_main, stack, opts)
    #draw_extra_stuff(pad_main, opts)

    ## ymin ymax needs to be defined
    #if opts["legend_smart"]:
    #    utils.smart_legend(legend, bgs, data=data, ymin=ymin, ymax=ymax, opts=opts)
    #legend.Draw()

    #save(c1, opts)
    if not "legend_alignment"               in options: options["legend_alignment"]               = "bottomright"
    if not "legend_scalex"                  in options: options["legend_scalex"]                  = 1.5
    if not "legend_scaley"                  in options: options["legend_scaley"]                  = 0.8
    if not "legend_border"                  in options: options["legend_border"]                  = False

    valpairs.extend(pointpairs)

    draw_styles=[]
    for i in colors: draw_styles.append(1)

    colors.extend(colors)

    ll = []
    for x in legend_labels:
        if len(x) > 0:
            ll.append(x)
    legend_labels = ll

    c1 = p.plot_graph(valpairs, colors=colors, legend_labels=legend_labels, options=options, draw_styles=draw_styles)

    copy_nice_plot_index_php(options)

    return c1


#______________________________________________________________________________________________________________________
def plot_roc_v1(fps=[],tps=[],legend_labels=[],colors=[],cutvals=[],scanreverse=[],options={},_persist=[]):

    #opts = Options(options, kind="graph")

    #style = utils.set_style()

    #c1 = r.TCanvas()
    #if opts["canvas_width"] and opts["canvas_height"]:
    #    width = opts["canvas_width"]
    #    height = opts["canvas_height"]
    #    c1 = r.TCanvas("c1", "c1", width, height)
    #_persist.append(c1) # need this to avoid segfault with garbage collection

    #pad_main = r.TPad("pad1","pad1",0.,0.,1.,1.)
    #if opts["canvas_main_topmargin"]: pad_main.SetTopMargin(opts["canvas_main_topmargin"])
    #if opts["canvas_main_rightmargin"]: pad_main.SetRightMargin(opts["canvas_main_rightmargin"])
    #if opts["canvas_main_bottommargin"]: pad_main.SetBottomMargin(opts["canvas_main_bottommargin"])
    #if opts["canvas_main_leftmargin"]: pad_main.SetLeftMargin(opts["canvas_main_leftmargin"])
    #if opts["canvas_tick_one_side"]: pad_main.SetTicks(0, 0)
    #pad_main.Draw()

    #pad_main.cd()

    map(u.move_in_overflows, tps)
    map(u.move_in_overflows, fps)

    #legend = get_legend(opts)

    # generalize later
    if len(tps) != len(fps):
        print(len(tps), len(fps))
        print(">>> number of true positive hists and false positive hists must match")
        sys.exit(-1)

    debug = False

    ## do your thing
    valpairs = []
    pointpairs = []
    ref_seff = 0
    ref_beff = 0
    for index, _ in enumerate(tps):

        sighist = tps[index]
        bkghist = fps[index]
        cutval = cutvals[index] if len(cutvals) == len(tps) else -999

        if debug: print("[DEBUG] >>> here", sighist.GetName(), bkghist.GetName())

        error = c_double() # TODO: THIS IS BUGGY!

        stot = sighist.IntegralAndError(0, sighist.GetNbinsX()+1, error)
        btot = bkghist.IntegralAndError(0, bkghist.GetNbinsX()+1, error)

        if debug: print('[DEBUG] >>>', stot, btot)
        if debug: print('[DEBUG] >>> sighist.GetMean()', sighist.GetMean())
        if debug: print('[DEBUG] >>> bkghist.GetMean()', bkghist.GetMean())

        x=[]
        y=[]
        cuteffset = False

        for i in range(0, sighist.GetNbinsX()+2):
            if len(scanreverse) > 0:
                doreverse = scanreverse[index]
            else:
                doreverse = False
            s = sighist.IntegralAndError(sighist.GetNbinsX()-i, sighist.GetNbinsX()+1, error)
            b = bkghist.IntegralAndError(sighist.GetNbinsX()-i, bkghist.GetNbinsX()+1, error)
            if doreverse:
                s = sighist.IntegralAndError(0, 1 + i, error.value)
                b = bkghist.IntegralAndError(0, 1 + i, error.value)
            #s = sighist.IntegralAndError(0, i, error)
            #b = bkghist.IntegralAndError(0, i, error)
            seff = s / stot
            beff = b / btot
            curval = sighist.GetXaxis().GetBinUpEdge(sighist.GetNbinsX()) - i * sighist.GetXaxis().GetBinWidth(1)
            if doreverse:
                curval = sighist.GetXaxis().GetBinUpEdge(1 + i)
            print(seff, beff, curval)
#            if abs(ref_seff - seff) < 0.03:
#                print(abs(ref_seff - seff) < 0.03)
#                print(ref_seff)
#                print(cuteffset)
#                print(cutval == -999, cutval)
            if abs(ref_seff - seff) < 0.01 and ref_seff > 0 and not cuteffset and cutval == -999:
#                print('here')
                cuteffset = True
                legend_labels[index] = "({0:.2f}, {1:.4f}) @ {2} ".format(seff, beff, curval) + legend_labels[index] if len(legend_labels[index]) > 0 else ""
                pointpairs.append(([beff], [seff]))
            if not doreverse:
                if curval <= cutval and not cuteffset:
                    legend_labels[index] = "({0:.2f}, {1:.4f}) @ {2} ".format(seff, beff, curval) + legend_labels[index] if len(legend_labels[index]) > 0 else ""
                    pointpairs.append(([beff], [seff]))
                    cuteffset = True
                    if ref_seff == 0: ref_seff = seff
                    if ref_beff == 0: ref_beff = beff
            else:
                if curval >= cutval and not cuteffset:
                    legend_labels[index] = "({0:.2f}, {1:.4f}) @ {2} ".format(seff, beff, curval) + legend_labels[index] if len(legend_labels[index]) > 0 else ""
                    pointpairs.append(([beff], [seff]))
                    cuteffset = True
                    if ref_seff == 0: ref_seff = seff
                    if ref_beff == 0: ref_beff = beff
            if debug:
                if abs(sighist.GetBinLowEdge(i) - 0.25) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(sighist.GetBinLowEdge(i) - 0.15) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(sighist.GetBinLowEdge(i) - 0.10) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(sighist.GetBinLowEdge(i) - 0.07) < 0.01: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(beff - 0.07) < 0.02: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(beff - 0.04) < 0.02: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
                if abs(seff - 0.91) < 0.02: print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff))
            #if beff != 0:
            #    print(seff, beff, sighist.GetBinLowEdge(i), seff*seff / math.sqrt(beff), seff / math.sqrt(beff), s, b, stot, btot)
            x.append(beff)
            y.append(seff)

        valpairs.append((x,y))

        #graph = ROOT.TGraph(len(x))
        #for index, i in enumerate(x):
        #    graph.SetPoint(index, x[index], y[index])

        #graph.SetTitle(legend_labels[index])
        #graph.SetName(legend_labels[index])
        #graph.SetMinimum(0.)
        #graph.SetMaximum(1)
        ##graph.GetXaxis().SetRangeUser(0.05,1)
        #graph.SetLineColor(colors[index])
        #graph.SetLineWidth(colors[index])
        ##graph.GetXaxis().SetTitle("Eff Background")
        ##graph.GetYaxis().SetTitle("Eff Signal")
        ##self.histmanager.set_histaxis_settings(graph, 1.0)
        ##from copy import deepcopy
        #graphs.append(graph)
        ##self.objs.append(deepcopy(graph))

    #ymin, ymax = 0., 1. # generally ROC curves are always between 0. to 1.

    #for index, graph in enumerate(graphs):
    #    if index == 0:
    #        if opts["yaxis_range"]:
    #            graph.SetMinimum(opts["yaxis_range"][0])
    #            graph.SetMaximum(opts["yaxis_range"][1])
    #            ymin, ymax = opts["yaxis_range"]
    #        graph.SetMinimum(ymin)
    #        graph.SetMaximum(ymax)
    #        graph.Draw("alp")
    #    else:
    #        graph.Draw("lp")

    #draw_cms_lumi(pad_main, opts)
    #handle_axes(pad_main, stack, opts)
    #draw_extra_stuff(pad_main, opts)

    ## ymin ymax needs to be defined
    #if opts["legend_smart"]:
    #    utils.smart_legend(legend, bgs, data=data, ymin=ymin, ymax=ymax, opts=opts)
    #legend.Draw()

    #save(c1, opts)
    if not "legend_alignment"               in options: options["legend_alignment"]               = "bottomright"
    if not "legend_scalex"                  in options: options["legend_scalex"]                  = 1.5
    if not "legend_scaley"                  in options: options["legend_scaley"]                  = 0.8
    if not "legend_border"                  in options: options["legend_border"]                  = False

    valpairs.extend(pointpairs)

    draw_styles=[]
    for i in colors: draw_styles.append(1)

    colors.extend(colors)

    ll = []
    for x in legend_labels:
        if len(x) > 0:
            ll.append(x)
    legend_labels = ll

    c1 = p.plot_graph(valpairs, colors=colors, legend_labels=legend_labels, options=options, draw_styles=draw_styles)

    copy_nice_plot_index_php(options)

    return c1


#______________________________________________________________________________________________________________________
def plot_hist_2d(hist,options={}):
    p.plot_hist_2d(hist, options)
    options["output_name"] = options["output_name"].replace("pdf","png")
    p.plot_hist_2d(hist, options)

#______________________________________________________________________________________________________________________
def dump_plot_v1(fname, dirname="plots"):

    f = r.TFile(fname)
    
    hists = {}
    for key in f.GetListOfKeys():
        hists[key.GetName()] = f.Get(key.GetName())
    
    fn = fname.replace(".root", "")
    for hname in hists:
        if hists[hname].GetDimension() == 1:
            plot_hist(bgs=[hists[hname]], options={"output_name": dirname + "/" + fn + "_" + hname + ".pdf"})
        if hists[hname].GetDimension() == 2:
            plot_hist_2d(hist=hists[hname], options={"output_name": dirname + "/" + fn + "_" + hname + ".pdf"})

#______________________________________________________________________________________________________________________
def dump_plot(
        fnames=[],
        sig_fnames=[],
        data_fname=None,
        dirname="plots",
        legend_labels=[],
        legend_labels_tex=[],
        signal_labels=None,
        signal_labels_tex=None,
        donorm=False,
        filter_pattern="",
        signal_scale=1,
        extraoptions={},
        usercolors=None,
        do_sum=False,
        output_name=None,
        dogrep=False,
        _plotter=plot_hist,
        doKStest=False,
        histmodfunc=None,
        histxaxislabeloptions={},
        skip2d=False,
        data_syst=None,
        bkg_syst=None,
        sig_syst=None,
        ):

    # color_pallete
    colors_ = default_colors
    if usercolors:
        colors_ = usercolors + default_colors

    # Open all files and define color schemes
    sample_names = []
    tfs = {}
    clrs = {}
    issig = [] # Aggregate a list of signal samples
    isbkg = [] # Aggregate a list of signal samples
    for index, fname in enumerate(fnames + sig_fnames):
        n = os.path.basename(fname.replace(".root", ""))
        n += str(index)
        tfs[n] = r.TFile(fname)
        clrs[n] = colors_[index]
        sample_names.append(n)
        if index >= len(fnames):
            issig.append(n)
        else:
            isbkg.append(n)

    if data_fname:
        n = os.path.basename(data_fname.replace(".root", ""))
        tfs[n] = r.TFile(data_fname)
        clrs[n] = colors_[index]
        sample_names.append(n)
    
    # Tag the data sample names
    data_sample_name = None
    if data_fname:
        n = os.path.basename(data_fname.replace(".root", ""))
        data_sample_name = n

    # Form a complete key list
    hist_names = []
    for n in tfs:
        for key in tfs[n].GetListOfKeys():

            keyname = str(key.GetName())

            # If to filter certain histograms
            if filter_pattern:
                if dogrep:
                    doskip = True
                    for item in filter_pattern.split(","):
                        if "*" in item:
                            match = True
                            for token in item.split("*"):
                                if token not in keyname:
                                    match = False
                                    break
                            if match:
                                doskip = False
                                break
                        else:
                            if item in keyname:
                                doskip = False
                                break
                    if doskip:
                        continue
                else:
                    doskip = True
                    for item in filter_pattern.split(","):
                        if keyname == item:
                            doskip = False
                            break
                    if doskip:
                        continue

            if "TH" in tfs[n].Get(str(key.GetName())).ClassName():
                hist_names.append(keyname)

    # Remove duplicate names
    hist_names = list(set(hist_names))

    # Sort
    hist_names.sort()

    # summed_hist if do_sum is true
    summed_hist = []

    # Loop over hist_names
    for hist_name in hist_names:

        # If to filter certain histograms
        if filter_pattern:
            if dogrep:
                doskip = True
                for item in filter_pattern.split(","):
                    if "*" in item:
                        match = True
                        for token in item.split("*"):
                            if token not in hist_name:
                                match = False
                                break
                        if match:
                            doskip = False
                            break
                    else:
                        if item in hist_name:
                            doskip = False
                            break
                if doskip:
                    continue
            else:
                doskip = True
                for item in filter_pattern.split(","):
                    if hist_name == item:
                        doskip = False
                        break
                if doskip:
                    continue

        hists = []
        colors = []
        for n in sample_names:

            # If there is a syst suffix defined
            hist_name_to_get = hist_name
            if n in isbkg and bkg_syst: hist_name_to_get = hist_name_to_get.replace("__", bkg_syst+"__")
            if n in issig and sig_syst: hist_name_to_get = hist_name_to_get.replace("__", sig_syst+"__")
            if n == data_sample_name and data_syst: hist_name_to_get = hist_name_to_get.replace("__", data_syst+"__")

            h = tfs[n].Get(hist_name_to_get)

            if h:
                if n in issig:
                    if signal_labels:
                        h.SetName(signal_labels[issig.index(n)])
                    else:
                        h.SetName(n)
                    if signal_labels_tex:
                        h.SetTitle(signal_labels_tex[issig.index(n)])
                else:
                    if len(legend_labels) > 0:
                        try:
                            hrsn = human_readable_sample_name(legend_labels[sample_names.index(n)])
                            h.SetName(hrsn)
                        except:
                            h.SetName(n)
                        if len(legend_labels_tex) > 0:
                            try:
                                # print(n)
                                # print(sample_names.index(n))
                                # print(legend_labels_tex)
                                texn = legend_labels_tex[sample_names.index(n)]
                                h.SetTitle(texn)
                            except:
                                h.SetTitle(n)
                    else:
                        h.SetName(n)
                hists.append(h)
                colors.append(clrs[n])
            else:
                print("ERROR: did not find histogram", hist_name_to_get, "for the file", tfs[n].GetName())
                sys.exit(1)

        if do_sum:

            if len(summed_hist) > 0:

                for index, h in enumerate(hists):

                    summed_hist[index].Add(h)

            else:

                for h in hists:

                    summed_hist.append(h.Clone())

        else:

            if output_name:
                hist_name = output_name

            if len(hists) > 0:
                if hists[0].GetDimension() == 1:
                    if donorm: # shape comparison so use sigs to overlay one bkg with multiple shape comparison
                        options = {"signal_scale": "auto", "output_name": dirname + "/" + hist_name + ".pdf"}
                        options.update(extraoptions)
                        _plotter(bgs=[hists[0]], sigs=hists[1:], colors=colors, options=options, legend_labels=legend_labels)
                    else:
                        # Get the list of histograms and put them in either bkg or signals
                        sigs = [ hists[index] for index, n in enumerate(sample_names) if n in issig ] # list of signal histograms
                        bkgs = [ hists[index] for index, n in enumerate(sample_names) if (n not in issig) and n != data_sample_name ] # list of bkg histograms
                        data = [ hists[index] for index, n in enumerate(sample_names) if n == data_sample_name ][0] if data_sample_name else None
                        colors = [ colors[index] for index, n in enumerate(sample_names) if n not in issig ] # list of bkg colors
                        # But check if bkgs is at least 1
                        if len(bkgs) == 0:
                            bkgs = [ sigs.pop(0) ]
                        hist_output_name = hist_name
                        if bkg_syst:
                            hist_output_name += "__b{}".format(bkg_syst)
                        if sig_syst:
                            hist_output_name += "__s{}".format(sig_syst)
                        if data_syst:
                            hist_output_name += "__d{}".format(data_syst)
                        options = {"output_name": dirname + "/" + hist_output_name + ".pdf", "signal_scale": signal_scale, "do_ks_test":doKStest}
                        options.update(extraoptions)
                        # ---------Below is special setting that gets set by user
                        # The histxaxislabeloptions is a dict with keys being either "Mbb" or "SRLLChannell__Mbb" <-- with a cut name in front
                        # if the latter is provided then the former is overridden
                        # Otherwise go with the former
                        if hist_name in histxaxislabeloptions or (("__" in hist_name) and (hist_name.split("__")[1] in histxaxislabeloptions)):
                            # has variable name in config?
                            if "__" in hist_name and hist_name.split("__")[1] in histxaxislabeloptions:
                                options.update(histxaxislabeloptions[hist_name.split("__")[1]]) # First update with just variable name as key
                            # has_full_name_config?
                            if hist_name in histxaxislabeloptions:
                                hist_var_name = hist_name
                                options.update(histxaxislabeloptions[hist_var_name]) # If full name key exists update with that as well
                        # ---------Below is special setting that gets set by user
                        _plotter(bgs=bkgs, sigs=sigs, data=data, colors=colors, options=options, legend_labels=legend_labels if _plotter==plot_hist else [], sig_labels=[])
                if hists[0].GetDimension() == 2 and not skip2d:
                    if donorm:
                        for h in hists:
                            h.Scale(1./h.Integral())

                    # Compute range
                    zmax = hists[0].GetMaximum()
                    zmin = hists[0].GetMinimum()
                    for h in hists:
                        zmax = h.GetMaximum() if h.GetMaximum() > zmax else zmax
                        zmin = h.GetMinimum() if h.GetMinimum() > zmin else zmin
                    for h in hists:
                        if histmodfunc:
                            h = histmodfunc(h)
                        # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_log.pdf", "zaxis_log":True, "draw_option_2d":"lego2"})
                        # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_lin.pdf", "zaxis_log":False, "draw_option_2d":"lego2"})
                        # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlog.pdf", "zaxis_log":True, "zaxis_range":[zmin, zmax], "draw_option_2d":"lego2"})
                        # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlin.pdf", "zaxis_log":False, "zaxis_range":[zmin, zmax], "draw_option_2d":"lego2"})
                        options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_log.pdf", "zaxis_log":True, "draw_option_2d":"colz"}
                        options.update(extraoptions)
                        plot_hist_2d(hist=h, options=options)
                        options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_lin.pdf", "zaxis_log":False, "draw_option_2d":"colz"}
                        options.update(extraoptions)
                        plot_hist_2d(hist=h, options=options)
                        options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlog.pdf", "zaxis_log":True, "zaxis_range":[zmin, zmax], "draw_option_2d":"colz"}
                        options.update(extraoptions)
                        plot_hist_2d(hist=h, options=options)
                        options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlin.pdf", "zaxis_log":False, "zaxis_range":[zmin, zmax], "draw_option_2d":"colz"}
                        options.update(extraoptions)
                        plot_hist_2d(hist=h, options=options)
    if do_sum:

        hists = summed_hist

        if output_name:
            hist_name = output_name
        else:
            hist_name = hists[0].GetName()

        if hists[0].GetDimension() == 1:
            if donorm: # shape comparison so use sigs to overlay one bkg with multiple shape comparison
                options = {"signal_scale": "auto", "output_name": dirname + "/" + hist_name + ".pdf"}
                options.update(extraoptions)
                _plotter(bgs=[hists[0]], sigs=hists[1:], colors=colors, options=options, legend_labels=legend_labels)
            else:
                # Get the list of histograms and put them in either bkg or signals
                sigs = [ hists[index] for index, n in enumerate(sample_names) if n in issig ] # list of signal histograms
                bkgs = [ hists[index] for index, n in enumerate(sample_names) if (n not in issig) and n != data_sample_name ] # list of bkg histograms
                data = [ hists[index] for index, n in enumerate(sample_names) if n == data_sample_name ][0] if data_sample_name else None
                colors = [ colors[index] for index, n in enumerate(sample_names) if n not in issig ] # list of bkg colors
                # But check if bkgs is at least 1
                if len(bkgs) == 0:
                    bkgs = [ sigs.pop(0) ]
                options = {"output_name": dirname + "/" + hist_name + ".pdf", "signal_scale": signal_scale}
                options.update(extraoptions)
                _plotter(bgs=bkgs, sigs=sigs, data=data, colors=colors, options=options, legend_labels=legend_labels if _plotter==plot_hist else [])
        if hists[0].GetDimension() == 2:
            if donorm:
                for h in hists:
                    h.Scale(1./h.Integral())

            # Compute range
            zmax = hists[0].GetMaximum()
            zmin = hists[0].GetMinimum()
            for h in hists:
                zmax = h.GetMaximum() if h.GetMaximum() > zmax else zmax
                zmin = h.GetMinimum() if h.GetMinimum() > zmin else zmin
            for h in hists:
                plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_log.pdf", "zaxis_log":True, "draw_option_2d":"lego2"})
                plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_lin.pdf", "zaxis_log":False, "draw_option_2d":"lego2"})
                plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlog.pdf", "zaxis_log":True, "zaxis_range":[zmin, zmax], "draw_option_2d":"lego2"})
                plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlin.pdf", "zaxis_log":False, "zaxis_range":[zmin, zmax], "draw_option_2d":"lego2"})
                # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_log.pdf", "zaxis_log":True, "draw_option_2d":"colz"})
                # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_lin.pdf", "zaxis_log":False, "draw_option_2d":"colz"})
                # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlog.pdf", "zaxis_log":True, "zaxis_range":[zmin, zmax], "draw_option_2d":"colz"})
                # plot_hist_2d(hist=h, options={"output_name": dirname + "/" + str(h.GetName()) + "_" + hist_name + "_commonlin.pdf", "zaxis_log":False, "zaxis_range":[zmin, zmax], "draw_option_2d":"colz"})


def plot_yields(fnames=[], sig_fnames=[], data_fname=None, regions=[], binlabels=[], output_name="yield", dirname="plots", legend_labels=[], signal_labels=None, donorm=False, signal_scale="", extraoptions={}, usercolors=None, hsuffix="_cutflow", _plotter=plot_hist):

    # color_pallete
    colors_ = default_colors
    if usercolors:
        colors_ = usercolors + default_colors

    # Open all files and define color schemes
    sample_names = []
    tfs = {}
    fns = {}
    clrs = {}
    issig = [] # Aggregate a list of signal samples
    for index, fname in enumerate(fnames + sig_fnames):
        n = os.path.basename(fname.replace(".root", ""))
        n += str(index)
        tfs[n] = r.TFile(fname)
        fns[n] = fname
        clrs[n] = colors_[index]
        sample_names.append(n)
        if index >= len(fnames):
            issig.append(n)

    if data_fname:
        n = os.path.basename(data_fname.replace(".root", ""))
        tfs[n] = r.TFile(data_fname)
        fns[n] = data_fname
        clrs[n] = colors_[index]
        sample_names.append(n)

    # Tag the data sample names
    data_sample_name = None
    if data_fname:
        n = os.path.basename(data_fname.replace(".root", ""))
        data_sample_name = n

    yield_hs = []
    for sn in sample_names:
        yield_hs.append(ru.get_yield_histogram( list_of_file_names=[ fns[sn] ], regions=regions, labels=binlabels, hsuffix=hsuffix))
        if signal_labels:
            if sn in issig:
                yield_hs[-1].SetName(signal_labels[issig.index(sn)])
            else:
                yield_hs[-1].SetName(sn)
        else:
            yield_hs[-1].SetName(sn)

    colors = []
    for n in sample_names:
        colors.append(clrs[n])

    # Get the list of histograms and put them in either bkg or signals
    sigs = [ yield_hs[index] for index, n in enumerate(sample_names) if n in issig ] # list of signal histograms
    bkgs = [ yield_hs[index] for index, n in enumerate(sample_names) if (n not in issig) and n != data_sample_name ] # list of bkg histograms
    data = [ yield_hs[index] for index, n in enumerate(sample_names) if n == data_sample_name ][0] if data_sample_name else None
    colors = [ colors[index] for index, n in enumerate(sample_names) if n not in issig ] # list of bkg colors
    # But check if bkgs is at least 1
    if len(bkgs) == 0:
        bkgs = [ sigs.pop(0) ]
    options = {"output_name": dirname + "/" + output_name + ".pdf", "signal_scale": signal_scale}
    options.update(extraoptions)
    _plotter(bgs=bkgs, sigs=sigs, data=data, colors=colors, options=options, legend_labels=legend_labels if _plotter==plot_hist else [])

