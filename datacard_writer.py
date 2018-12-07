#!/bin/env python

import sys
import ROOT as r

class DataCardWriter:

    def __init__(self, sig=None, bgs=[], data=None, datacard_filename="datacard.txt", region_name="SR", bin_number=1, no_stat_procs=[], systs=[]):

        """
        sig                  TH1         Signal histogram where each bins are a region (bin_number will choose which one to write out)
        bgs                  [TH1s]      Background histograms in a list
        data                 TH1         Data histogram (could be set to None)
        datacard_filename    str         File name to write the datacards to
        region_name          str         Name of this region
        bin_number           int         bin number to write out from each histogram
        no_stat_procs        list        list of processes to skip statistical error provided by the nominal histograms
        systs                list        format is explained below
        [
            ('syst', 'lnN', [], {'qqWW': [TH1, TH1], 'ggWW': None, 'ggH': [TH1, TH1], 'others': None}),
              name
                     
                      lnN
                      gmN
                      or
                      etc
                            yield for gmN
                                dictionary of process -> [TH1, TH1]
                                                          up   down variation
                                                          It could also not be a list and have a one TH1 to indicate symmetric
                                                          If it set to None then it's not applied
                                                          A float can work too (e.g. below)
            ('syst', 'lnN', [], {'qqWW': float, 'ggWW': None, 'ggH': float, 'others': None}),
            ...
            ...
            ...
            ...
            ...
            # If provided is a gmN then the 3rd option has the data yield for gmN and each process has the extrapolation factors
            ('WW_norm', 'gmN', [TH1], {'qqWW': TH1, 'ggWW': TH1, 'ggH': TH1, 'others': TH1}),
            ...
            ...
            ...
        ]
        """

        self.sig = sig
        self.bgs = bgs
        self.data = data
        self.datacard_filename = datacard_filename
        self.region_name = region_name
        self.systs = systs
        self.bin_number = bin_number
        self.hists = [self.sig] + self.bgs
        self.no_stat_procs = no_stat_procs

    def set_bin(self, i):
        self.bin_number = i

    def process(self):
        self.sanity_check()
        self.preproces_data()
        self.retrieve_process_names()
        self.retrieve_rates()
        self.retrieve_errors()

    def sanity_check(self):
        """ If no signal histogram, and/or background histograms are provided throw a fit. """

        if not self.sig:
            raise ValueError("Error: No signal histogram provided for the statistics datacard writing.")

        if len(self.bgs) == 0:
            raise ValueError("Error: No background histograms provided for the statistics datacard writing.")

    def preproces_data(self):
        """ If no data histogram is provided, then set it to total background. """
        if not self.data:
            print "Warning: No data histogram provided for the statistics datacard writing."
            print "Data will be set to total bkg expectation. (Of course,rounded.)"
            fakedata = self.bgs[0].Clone()
            fakedata.Reset()
            fakedata.GetXaxis().SetCanExtend(False)
            for b in self.bgs:
                fakedata.Add(b)
            for i in xrange(1,fakedata.GetNbinsX()+2):
                b = fakedata.GetBinContent(i)
                fakedata.SetBinContent(i, int(b) if b > 0 else 0)
            self.data = fakedata

    def retrieve_process_names(self):
        """ Loop over signal and backgrounds and aggregate a list of process names. """
        """ Put together process name from TH1::SetTitle of the histogram """
        self.proc_names = []
        for h in self.hists:
            self.proc_names.append(h.GetTitle())

    def retrieve_rates(self):
        """ Loop over signal and backgrounds and aggregate the rates of each process. """
        self.rates = []
        for h in self.hists:
            self.rates.append(h.GetBinContent(self.bin_number))

    def retrieve_errors(self):
        """ Loop over signal and backgrounds and aggregate the rates of each process. """
        self.errors = []
        for h in self.hists:
            self.errors.append(h.GetBinError(self.bin_number))

    def get_delimiter(self):
        delimiter = "-" * 38
        for index in self.hists:
            delimiter += "-" * 17
        return delimiter

    def get_header_str(self):
        """ Returns a string for the header. """
        rtn  = "imax 1 number of bins\n"
        rtn += "jmax * number of processes\n"
        rtn += "kmax * number of nuisance parameters\n"
        rtn += "{}\n".format(self.get_delimiter())
        return rtn

    def get_observation_str(self):
        """ Returns a string for the observation line. """
        rtn  = "bin           {}\n".format(self.region_name)
        rtn += "observation   {}\n".format(self.data.GetBinContent(self.bin_number))
        rtn += "{}\n".format(self.get_delimiter())
        return rtn
        return rtn

    def get_rates_str(self):
        """ Returns a string of process names and rates. """
        rtn  = "bin                                     {}\n".format("{:17s}".format(self.region_name) * len(self.hists))
        rtn += "process                                 {}\n".format("".join(["{:<17d}".format(i) for i in xrange(len(self.hists))]))
        rtn += "process                                 {}\n".format("".join(["{:17s}".format(i) for i in self.proc_names]))
        rtn += "rate                                    {}\n".format("".join(["{:<17.3f}".format(i) for i in self.rates]))
        rtn += "{}\n".format(self.get_delimiter())
        return rtn

    def get_stats_str(self):
        rtn = ""
        for proc in self.proc_names:
            if len(self.no_stat_procs):
                if proc in self.no_stat_procs:
                    continue
            rtn += "{:27s}".format("{}_{}_stat".format(proc, self.region_name))
            rtn += "{:13s}".format("lnN")
            for index, iproc in enumerate(self.proc_names):
                if iproc == proc:
                    rtn += "{:<17.4f}".format(self.errors[index]/self.rates[index]+1 if self.rates[index] > 1e-3 else 1)
                else:
                    rtn += "{:17s}".format("-")
            rtn += "\n"
        return rtn

    def get_syst_str(self, syst):
        """
        Below are some example
            ('syst', 'lnN', [], {'qqWW': [TH1, TH1], 'ggWW': None, 'ggH': [TH1, TH1], 'others': None}),
            ('syst', 'lnN', [], {'qqWW': float, 'ggWW': None, 'ggH': float, 'others': None}),
            ('WW_norm', 'gmN', [TH1], {'qqWW': TH1, 'ggWW': TH1, 'ggH': TH1, 'others': TH1}),
        NOTE: if error type is gmN, the currently supports only when the extrapolation factor (alpha) is provided via TH1
        """

        """ Parse the relevant information"""
        #print syst
        systname = syst[0]
        systtype = syst[1]
        isgmNerr = len(syst[2]) != 0
        gmNyield = syst[2][0].GetBinContent(self.bin_number) if isgmNerr else ""
        systdict = syst[3]

        """ Sanity check for gmN case. Currently only supports when the extrapolation factor (alpha) is provided via TH1 """
        if isgmNerr:
            """ Need to check either it is 0 or None or r.TH1"""
            for i in systdict:
                if isinstance(systdict[i], r.TH1) or systdict[i] == None or systdict[i] == 0:
                    pass
                else:
                    print syst
                    raise ValueError("gmN error must be accompanied with extrapolation factors via TH1.")

        """ The first column is the name of the syst, and the second (or third) is the syst type"""
        rtn  = "{:27s}".format(systname)
        rtn += "{:13s}".format(systtype + "   " + (str(int(gmNyield)) if gmNyield != "" else ""))

        """ Now go through the processes and retrieve the systematic values and add that to the output string"""
        for index, proc in enumerate(self.proc_names):
            systval = systdict[proc]
            """
            systval could be one of the following
              1. [TH1, TH1]
              2. TH1
              3. [float, float]
              4. float
              5. str         # direct input
              6. [str, ... ] # direct input per bin
              7. None
            """
            """ Below is a bunch of if/else statement to characterize the input to one of 7 cases shown above"""
            if not systval:
                rtn += "{:17s}".format("-")
            elif isinstance(systval, list):
                """ Check whether this is a paired error."""
                if len(systval) != 2:
                    """ Check whether it is a direct [str, ... ] case."""
                    if len(systval) != 0:
                        if isinstance(systval[0], str):
                            """ Case 6"""
                            rtn += "{:17s}".format(systval[self.bin_number-1]) # Becuase bin_number follows American floor system and not European
                        else:
                            print systdict
                            raise ValueError("A systematic variation provided is not up/down variation. i.e. len(systval) != 2")
                    else:
                        print systdict
                        raise ValueError("A systematic variation provided is not up/down variation. i.e. len(systval) == 0")
                if isinstance(systval[0], float):
                    """ Case 3"""
                    rtn += "{:.4f}/{:.4f}    ".format(systval[0]/self.rates[index], systval[1]/self.rates[index])
                elif isinstance(systval[0], r.TH1):
                    """ Case 1"""
                    rtn += "{:.4f}/{:.4f}    ".format(systval[0].GetBinContent(self.bin_number)/self.rates[index], systval[1].GetBinContent(self.bin_number)/self.rates[index])
            elif isinstance(systval, float):
                """ Case 4"""
                rtn += "{:<17.4f}".format(systval/self.rates[index])
            elif isinstance(systval, r.TH1):
                # gmN should always be with r.TH1
                if isgmNerr:
                    if systval.GetBinContent(self.bin_number) == 0:
                        """ Case 7"""
                        rtn += "{:17s}".format("-")
                    else:
                        """ Case 2 with a twist of alpha factor"""
                        rtn += "{:<17.4f}".format(systval.GetBinContent(self.bin_number))
                else:
                    """ Case 2"""
                    rtn += "{:<17.4f}".format(systval.GetBinContent(self.bin_number)/self.rates[index])
            elif isinstance(systval, str):
                """ Case 5"""
                rtn += "{:17s}".format(systval)
            else:
                print systval
                raise ValueError("Hm? I don't know how to process this")

        return rtn

    def get_systs_str(self):
        rtn = ""
        for syst in self.systs:
            rtn += self.get_syst_str(syst) + "\n"
        return rtn

    def get_str(self):
        self.process()
        rtn  = self.get_header_str()
        rtn += self.get_observation_str()
        rtn += self.get_rates_str()
        rtn += self.get_systs_str()
        rtn += self.get_stats_str()
        return rtn

    def write(self, output_name=""):
        if output_name:
            f = open(output_name, "w")
        else:
            f = open(self.datacard_filename, "w")
        f.write(self.get_str())

