#!/bin/env python

import ROOT
import re
import os
import errno    
import sys
from QFramework import *
from syncfiles.pyfiles.errors import E
from syncfiles.pyfiles.tqdm import tqdm
import multiprocessing
import plottery_wrapper as p


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
        if sample.getNSamples(True) == 0:
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

########################################################################################
def makedir(dirpath):
    try:
        os.makedirs(dirpath)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(dirpath):
            pass
        else:
            raise

########################################################################################
def exportTQCutsToTextFile(cuts, filename):

    # Dump to TQFolder
    folder = TQFolder("cuts")
    cuts.dumpToFolder(folder)

    # Export to text files
    folder.exportToTextFile(filename)

########################################################################################
def loadTQCutsFromTextFile(filename):
    # Load from cuts.cfg
    cut_definitions = TQFolder("cuts")
    cut_definitions.importFromTextFile(filename)
    tqcut = None
    for f in cut_definitions.getListOfFolders():
        if not tqcut:
            tqcut = TQCut.importFromFolder(cut_definitions.getFolder(f.GetName()))
        else:
            tqcut.importFromFolderInternal(cut_definitions.getFolder(f.GetName()))
    return tqcut

########################################################################################
def runSingle(samples, sample_to_run, options):

    # Perhaps you run all in serial
    isparallel = (sample_to_run == "")

    # Load the cuts from the config file
    cuts = loadTQCutsFromTextFile(options["cuts"])

    #
    # Book Analysis Jobs (Histogramming, Cutflow, Event lists, etc.)
    #

    # Cutflow is always booked
    cutflowjob = TQCutflowAnalysisJob("cutflow")
    cuts.addAnalysisJob(cutflowjob, "*")

    # If the histogram configuration file is provided
    if "histo" in options and options["histo"] != "":
        histojob = TQHistoMakerAnalysisJob()
        histojob.importJobsFromTextFiles(options["histo"], cuts, "*", True if not isparallel else False)

    # Eventlist jobs (use this if we want to print out some event information in a text format e.g. run, lumi, evt or other variables.)
    if "eventlist" in options and options["eventlist"] != "":
        eventlistjob = TQEventlistAnalysisJob("eventlist")
        eventlistjob.importJobsFromTextFiles(options["eventlist"], cuts, "*", True if not isparallel else False)

    # Declare custom observables
    if "customobservables" in options and len(options["customobservables"]) != 0:
        for observable in options["customobservables"]:
            TQObservable.addObservable(options["customobservables"][observable], observable)

    # Print cuts and numebr of booked analysis jobs for debugging purpose
    if not isparallel:
        cuts.printCut("trd")

    #
    # Loop over the samples
    #

    # setup a visitor to actually loop over ROOT files
    vis = TQAnalysisSampleVisitor(cuts, True)

    # Run the job!
    if sample_to_run:
        samples.visitSampleFolders(vis, "{}".format(sample_to_run))
    else:
        samples.visitSampleFolders(vis)

    # Write the output histograms and cutflow cut values and etc.
    if sample_to_run == "":
        sample_to_run = "output"
    samples.writeToFile(os.path.join(options["output_dir"], pathToUniqStr(sample_to_run) + ".root"))

########################################################################################
def merge_output(samples, options):
    individual_files = []
    for sample in samples.getListOfSamples():
        if sample.getNSamples(True) == 0:
            path = str(sample.getPath())
            individual_files.append(os.path.join(options["output_dir"], pathToUniqStr(path) + ".root"))
    os.system("python rooutil/qframework/share/tqmerge -o {}/output.root -t analysis {}".format(options["output_dir"], " ".join(individual_files)))

########################################################################################
def loop(user_options):

    options = {

        # The main root TQSampleFolder name
        "master_sample_name" : "samples",

        # Where the ntuples are located
        "ntuple_path" : "/nfs-7/userdata/phchang/WWW_babies/WWW_v1.2.3/skim/",

        # Path to the config file that defines how the samples should be organized
        "sample_config_path" : "samples.cfg",

        # The samples with "priority" (defined in sample_config_pat) values satisfying the following condition is looped over
        "priority_value" : ">0",

        # The samples with "priority" (defined in sample_config_pat) values satisfying the following condition is NOT looped over
        "exclude_priority_value" : "<-1",

        # N-cores
        "ncore" : 16,

        # TQCuts config file
        "cuts" : "cuts.cfg",

        # Histogram config file
        "histo" : "histo.cfg",

        # Eventlist histogram
        "eventlist" : "eventlist.cfg",

        # Custom observables (dictionary)
        "customobservables" : {},

        # Custom observables (dictionary)
        "output_dir" : "outputs/",

        # Do merge
        "do_merge" : True

    }

    # Update options with the user provided values
    options.update(user_options)

    # Create output dir
    makedir(options["output_dir"])

    # Create the master TQSampleFolder
    samples = TQSampleFolder(options["master_sample_name"])

    # Connect input baby ntuple
    connectNtuples(samples, options["sample_config_path"], options["ntuple_path"], options["priority_value"], options["exclude_priority_value"])

    # Run parallel jobs
    runParallel(options["ncore"], runSingle, samples, options)

    # Merge output
    if options["do_merge"]:
        merge_output(samples, options)

