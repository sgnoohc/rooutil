#!/bin/env python

import argparse
parser = argparse.ArgumentParser(description="From series of CSVs create a TTree")
parser.add_argument('--output'     , '-o' , dest='output'     , default='output.root' , help='output root file path')
parser.add_argument('--treename'   , '-t' , dest='treename'   , default='t'           , help='reference tree name to obtain number of events to determine how many files to merge at a time')
parser.add_argument('--nevents'    , '-n' , dest='nevents'    , default=50000         , help='number of events to put at max per output merged files')
parser.add_argument('files', metavar='FILE.csv', type=str, nargs='+', help='input files')
args = parser.parse_args()

import ROOT as r
import sys
import os

# Get all the nevents file from the reference tree name
nevents = {}
for to_merge_file_path in args.files:
    f = r.TFile(to_merge_file_path)
    t = f.Get(args.treename)
    nevents[to_merge_file_path] = int(t.GetEntries())

# Now figure out what files to merge at a time
clusters = []
cluster = []
nevents_in_current_cluster = 0
for fn in args.files:
    if nevents_in_current_cluster + nevents[fn] > args.nevents:
        clusters.append(cluster)
        nevents_in_current_cluster = nevents[fn]
        cluster = [fn]
    else:
        nevents_in_current_cluster += nevents[fn]
        cluster.append(fn)

# very last cluster needs to be added
clusters.append(cluster)

## print the clustering info
#for cluster in clusters:
#    print cluster

# Obtain the header path
output_path_without_dot_root = args.output.replace(".root", "")

# Run the commands
for index, cluster in enumerate(clusters):

    command = "hadd -f {}_{}.root {}".format(output_path_without_dot_root, index+1, " ".join(cluster))
    print command
    os.system(command)
