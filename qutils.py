#!/bin/env python

import ROOT
import re
import os
import errno    
import sys
from QFramework import TQSampleFolder, TQXSecParser, TQCut, TQAnalysisSampleVisitor, TQSampleInitializer, TQCutflowAnalysisJob, TQCutflowPrinter, TQHistoMakerAnalysisJob, TQNFCalculator, TQCounter
from syncfiles.pyfiles.errors import E
from syncfiles.pyfiles.tqdm import tqdm
import multiprocessing

########################################################################################
def addWeightSystematics(cut, systvars, cutdict):
    for systvar in systvars:
        newname = str(cut.GetName()) + systvar
        newtitle = str(cut.GetTitle()) + systvar
        wgtdef = systvars[systvar]
        #print wgtdef
        newcut = TQCut(newname, newtitle, "1", wgtdef)
        cutdict[str(newcut.GetName())] = newcut
        cut.addCut(newcut)

########################################################################################
# Example usage:
#
#   test = copyEditCuts(
#           cut=tqcuts["SRDilep"],
#           name_edits={"SR":"AR"},
#           cut_edits={"SRDilep" : TQCut("ARDilep" , "ARDilep" , "(nVlep==2)*(nLlep==2)*(nTlep==1)*(lep_pt[0]>25.)*(lep_pt[1]>25.)" , "lepsf"+lepsfvar_suffix)},
#           cutdict=tqcuts,
#           )
#
#   tqcuts["ARDilep"].printCuts("trd")
#
#   tqcuts["Presel"].addCuts(tqcuts["ARDilep"])
#
#
def copyEditCuts(cut, name_edits, cut_edits, cutdict, parentcut=None):

    # Create a new cut
    if cut.GetName() in cut_edits:
        newcut = cut_edits[cut.GetName()]
    else:
        name = str(cut.GetName())
        title = str(cut.GetTitle())
        cutdef = str(cut.getCutExpression())
        wgtdef = str(cut.getWeightExpression())
        newname = reduce(lambda x, y: x.replace(y, name_edits[y]), name_edits, name)
        newtitle = reduce(lambda x, y: x.replace(y, name_edits[y]), name_edits, title)
        newcut = TQCut(newname, newtitle, cutdef, wgtdef)

    cutdict[str(newcut.GetName())] = newcut

    if not parentcut:
        parentcut = newcut
    else:
        parentcut.addCut(newcut)

    if len(cut.getCuts()) == 0:
        return

    # if this cut is to be modded based on what was passed to cut_edits, then replace or add
    for c in cut.getCuts():
        copyEditCuts(c, name_edits, cut_edits, cutdict, newcut)


########################################################################################
def QE(samples, proc, cut):
    count = samples.getCounter(proc, cut).getCounter()
    error = samples.getCounter(proc, cut).getError()
    return E(count, error)

########################################################################################
def addCuts(base, prefix_base, cutdefs, doNm1=True):
    doSyst = False
    cuts = []
    prefix = prefix_base.split("base_")[1]
    for i, cutdef in enumerate(cutdefs):
        cutname = "cut{}_{}".format(i, prefix)
        if i == len(cutdefs) - 1 :
            cutname = "{}".format(prefix)
        cut = TQCut(cutname, cutname, cutdef[0], cutdef[1])
        cuts.append(cut)
    for i in xrange(len(cuts) - 1):
        cuts[i].addCut(cuts[i+1])
    base.addCut(cuts[0])
    if doNm1:
        for i, cutdef in enumerate(cutdefs):
            nm1cuts = [ cut[0] for j, cut in enumerate(cutdefs) if j!=i]
            nm1wgts = [ cut[1] for j, cut in enumerate(cutdefs) if j!=i]
            cutname = "{}_minus_{}".format(prefix, i)
            base.addCut(TQCut(cutname, cutname, combexpr(nm1cuts), combexpr(nm1wgts)))

########################################################################################
def createTQCut(cutname, cutdefs):

    # To hold the TQCuts
    cuts = []
    for i, cutdef in enumerate(cutdefs):

        # Create cut name
        this_cut_name = "cut{}_{}_{}".format(i, cutname, cutdef[0])

        ## If last cut, then the cut name is the "cutname"
        #if i == len(cutdefs) - 1: this_cut_name = "{}".format(cutname)

        # Create TQCut instance
        cut = TQCut(this_cut_name, "{} ".format(i) + cutdef[1] + " ({})".format(cutdef[0]), cutdef[2], cutdef[3])

        # Aggregate cuts
        cuts.append(cut)

    # Add the last cut again
    cutdef = cutdefs[-1]

    # Create TQCut instance
    cut = TQCut(cutname, cutname, cutdef[2], cutdef[3])

    # Aggregate cuts
    cuts.append(cut)

    # Link all the cuts in steps
    for i in xrange(len(cuts)-1):
        cuts[i].addCut(cuts[i+1])

    return cuts[0]

########################################################################################
def combexpr(exprlist):
    cutlist = [ expr[0] if len(expr) != 0 else "1" for expr in exprlist ]
    wgtlist = [ expr[1] if len(expr) != 0 else "1" for expr in exprlist ]
    return "({})".format(")*(".join(cutlist)), "({})".format(")*(".join(wgtlist))

########################################################################################
def atoi(text):
    return int(text) if text.isdigit() else text

