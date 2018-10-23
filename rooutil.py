#!/bin/env python

import plottery_wrapper as p
import ROOT as r
import sys

#______________________________________________________________________________
def get_histograms(list_of_file_names, hist_name):
    hists = []
    for file_name in list_of_file_names:
        f = r.TFile(file_name)
        h = f.Get(hist_name).Clone(hist_name)
        h.SetDirectory(0)
        hists.append(h)
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
        print "error no histograms are found"
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
