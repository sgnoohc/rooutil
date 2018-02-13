#!/bin/env python

import os
import sys
import ROOT as r

def printsf(funcname, xthreshs, ythreshs, sfs, errs, filename="", xvar="eta", yvar="pt", xvarabs=False, yvarabs=False):
    """
    Function to print scale factors (or fake rate) from arrays of numbers
    """
    # parse some options and process some stuff
    yvarabsstr = ""
    xvarabsstr = ""
    if yvarabs: yvarabsstr = "fabs"
    if xvarabs: xvarabsstr = "fabs"

    # Form the function sring
    funcstr = ""
    funcstr += "float {}(float {}, float {}, int isyst=0)\n".format(funcname, yvar, xvar)
    funcstr += "{\n"
    funcstr += "    if (isyst != 1 && isyst != -1 && isyst != 0)\n"
    funcstr += "        printf(Form(\"WARNING - in function=%s, isyst=%d is not recommended!\\n\", __FUNCTION__, isyst));\n"
    for i, xthresh in enumerate(xthreshs):
        for j, ythresh in enumerate(ythreshs):
            sf = sfs[i][j]
            err = errs[i][j]
            if i == len(xthreshs) - 1 and j == len(ythreshs):
                funcstr += "    return {} + isyst * {};\n".format(yvarabsstr, yvar, ythresh, xvarabsstr, xvar, xthresh, sf, err)
            elif i == len(xthreshs) -1:
                funcstr += "    if ({}({}) < {}) return {} + isyst * {};\n".format(yvarabsstr, yvar, ythresh, sf, err)
            #elif j == len(ythreshs) -1:
            #    funcstr += "    if ({}({}) < {}) return {} + isyst * {};\n".format(yvarabsstr, yvar, ythresh, sf, err)
            else:
                funcstr += "    if ({}({}) < {} && {}({}) < {}) return {} + isyst * {};\n".format(yvarabsstr, yvar, ythresh, xvarabsstr, xvar, xthresh, sf, err)
    funcstr += "    printf(\"WARNING in {}(): the given phase-space (%f, %f) did not fall under any range!\\n\", {}, {}); \n".format(funcname, yvar, xvar)
    funcstr += "    return 1;\n"
    funcstr += "}\n"

    # print or write to file
    if len(filename) != 0:
        f = open(filename, "w")
        f.write(funcstr)
    else:
        print funcstr

def printsf_th2(funcname, th2, filename="", xvar="eta", yvar="pt", xvarabs=False, yvarabs=False):
    """
    Function to print scale factors (or fake rate) from TH2
    """
    sfs = []
    errs = []
    xthreshs = []
    ythreshs = []
    for i in xrange(1, th2.GetNbinsX() + 1): xthreshs.append(th2.GetXaxis().GetBinUpEdge(i))
    for i in xrange(1, th2.GetNbinsY() + 1): ythreshs.append(th2.GetYaxis().GetBinUpEdge(i))
    for i in xrange(1, th2.GetNbinsX() + 1):
        sfs.append([])
        errs.append([])
        for j in xrange(1, th2.GetNbinsY() + 1):
            sfs[i-1].append(th2.GetBinContent(i, j))
            errs[i-1].append(th2.GetBinError(i, j))

    printsf(funcname, xthreshs, ythreshs, sfs, errs, filename, xvar, yvar, xvarabs, yvarabs)

def printsf_tgraph1d(funcname, tgraph1d, filename="", xvar="eta", yvar="pt", xvarabs=False, yvarabs=False):
    """
    Function to print scale factors (or fake rate) from 1D TGraph
    WARNING: this only takes one side of the error
    """
    npoints = tgraph1d.GetN()
    xs = tgraph1d.GetX()
    ys = tgraph1d.GetY()
    xerrs = tgraph1d.GetEX()
    yerrs = tgraph1d.GetEY()
    xthreshs = [1000000]
    ythreshs = []
    sfs = [[]]
    errs = [[]]
    for index, x in enumerate(xs):
        ythreshs.append(x + tgraph1d.GetErrorXhigh(index))
    for index, y in enumerate(ys):
        sfs[0].append(y)
        errs[0].append(tgraph1d.GetErrorYhigh(index))
    printsf(funcname, xthreshs, ythreshs, sfs, errs, filename, xvar, yvar, xvarabs, yvarabs)

#eof
