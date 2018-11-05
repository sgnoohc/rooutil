#!/bin/env python

import plottery_wrapper as p
import ROOT as r
import sys

#______________________________________________________________________________
def get_histograms(list_of_file_names, hist_name):
    hists = []
    for file_name in list_of_file_names:
        f = r.TFile(file_name)
        try:
            h = f.Get(hist_name).Clone(hist_name)
            h.SetDirectory(0)
            hists.append(h)
        except:
            print "Could not find", hist_name, "in", file_name
    return hists

#______________________________________________________________________________
def get_summed_histogram(list_of_file_names, hist_names):
    if isinstance(hist_names, list):
        hists = []
        for hist_name in hist_names:
            hists.extend(get_histograms(list_of_file_names, hist_name))
        hist_name = hist_names[0] + "_plus_etc"
    else:
        hists = get_histograms(list_of_file_names, hist_names)
        hist_name = hist_names
    if len(hists) == 0:
        print "error no histograms are found query=", list_of_file_names, hist_names
        raise ValueError("No histograms are found with the query")
        sys.exit()
    rtn_hist = hists[0].Clone(hist_name)
    rtn_hist.Reset()
    rtn_hist.SetDirectory(0)
    for h in hists:
        rtn_hist.Add(h)
    return rtn_hist

#______________________________________________________________________________
def get_yield_from_cutflow_histogram(list_of_file_names, reg_name):
    hist = get_summed_histograms(list_of_file_names, reg_name + "_cutflow")
    return hist.GetBinContent(hist.GetNbinsX())

#______________________________________________________________________________
def get_shape_reweighting_histogram(numerator, denominator):
    ratio = numerator.Clone("ratio")
    ratio.Divide(denominator)
    if numerator.Integral() == 0:
        raise ValueError("numerator histogram has integral of zero")
    scale = denominator.Integral() / numerator.Integral() 
    ratio.Scale(scale)
    return ratio
