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

#______________________________________________________________________________
def submit_metis(job_tag, samples_map, arguments_map="", exec_script="metis.sh", tar_files=[], hadoop_dirname="testjobs", files_per_output=1, globber="*.root"):

    import time
    import json
    import metis

    from time import sleep

    from metis.Sample import DirectorySample
    from metis.CondorTask import CondorTask

    from metis.StatsParser import StatsParser

    import os
    import glob
    import subprocess


    # file/dir paths
    main_dir             = os.getcwd()
    metis_path           = os.path.dirname(os.path.dirname(metis.__file__))
    tar_path             = os.path.join(metis_path, "package.tar")
    tar_gz_path          = tar_path + ".gz"
    metis_dashboard_path = os.path.join(metis_path, "dashboard")
    exec_path            = os.path.join(main_dir, exec_script)
    hadoop_path          = "metis/{}/{}".format(hadoop_dirname, job_tag) # The output goes to /hadoop/cms/store/user/$USER/"hadoop_path"

    # Create tarball
    os.chdir(main_dir)
    print os.getcwd()
    print "tar -chzf {} {}".format(tar_gz_path, " ".join(tar_files))
    os.system("tar -chzf {} {}".format(tar_gz_path, " ".join(tar_files)))

    # Change directory to metis
    os.chdir(metis_path)

    total_summary = {}

    samples_to_run = []
    for key in samples_map:
        samples_to_run.append(
                DirectorySample(
                    dataset=key,
                    location=samples_map[key],
                    globber=globber,
                    )
                )

    # Loop over datasets to submit
    while True:

        all_tasks_complete = True

        #for sample in sorted(samples):
        for sample in samples_to_run:

            # define the task
            maker_task = CondorTask(
                    sample               = sample,
                    tag                  = job_tag,
                    arguments            = arguments_map[sample.get_datasetname()],
                    executable           = exec_path,
                    tarfile              = tar_gz_path,
                    special_dir          = hadoop_path,
                    output_name          = "output.root",
                    files_per_output     = files_per_output,
                    condor_submit_params = {"sites" : "T2_US_UCSD"},
                    open_dataset         = False,
                    flush                = True,
                    #no_load_from_backup  = True,
                    )


            # process the job (either submits, checks for resubmit, or finishes etc.)
            maker_task.process()

            # save some information for the dashboard
            total_summary[maker_task.get_sample().get_datasetname()] = maker_task.get_task_summary()

            # Aggregate whether all tasks are complete
            all_tasks_complete = all_tasks_complete and maker_task.complete()


        # parse the total summary and write out the dashboard
        StatsParser(data=total_summary, webdir=metis_dashboard_path).do()

        # Print msummary table so I don't have to load up website
        os.system("msummary -r | tee summary.txt")
        os.system("chmod -R 755 {}".format(metis_dashboard_path))

        # If all done exit the loop
        if all_tasks_complete:
            print ""
            print "Job={} finished".format(job_tag)
            print ""
            break

        # Neat trick to not exit the script for force updating
        print 'Press Ctrl-C to force update, otherwise will sleep for 300 seconds'
        try:
            for i in range(0,60):
                sleep(1) # could use a backward counter to be preeety :)
        except KeyboardInterrupt:
            raw_input("Press Enter to force update, or Ctrl-C to quit.")
            print "Force updating..."

if __name__ == "__main__":
    main()

#eof