class DataCardConverter:

    """
    This class converts a datacard that is utilizing histograms to fit multi-bin SR analysis into a single bin text-based data cards.
    e.g.
    dc = DataCardConverter("datacard.txt", 3)
                                           ^ choose 3rd bin of multi-bin SR histogram
    """

    """ Converts histogram based datacards to txt based datacards."""

    def __init__(self, datacard_path, bin_number=1, bin_name="SR"):
        """ The path is given when the converter is initialized."""
        self.datacard_path = datacard_path
        self.bin_number = bin_number # This is the bin number for the yield histograms
        self.bin_name = bin_name # This is the label used to label the channel

        self.open_datacard_file()
        self.get_datacard_lines()
        self.get_shape_systematic_configuration_line()
        self.retrieve_histogram_root_file_path()
        self.get_systematic_format()
        self.open_hist_rootfile()
        self.get_process_names_and_indices()
        self.process_nominal_rates()
        self.get_shape_systematic_lines()
        self.process_systematics()
        #self.print_contents()

    def get_str(self):
        return self.get_header() + self.get_rates_table() + self.get_systematic_table()

    def open_datacard_file(self):
        self.f = open(self.datacard_path)

    def get_datacard_lines(self):
        self.datacard_lines = [ x.strip() for x in self.f.readlines() ]

    def get_shape_systematic_configuration_line(self):
        """ Line that looks like "shapes * * hist.root ... " contains the configuration of the shape systematics."""
        for line in self.datacard_lines:
            if "shapes" in line:
                self.shape_config_line = line
                break

    def retrieve_histogram_root_file_path(self):
        """ The shape systematic configuration line contains the path to the rootfile histograms."""
        self.hist_rootfile_path = self.shape_config_line.split()[3]

    def get_systematic_format(self):
        """ The histogram naming convention for shape systematic histograms are defined in the shape systematic configuration line."""
        tmp = self.shape_config_line.split()[5]
        if tmp != "$PROCESS_$SYSTEMATIC":
            raise ValueError("DataCardConverter currently does not know how to deal with shape systematic histogram naming convention other than $PROCESS_$SYSTEMATIC. (user had set to {})".format(tmp))
        self.shape_systematic_format = "{PROCESS}_{SYSTEMATIC}"

    def open_hist_rootfile(self):
        self.hf = r.TFile(self.hist_rootfile_path)

    def get_process_names_and_indices(self):
        """ There are two lines that begin with "process". one of them has indices, while the other has the label for the process."""
        """ Use the fact that the line with indices always start like "process    0   1   2  .... "."""
        self.lines = []
        for line in self.datacard_lines:
            if "process" in line.split():
                self.lines.append(line)
        which_line_has_labels  = 1 if self.lines[0].split()[1] == "0" else 0
        which_line_has_indices = 0 if self.lines[0].split()[1] == "0" else 1

        """ After parsing create 3 objects: a list of process_names, a list of indices, a map of index to process name."""
        self.process_names = self.lines[which_line_has_labels].split()[1:]
        self.process_indices = [ int(x) for x in self.lines[which_line_has_indices].split()[1:] ]
        self.process_names_indices_map = {}
        for name, i in zip(self.process_names, self.process_indices):
            self.process_names_indices_map[i] = name

    def process_nominal_rates(self):
        """ Go through nominal histograms and obtain the main rates"""
        self.rates = {}
        for proc in self.process_names:
            self.rates[proc] = self.get_rate(proc)

    def get_rate(self, proc, syst=""):
        """ From the histograms, retrieve the yields."""
        if syst:
            h = self.hf.Get(self.shape_systematic_format.format(PROCESS=proc, SYSTEMATIC=syst))
        else:
            h = self.hf.Get(proc)
        return h.GetBinContent(self.bin_number)

    def get_frac_error(self, proc, syst):
        """ From the nominal and systematic histograms compute the fractional error for the datacard"""
        rate_nom = self.get_rate(proc)
        rate_sys = self.get_rate(proc, syst)
        frac_err = rate_sys / rate_nom if rate_nom != 0 else 1
        if frac_err <= 1e-3:
            frac_err = 1e-3
        return frac_err

    def get_shape_systematic_lines(self):
        """ Identify lines that refer to systematics and save them"""
        self.shape_systematic_lines = []
        for line in self.datacard_lines:
            if len(line.split()) > 1:
                if "shape" in line.split()[1]:
                    self.shape_systematic_lines.append(line)

    def process_systematics(self):
        """ Loop over the systematics"""
        self.systs = {}

        """ Loop over the shape systematic lines and create place holders for systematic values."""
        """ Nested map to hold all the systematic values (map - map - map) where it will be key'd as [proc][syst]["Up"] or [proc][syst]["Down"] """
        for line in self.shape_systematic_lines:
            syst_name = line.split()[0]
            self.systs[syst_name] = {}
            for index in self.process_names_indices_map:
                proc = self.process_names_indices_map[index]
                self.systs[syst_name][proc] = {"Up" : 1., "Down" : 1.}

        """ Now loop over the shape systematic lines and process them."""
        """ Each line will have something like "systname   shape   1   -    -   1   -  -  " """
        """ 1 means the multiplier to the systematic is 1, and - means no systematic for that process."""
        for line in self.shape_systematic_lines:
            for index, multiplier in enumerate(line.split()[2:]): # The multiplier will most likely will be 1 or -. could have other multiplier values but unlikely.
                proc = self.process_names_indices_map[index]
                syst = line.split()[0]
                if multiplier == "-": # if the multiplier is set to "-" that means this process does not have any shape systematics applied
                    self.systs[syst][proc]["Up"] = "-"
                    self.systs[syst][proc]["Down"] = "-"
                else:
                    multiplier = float(multiplier)
                    if multiplier != 1:
                        raise ValueError("DataCardConverter does not know what to do When the value set for the shape systematic is not equal to 1 or - (see your datacard for {})".format(line))
                    self.systs[syst][proc]["Up"] = self.get_frac_error(proc, syst+"Up")
                    self.systs[syst][proc]["Down"] = self.get_frac_error(proc, syst+"Down")

    def get_delimiter(self):
        delimiter = "-" * 38
        for index in self.process_names_indices_map:
            delimiter += "-" * 17
        return delimiter

    def get_header(self):
        header = ""
        header += "imax 1  number of channels\n"
        header += "jmax *  number of backgrounds ('*' = automatic)\n"
        header += "kmax *  number of nuisance parameters (sources of systematical uncertainties)\n"
        header += "{}\n".format(self.get_delimiter())
        header += "bin            {}\n".format(self.bin_name)
        header += "observation    0\n"
        header += "{}\n".format(self.get_delimiter())
        return header

    def get_rates_table(self):
        """ Print the top portion of the table where the rates are listed (and the process name)"""
        """ loop over process and print the rates"""
        proc_bin_line   = "{:38s}".format("bin")
        proc_index_line = "{:38s}".format("process")
        proc_label_line = "{:38s}".format("process")
        proc_rates_line = "{:38s}".format("rate")
        for index in self.process_names_indices_map:
            proc_name = self.process_names_indices_map[index]
            proc_bin_line   += "{:17s}".format(self.bin_name)
            proc_index_line += "{:17s}".format(str(index))
            proc_label_line += "{:17s}".format(str(proc_name))
            proc_rates_line += "{:.5f}{:10s}".format(self.rates[proc_name],"")
        rtn  = proc_bin_line + "\n"
        rtn += proc_index_line + "\n"
        rtn += proc_label_line + "\n"
        rtn += proc_rates_line + "\n"
        rtn += self.get_delimiter() + "\n"
        return rtn

    def get_systematic_table(self):
        """ Print the main systematic error table portion"""
        """ loop over systematics and process and print the table"""
        rtnstr = ""
        for syst in sorted(self.systs.keys()):
            tmpstr = "{:23s}lnN            ".format(syst)
            for index in self.process_names_indices_map:
                proc = self.process_names_indices_map[index]
                if self.systs[syst][proc]["Up"] == "-":
                    tmpstr += "{:17s}".format("-")
                else:
                    tmpstr += "{:.5f}/{:.5f}  ".format(self.systs[syst][proc]["Down"], self.systs[syst][proc]["Up"])
            rtnstr += tmpstr + "\n"
        return rtnstr

    def print_contents(self):
        print self.f
        for line in self.datacard_lines:
            print line
        print self.shape_config_line
        print self.hist_rootfile_path
        print self.shape_systematic_format
        print self.hf
        for proc in self.process_names:
            print proc
        for proc in self.process_names:
            print proc, self.rates[proc]
        for syst_line in self.shape_systematic_lines:
            print syst_line
        for proc in self.systs:
            for syst in self.systs[proc]:
                if isinstance(self.systs[proc][syst]["Up"], str):
                    print proc, syst, "{:s}".format(self.systs[proc][syst]["Up"])+"/"+"{:s}".format(self.systs[proc][syst]["Down"])
                else:
                    print proc, syst, "{:.5f}".format(self.systs[proc][syst]["Up"])+"/"+"{:.5f}".format(self.systs[proc][syst]["Down"])

if __name__ == "__main__":

    datacard_path = sys.argv[1]

    dc = DataCardConverter(datacard_path, 3)
    print dc.get_str()
