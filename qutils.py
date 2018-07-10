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
def copyEditCuts(cut, name_edits, cut_edits, cutdict, terminate=[], parentcut=None):

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

    if str(newcut.GetName()) not in cutdict:
        cutdict[str(newcut.GetName())] = newcut

    if not parentcut:
        parentcut = newcut
    else:
        parentcut.addCut(newcut)

    if cut.GetName() in terminate:
        return

    if len(cut.getCuts()) == 0:
        return

    # if this cut is to be modded based on what was passed to cut_edits, then replace or add
    for c in cut.getCuts():
        copyEditCuts(c, name_edits, cut_edits, cutdict, terminate, newcut)


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
        #histojob.importJobsFromTextFiles(options["histo"], cuts, "*", True if not isparallel else False)
        histojob.importJobsFromTextFiles(options["histo"], cuts, "*", False)

    # Eventlist jobs (use this if we want to print out some event information in a text format e.g. run, lumi, evt or other variables.)
    if "eventlist" in options and options["eventlist"] != "":
        eventlistjob = TQEventlistAnalysisJob("eventlist")
        #eventlistjob.importJobsFromTextFiles(options["eventlist"], cuts, "*", True if not isparallel else False)
        eventlistjob.importJobsFromTextFiles(options["eventlist"], cuts, "*", False)

    # Declare custom observables
    if "customobservables" in options and len(options["customobservables"]) != 0:
        for observable in options["customobservables"]:
            TQObservable.addObservable(options["customobservables"][observable], observable)

    ## Print cuts and numebr of booked analysis jobs for debugging purpose
    #if not isparallel:
    #    cuts.printCut("trd")

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
    os.system("python rooutil/qframework/share/tqmerge -o {}/output{}.root -t analysis {}".format(options["output_dir"], options["output_suffix"], " ".join(individual_files)))

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

        # specific path defined
        "output_suffix" : "",

        # Do merge
        "do_merge" : True,

        # specific path defined
        "path" : ""

    }

    # Update options with the user provided values
    options.update(user_options)

    # Create output dir
    makedir(options["output_dir"])

    # Create the master TQSampleFolder
    samples = TQSampleFolder(options["master_sample_name"])

    # Connect input baby ntuple
    connectNtuples(samples, options["sample_config_path"], options["ntuple_path"], options["priority_value"], options["exclude_priority_value"])

    # If a specific path is specified run one job
    if "path" in options and options["path"] != "":
        runSingle(samples, options["path"], options)

    # Otherwise, run parallel jobs
    else:
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
def plot(samples, histname, bkg_path=[], sig_path=[], data_path=None, systs=None, clrs=[], options={}, plotfunc=p.plot_hist):
    output_dir = "plots"
    if "output_dir" in options:
        output_dir = options["output_dir"]
        del options["output_dir"]
    # Options
    alloptions= {
                "ratio_range":[0.0,2.0],
                "nbins": 30,
                "autobin": False,
                "legend_scalex": 1.8,
                "legend_scaley": 1.1,
                "output_name": "{}/{}.pdf".format(output_dir, output_plotname(histname)),
                "bkg_sort_method": "unsorted"
                }
    alloptions.update(options)
    bkgs = []
    sigs = []
    for bkg, path in bkg_path: bkgs.append(samples.getHistogram(path, histname).Clone(bkg))
    for sig, path in sig_path: sigs.append(samples.getHistogram(path, histname).Clone(sig))
    # Check for blinding condition
    blind = False
    if "blind" in options:
        for keyword in options["blind"]:
            print keyword, histname
            if histname.find(keyword) != -1:
                blind = True
        alloptions["blind"] = blind
    if data_path:
        data = samples.getHistogram(data_path, histname).Clone("Data")
    else:
        data = None
    if len(clrs) == 0: colors = [ 920, 2007, 2005, 2003, 2001, 2 ]
    else: colors = clrs
    plotfunc(
            sigs = sigs,
            bgs  = bkgs,
            data = data,
            colors = colors,
            syst = systs,
            options=alloptions)

