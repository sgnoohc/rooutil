#!/bin/env python

import argparse
parser = argparse.ArgumentParser(description="duplicate event removal and create a new set of TTrees")
parser.add_argument('--output', '-o', dest='output', default='output.root', help='output root file path')
parser.add_argument('--size', '-s', dest='size', help='max tree size')
parser.add_argument('--treename', '-t', dest='treename', default='t', help='treename')
parser.add_argument('--evt', '-e', dest='evt', default='evt', help='event branch name')
parser.add_argument('--run', '-r', dest='run', default='run', help='run branch name')
parser.add_argument('--lumi', '-l', dest='lumi', default='lumi', help='lumi branch name')
parser.add_argument('files', metavar='FILE.root', type=str, nargs='+', help='input files')

args = parser.parse_args()

#print args.output
#print args.size
#print args.treename
#print args.files

import ROOT as r
import os
r.gROOT.SetBatch(True)
thispypathdir = os.path.dirname(os.path.realpath(__file__))
r.gSystem.Load(os.path.join(thispypathdir, "rooutil.so"))
r.gROOT.ProcessLine('.L {}'.format(os.path.join(thispypathdir, "dorky.h")))

chain = r.TChain(args.treename)
for rfile in args.files:
    chain.Add(rfile)

print chain.GetEntries()

ofile = r.TFile.Open(args.output, 'recreate')
otree = chain.CloneTree(0)

if args.size:
    otree.SetMaxTreeSize(int(args.size))

for event in chain:
    evt = getattr(event, args.evt)
    run = getattr(event, args.run)
    lumi = getattr(event, args.lumi)
    eventid = r.duplicate_removal.DorkyEventIdentifier(run, evt, lumi)
    if r.duplicate_removal.is_duplicate(eventid):
        continue
    otree.Fill()

otree.Write()
