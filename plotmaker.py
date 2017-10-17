#!/cvmfs/cms.cern.ch/slc6_amd64_gcc530/cms/cmssw/CMSSW_8_0_18/external/slc6_amd64_gcc530/bin/python

import os
import ROOT as r
r.gROOT.SetBatch(True)
r.gROOT.ProcessLine(".L {0}/plotmaker.cc".format(os.path.realpath(__file__).rsplit("/",1)[0]))