########################################################################################
def autoplot(samples, histnames=[], bkg_path=[], sig_path=[], data_path=None, systs=None, clrs=[], options={}, plotfunc=p.plot_hist):
    import multiprocessing
    jobs = []
    if len(histnames) == 0:
        histnames = samples.getListOfHistogramNames()
    for histname in histnames:
        proc = multiprocessing.Process(target=plot, args=[samples, str(histname)], kwargs={"bkg_path":bkg_path, "sig_path":sig_path, "data_path":data_path, "systs":systs, "clrs":clrs, "options":options, "plotfunc":plotfunc})
        jobs.append(proc)
        proc.start()
    for job in jobs:
        job.join()

########################################################################################
def table(samples, from_cut, bkg_path=[], sig_path=[], data_path=None, systs=None, options={}):
    printer = TQCutflowPrinter(samples)

    # Defining which columns. e.g. Backgrounds, total background, signal, data, ratio etc.
    printer.addCutflowProcess("|", "|")
    for bkg, path in bkg_path:
        printer.addCutflowProcess(path, bkg)
    printer.addCutflowProcess("|", "|")
    totalbkgpath = '+'.join([ path[1:] for bkg, path in bkg_path ])
    printer.addCutflowProcess(totalbkgpath, "Total Bkg.")
    printer.addCutflowProcess("|", "|")
    if len(sig_path) > 0:
        for sig, path in sig_path:
            printer.addCutflowProcess(path, sig)
        printer.addCutflowProcess("|", "|")
    if data_path:
        printer.addCutflowProcess(data_path, "Data")
        printer.addCutflowProcess("$ratio({}, {})".format(data_path, totalbkgpath), "Data / Total Bkg.")
        printer.addCutflowProcess("|", "|")

    if "show_detail" in options and options["show_detail"]:
        for sample in samples.getListOfSamples():
            if sample.getNSamples(True) == 0:
                path = str(sample.getPath())
                printer.addCutflowProcess(path, path)

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

        # Otherwise, add all cuts
        addCutflowCuts(printer, tqcuts.getCut(from_cut))

    else:
        print "ERROR - Please provide options[\"cuts\"] = \"cuts.cfg\"!"

    # Write out to html, tex, txt, csv
    table = printer.createTable("style.firstColumnAlign=l")
    output_dir = "cutflows"
    if "output_dir" in options:
        output_dir = options["output_dir"]
    makedir(output_dir)
    table.writeCSV  ("{}/{}.csv" .format(output_dir, from_cut))
    table.writeHTML ("{}/{}.html".format(output_dir, from_cut))
    table.writeLaTeX("{}/{}.tex" .format(output_dir, from_cut))
    table.writePlain("{}/{}.txt" .format(output_dir, from_cut))

    print ">>> Saving {}/{}.html".format(output_dir, from_cut)

    # Stupid hack :( to fix the missing hashtag from qframework writeHTML function
    FileName = "{}/{}.html".format(output_dir, from_cut)
    with open(FileName) as f:
        newText=f.read().replace('&21B3', '&#x21B3')

    with open(FileName, "w") as f:
        f.write(newText)

########################################################################################
def autotable(samples, cutnames=[], bkg_path=[], sig_path=[], data_path=None, systs=None, options={}):
    import multiprocessing
    jobs = []
    if len(cutnames) == 0:
        print "ERROR - provided no cut names to create table from"
    for cutname in cutnames:
        proc = multiprocessing.Process(target=table, args=[samples, str(cutname)], kwargs={"bkg_path":bkg_path, "sig_path":sig_path, "data_path":data_path, "systs":systs, "options":options})
        jobs.append(proc)
        proc.start()
    for job in jobs:
        job.join()

