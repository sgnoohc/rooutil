#!/bin/env python

import plottery_wrapper as p

def plot(histname):

    p.dump_plot(
            fnames=[
                "whh_c3_1_hist.root",
                ],
            data_fname="whh_c3_9_hist.root",
            filter_pattern=histname,
            dogrep=True,
            extraoptions={
                "nbins":30,
                "ratio_range":[0., 20.],
                },
            )

if __name__ == "__main__":

    plot("Wgt__PtLead12_v4")
    plot("Wgt__PtSubLead12_v4")
    plot("Wgt__M12_v4")
    plot("Wgt__Pt0_v4")
    plot("Wgt__DeltaPhi12")
    plot("Wgt__DeltaEta12")
    plot("Wgt__DeltaR12")