########################################################################################
def output_plotname(histname):
    nicename = str(histname).replace("/","-")
    nicename = nicename.replace("{","Bin_")
    nicename = nicename.replace("}","")
    nicename = nicename.replace(",","_")
    nicename = nicename.replace(" ","")
    return nicename

########################################################################################
def plot(samples, histname, bkg_path={}, sig_path={}, data_path=None, systs=None, clrs=[], options={}, plotfunc=p.plot_hist):
    output_dir = "plots"
    if "output_dir" in options:
        output_dir = options["output_dir"]
        del options["output_dir"]
    # Options
    alloptions= {
                "ratio_range":[0.0,2.0],
                "nbins": 30,
                "autobin": False,
                "legend_scalex": 1.4,
                "legend_scaley": 1.1,
                "output_name": "{}/{}.pdf".format(output_dir, output_plotname(histname))
                }
    alloptions.update(options)
    bkgs = []
    sigs = []
    for bkg in bkg_path: bkgs.append(samples.getHistogram(bkg_path[bkg], histname).Clone(bkg))
    for sig in sig_path: sigs.append(samples.getHistogram(sig_path[sig], histname).Clone(sig))
    if data_path:
        data = samples.getHistogram(data_path, histname).Clone("Data")
    else:
        data = None
    if len(clrs) == 0: colors = [ 2005, 2001, 2012, 2003, 920, 2007 ]
    else: colors = clrs
    plotfunc(
            sigs = sigs,
            bgs  = bkgs,
            data = data,
            colors = colors,
            syst = systs,
            options=alloptions)

########################################################################################
def autoplot(samples, bkg_path={}, sig_path={}, data_path=None, systs=None, clrs=[], options={}, plotfunc=p.plot_hist):
    import multiprocessing
    jobs = []
    for histname in samples.getListOfHistogramNames():
        proc = multiprocessing.Process(target=plot, args=[samples, str(histname)], kwargs={"bkg_path":bkg_path, "sig_path":sig_path, "data_path":data_path, "systs":systs, "clrs":clrs, "options":options, "plotfunc":plotfunc})
        jobs.append(proc)
        proc.start()
    for job in jobs:
        job.join()

########################################################################################
def autotable(samples, tablename, bkg_path={}, sig_path={}, data_path=None, systs=None, clrs=[], options={}, plotfunc=p.plot_hist):
    printer = TQCutflowPrinter(samples)

    # Defining which columns. e.g. Backgrounds, total background, signal, data, ratio etc.
    printer.addCutflowProcess("|", "|")
    for bkg in bkg_path:
        printer.addCutflowProcess(bkg_path[bkg], bkg)
    printer.addCutflowProcess("|", "|")
    totalbkgpath = '+'.join([ bkg_path[bkg][1:] for bkg in bkg_path ])
    printer.addCutflowProcess(totalbkgpath, "Total Bkg.")
    printer.addCutflowProcess("|", "|")
    if len(sig_path) > 0:
        for sig in sig_path:
            printer.addCutflowProcess(sig_path[sig], sig)
        printer.addCutflowProcess("|", "|")
    if data_path:
        printer.addCutflowProcess(data_path, "Data")
        printer.addCutflowProcess("$ratio({}, {})".format(data_path, totalbkgpath), "Data / Total Bkg.")
        printer.addCutflowProcess("|", "|")

    # Defining which rows. e.g. which cuts
    # If cut configuration file is not provided by "cuts": cuts.cfg argument
    # then we use getListOfCounterNames()
    # If provided, then we use it to build up a nice table
    # TODO Cut filter
    if "cuts" in options:
        tqcuts = loadTQCutsFromTextFile(options["cuts"])

        # Recursive function
        def addCutflowCuts(printer, cuts, indent=0):
            printer.addCutflowCut(cuts.GetName(), "&emsp;"*indent + '&#x21B3;' * (indent > 0) + str(cuts.GetTitle()))
            nextindent = indent + 1
            for cut in cuts.getCuts():
                addCutflowCuts(printer, cut, nextindent)

        # Add all cuts
        addCutflowCuts(printer, tqcuts)

    else:
        print "ERROR - Please provide options[\"cuts\"] = \"cuts.cfg\"!"

    # Write out to html, tex, txt, csv
    table = printer.createTable("style.firstColumnAlign=l")
    output_dir = "cutflows"
    if "output_dir" in options:
        output_dir = options["output_dir"]
    makedir(output_dir)
    table.writeCSV  ("{}/{}.csv" .format(output_dir, tablename))
    table.writeHTML ("{}/{}.html".format(output_dir, tablename))
    table.writeLaTeX("{}/{}.tex" .format(output_dir, tablename))
    table.writePlain("{}/{}.txt" .format(output_dir, tablename))

    # Stupid hack :( to fix the missing hashtag from qframework writeHTML function
    FileName = "{}/{}.html".format(output_dir, tablename)
    with open(FileName) as f:
        newText=f.read().replace('&21B3', '&#x21B3')

    with open(FileName, "w") as f:
        f.write(newText)