########################################################################################
def get_cr_normalized_rate(options, key):
    # This is parsing an example like this:
    #     ("SRSSeeFull", "/typebkg/lostlep/[ttZ+WZ+Other]") : { "CR" : ("WZCRSSeeFull", "/data-typebkg/[qflip+photon+prompt+fakes]-sig"), "systs" : ["LepSF", "TrigSF", "BTagLF", "BTagHF", "PileUp", "JEC"] },
    sr = options["nominal_sample"].getCounter(key[1], key[0])
    crdatapath = options["control_regions"][key][1]
    crprocpath = key[1]
    crname = options["control_regions"][key][0]
    nf = options["nominal_sample"].getCounter(crdatapath, crname)
    pr = options["nominal_sample"].getCounter(crprocpath, crname)
    nf.divide(pr)
    #print sr.getCounter()
    sr.multiply(nf)
    #print nf.getCounter(), sr.getCounter()
    return sr.getCounter()

########################################################################################
def get_sr_rate(samples, path, r, suffix, options):
    if (r, path) not in options["control_regions"]:
        return samples.getCounter(path, r+suffix).getCounter()
    else:
        # The TF calculation
        cr = options["control_regions"][(r, path)][0]
        # nominal sr
        sr_nom = options["nominal_sample"].getCounter(path,  r)
        cr_nom = options["nominal_sample"].getCounter(path, cr)
        # syst
        sr_sys = samples.getCounter(path,  r+suffix)
        cr_sys = samples.getCounter(path, cr+suffix)
        sr_nom.divide(cr_nom)
        sr_sys.divide(cr_sys)
        sr_sys.divide(sr_nom)
        return get_cr_normalized_rate(options, (r, path)) * sr_sys.getCounter()

########################################################################################
def get_tf_rate(r, path, options):
    # The TF calculation
    cr = options["control_regions"][(r, path)][0]
    sr_nom = options["nominal_sample"].getCounter(path,  r)
    cr_nom = options["nominal_sample"].getCounter(path, cr)
    sr_nom.divide(cr_nom)
    return sr_nom.getCounter()