########################################################################################
def natural_keys(text):
    '''
    alist.sort(key=natural_keys) sorts in human order
    http://nedbatchelder.com/blog/200712/human_sorting.html
    (See Toothy's implementation in the comments)
    '''
    return [ atoi(c) for c in re.split('(\d+)', text) ]

########################################################################################
def printCutflow(samples, regionname):
    cuts = {}
    cutnames = []
    for counter in samples.getListOfCounterNames():
        if str(counter).find(regionname) != -1 and str(counter).find("cut") != -1:
            title = samples.getCounter("/data", str(counter)).GetTitle()
            cutnames.append(str(counter))
            cuts[str(counter)] = str(title)
    cutnames.sort(key=natural_keys)
    # Cutflow printing
    printer = TQCutflowPrinter(samples)
    for cut in cutnames:
        printer.addCutflowCut(cut, cuts[cut], True)
    addProcesses(printer, showdata=True)
    table = printer.createTable("style.firstColumnAlign=l")
    path = "cutflows/"
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise
    table.writeCSV("cutflows/{}.csv".format(regionname))
    table.writeHTML("cutflows/{}.html".format(regionname))
    table.writeLaTeX("cutflows/{}.tex".format(regionname))
    table.writePlain("cutflows/{}.txt".format(regionname))

########################################################################################
def getSampleListsDeprecated(samples):
    # Get all sample lists
    sample_names = []
    sample_full_names = {}
    for i in samples.getListOfSamples():
        if i.getNSamples(True) == 0:
            sample_name = i.GetName()
            nice_name = sample_name.replace(".root", "")
            sample_names.append(nice_name)
            sample_full_names[nice_name] = sample_name
#    for sample_name in sample_names:
#        print sample_name
    return sample_names, sample_full_names

########################################################################################
def getSampleLists(samples):
    # Get all sample lists
    sample_names = []
    sample_full_names = {}
    for i in samples.getListOfSamples():
        if i.getNSamples(True) == 0:
            sample_name = i.GetName()
            nice_name = sample_name.replace(".root", "")
            sample_names.append(nice_name)
            sample_full_names[nice_name] = sample_name
#    for sample_name in sample_names:
#        print sample_name
    return sample_full_names

########################################################################################
def connectNtuples(samples, config, path, priority="<2", excludepriority=""):
    parser = TQXSecParser(samples);
    parser.readCSVfile(config)
    parser.readMappingFromColumn("*path*")
    if priority.find(">") != -1:
        priority_value = int(priority.split(">")[1])
        parser.enableSamplesWithPriorityGreaterThan("priority", priority_value)
    elif priority.find("<") != -1:
        priority_value = int(priority.split("<")[1])
        parser.enableSamplesWithPriorityLessThan("priority", priority_value)
    if excludepriority.find(">") != -1:
        priority_value = int(excludepriority.split(">")[1])
        parser.disableSamplesWithPriorityGreaterThan("priority", priority_value)
    elif excludepriority.find("<") != -1:
        priority_value = int(excludepriority.split("<")[1])
        parser.disableSamplesWithPriorityLessThan("priority", priority_value)
    parser.addAllSamples(True)
    # By "visiting" the samples with the initializer we actually hook up the samples with root files
    init = TQSampleInitializer(path, 1)
    samples.visitMe(init)
    # Print the content for debugging purpose
    #samples.printContents("rtd")

########################################################################################
def addNtuples(samples, configstr, path, config_filename, priority="<2", excludepriority=""):
    parser = TQXSecParser(samples);
    f = open(config_filename, "w")
    f.write(configstr)
    f.close()
    parser.readCSVfile(config_filename)
    parser.readMappingFromColumn("*path*")
    if priority.find(">") != -1:
        priority_value = int(priority.split(">")[1])
        parser.enableSamplesWithPriorityGreaterThan("priority", priority_value)
    elif priority.find("<") != -1:
        priority_value = int(priority.split("<")[1])
        parser.enableSamplesWithPriorityLessThan("priority", priority_value)
    if excludepriority.find(">") != -1:
        priority_value = int(excludepriority.split(">")[1])
        parser.disableSamplesWithPriorityGreaterThan("priority", priority_value)
    elif excludepriority.find("<") != -1:
        priority_value = int(excludepriority.split("<")[1])
        parser.disableSamplesWithPriorityLessThan("priority", priority_value)
    parser.addAllSamples(True)
    # By "visiting" the samples with the initializer we actually hook up the samples with root files
    init = TQSampleInitializer(path, 1)
    samples.visitMe(init)
    # Print the content for debugging purpose
    #samples.printContents("rtd")

########################################################################################
def runParallel(njobs, func, samples, extra_args):
    pool = multiprocessing.Pool(processes=njobs)
    for sample in samples.getListOfSamples():
        path = str(sample.getPath())
        job = pool.apply_async(func, args=(samples, path, extra_args,))
        #job.get()
    pool.close()
    pool.join()

########################################################################################
def pathToUniqStr(sample_to_run):
    sample_to_run_prefix = sample_to_run.replace("/","-")
    sample_to_run_prefix = sample_to_run_prefix.replace("?","q")
    sample_to_run_prefix = sample_to_run_prefix.replace("[","_")
    sample_to_run_prefix = sample_to_run_prefix.replace("]","_")
    sample_to_run_prefix = sample_to_run_prefix.replace("+","-")
    return sample_to_run_prefix