########################################################################################
def make_counting_experiment_statistics_data_card(options):

    #
    # The goal is to create a data card for https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideHiggsAnalysisCombinedLimit
    #

    column_width = 10
    for b in options["bins"]:
        if len(b) + 1> column_width:
            column_width = len(b) + 1

    def form(s): return ("{:<"+str(column_width)+"s}").format(s)
    def flts(f): return ("{:<"+str(column_width)+"s}").format("{:<6.3f}".format(f)) if f > 0 else form("1e-9")

    # Channels (e.g. SR1, SR2, SR3, ...)
    nchannel = len(options["bins"])
    channels = [ form(x) for x in options["bins"]]

    # Processes (e.g. Higgs, ttbar, WW, W, Z, etc.)
    nprocess = len(options["bkgs"]) + 1
    processes = [ form(x) for x, path in ([options["sig"]] + options["bkgs"])]
    process_indices = [ form(str(index)) for index, x in enumerate([options["sig"]] + options["bkgs"])]
    bins_list = [ x * nprocess for x in channels ]
    processes_list = processes * nchannel

    # Creating list to access contents
    cuts_list = [ x for x in options["bins"] for i in range(nprocess) ]
    paths = [ path for x, path in ([options["sig"]] + options["bkgs"])]
    paths_list = paths * nchannel

    # nobservation to be printed
    nobs = [ form(str(int(options["nominal_sample"].getCounter(options["data"], r).getCounter()))) for r in options["bins"] ]

    # rates
    rates_val = []
    for r, path in zip(cuts_list, paths_list):
        key = (r, path)
        if key in options["control_regions"]:
            rates_val.append(get_cr_normalized_rate(options, key))
        else:
            rates_val.append(options["nominal_sample"].getCounter(path, r).getCounter())

    #rates_val = [ c.getCounter() for c in [ options["nominal_sample"].getCounter(path, r) if (r, proc.strip()) not in options["control_regions"] else get_cr_normalized_rate(options, options["control_regions"][(r, proc.strip())]) for r, path, proc in zip(cuts_list, paths_list, processes_list) ] ]
    rates_str = [ flts(cnt) for cnt in rates_val ]
            
    # items to be printed
    nchannel_formatted = nchannel
    channels_formatted = "".join(channels)
    bins_formatted = "".join(bins_list)
    processes_formatted = "".join(processes_list)
    process_indices_formatted = "".join(process_indices * nchannel)
    nobs_formatted = "".join(nobs)
    rates_formatted = "".join(rates_str)

    datacard = """# Counting experiment with multiple channels
imax {nchannel}  number of channels
jmax *   number of backgrounds ('*' = automatic)
kmax *   number of nuisance parameters (sources of systematical uncertainties)
------------
# three channels, each with it's number of observed events
bin          {channels}
observation  {nobs}
------------
# now we list the expected events for signal and all backgrounds in those three bins
# the second 'process' line must have a positive number for backgrounds, and 0 for signal
# then we list the independent sources of uncertainties, and give their effect (syst. error)
# on each process and bin
bin                                           {bins}
process                                       {processes}
process                                       {process_indices}
rate                                          {rates}
------------
""".format(
        nchannel=nchannel_formatted,
        channels=channels_formatted,
        nobs=nobs_formatted,
        bins=bins_formatted,
        processes=processes_formatted,
        process_indices=process_indices_formatted,
        rates=rates_formatted,
        )

    ## Weight variation systematics that are saved in the "nominal_sample" TQSampleFolder
    ## The nomenclature of the coutner names must be <BIN_COUNTER><SYSTS>Up and <BIN_COUNTER><SYSTS>Down
    ## Or if the "syst_samples" are provided in the dictionary use that instead
    ## The keyword are the systematics and then the items list the processes to apply the systematics
    #
    # For example they will have the following format
    # "systematics" : [
    #     ("LepSF"         , { "procs_to_apply" : ["vbsww", "ttw", "photon", "qflip", "prompt"]                                                                          }),
    #     ("TrigSF"        , { "procs_to_apply" : ["vbsww", "ttw", "photon", "qflip", "prompt"]                                                                          }),
    #     ("BTagLF"        , { "procs_to_apply" : ["vbsww", "ttw", "photon", "qflip", "prompt"]                                                                          }),
    #     ("BTagHF"        , { "procs_to_apply" : ["vbsww", "ttw", "photon", "qflip", "prompt"]                                                                          }),
    #     ("Pileup"        , { "procs_to_apply" : ["vbsww", "ttw", "photon", "qflip", "prompt"]                                                                          }),
    #     ("FakeRateEl"    , { "procs_to_apply" : ["fake"]                                                                                                               }),
    #     ("FakeRateMu"    , { "procs_to_apply" : ["fake"]                                                                                                               }),
    #     ("FakeClosureEl" , { "procs_to_apply" : ["fake"]                                                                                                               }),
    #     ("FakeClosureMu" , { "procs_to_apply" : ["fake"]                                                                                                               }),
    #     ("PDF"           , { "procs_to_apply" : ["www"]                                                                                                                }),
    #     ("AlphaS"        , { "procs_to_apply" : ["www"]                                                                                                                }),
    #     ("Qsq"           , { "procs_to_apply" : ["www"]                                                                                                                }),
    #     ("JEC"           , { "procs_to_apply" : ["www", "vbsww", "ttw", "photon", "qflip", "prompt"], "syst_samples" : {"Up" : samples_jec_up, "Down": samples_jec_dn} }),
    #     ("MCStat"        , { "procs_to_apply" : ["www", "vbsww", "ttw", "photon", "qflip", "prompt"], "individual": True                                               }),
    #     ],
    for syst, systinfo in options["systematics"]:
        # If "syst_samples" are provided in the systinfo dictionary then use nominal cut counter of the provided sample to get the variations
        # If not provided, then attach a suffix to the counter name (these would be the weight variations)
        # If "syst_samples" not provided than it is a weight variational type so create a suffix to attach to the counter name
        syst_up_name_suffix = syst + "Up"   if "syst_samples" not in systinfo else ""
        syst_dn_name_suffix = syst + "Down" if "syst_samples" not in systinfo else ""
        samples_up = options["nominal_sample"] if "syst_samples" not in systinfo else systinfo["syst_samples"]["Up"]
        samples_dn = options["nominal_sample"] if "syst_samples" not in systinfo else systinfo["syst_samples"]["Down"]
        syst_up_rates_val = [ c for c in [ get_sr_rate(samples_up, path, r, syst_up_name_suffix, options) if process.strip() in systinfo["procs_to_apply"] else 0 for r, process, path in zip(cuts_list, processes * nchannel, paths_list) ] ]
        syst_dn_rates_val = [ c for c in [ get_sr_rate(samples_dn, path, r, syst_dn_name_suffix, options) if process.strip() in systinfo["procs_to_apply"] else 0 for r, process, path in zip(cuts_list, processes * nchannel, paths_list) ] ]
        syst_val_str = [ form("{:.3f}/{:<.3f}".format(max(dn, 0.001), up)) if (up > 0 or dn > 0) else form("-")  for up, dn in [ ((u / n, d / n) if n > 0 else (1, 1)) if p.strip() in systinfo["procs_to_apply"] else (-999, -999) for u, d, n, p in zip(syst_up_rates_val, syst_dn_rates_val, rates_val, processes * nchannel) ] ]
        syst_item = """{:<35s}lnN        {}\n""".format(syst, "".join(syst_val_str))
        datacard += syst_item

    # Statistical error per bin per channel add a statistical error from the MC
    for index, (r, process, path) in enumerate(zip(cuts_list, processes * nchannel, paths_list)):
        if process.strip() not in options["statistical"]:
            continue
        cnt = options["nominal_sample"].getCounter(path, r).getCounter()
        err = options["nominal_sample"].getCounter(path, r).getError()
        errors = [(0, 0)] * nprocess * nchannel
        errors[index] = ((cnt + err) / cnt, (cnt - err) / cnt) if cnt > 0 else (1, 1)
        syst_val_str = [ form("{:.3f}/{:<.3f}".format(max(dn, 0.001), up)) if (up > 0 or dn > 0) else form("-") for up, dn in errors ]
        systname = process.strip() + "_MCstat" + "_" + r
        syst_item = """{:<35s}lnN        {}\n""".format(systname, "".join(syst_val_str))
        datacard += syst_item

    # Control region statistical error
    # CR data stat error can be controlled via "gmN" error
    # In the options the control regions are provided in a following format
    #
    # "control_regions" : {
    #     ("SRSSeeFull"  , "/typebkg/lostlep/[ttZ+WZ+Other]") : ("WZCRSSeeFull", "/data-typebkg/[qflip+photon+prompt+fakes]-sig"),
    #     ("SideSSeeFull", "/typebkg/lostlep/[ttZ+WZ+Other]") : ("WZCRSSeeFull", "/data-typebkg/[qflip+photon+prompt+fakes]-sig"),
    #     },
    #
    # We first invert the regions such that we have a mapping per "CR" -> "SR's"
    crmap = {}
    for k, v in options["control_regions"].iteritems():
        crmap[v] = crmap.get(v, [])
        crmap[v].append(k)

    for key in crmap:
        syst_val_str = []
        for index, (r, process, path) in enumerate(zip(cuts_list, processes * nchannel, paths_list)):
            if (r, path) not in crmap[key]:
                syst_val_str.append(form("-"))
            else:
                syst_val_str.append(form("{:.3f}".format(get_tf_rate(r, path, options))))
        systname = key[0] + "_CRstat"
        data = int(options["nominal_sample"].getCounter(options["data"], key[0]).getCounter())
        syst_item = """{:<35s}gmN {:<6d} {}\n""".format(systname, data, "".join(syst_val_str))
        datacard += syst_item

    for syst_name, proc, syst_val in options["flat_systematics"]:
        syst_val_str = []
        for index, (r, process, path) in enumerate(zip(cuts_list, processes * nchannel, paths_list)):
            if process.strip() == proc:
                syst_val_str.append(form(syst_val))
            else:
                syst_val_str.append(form("-"))
        syst_item = """{:<35s}lnN        {}\n""".format(syst_name, "".join(syst_val_str))
        datacard += syst_item

    return datacard
