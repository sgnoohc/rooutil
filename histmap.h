#ifndef histmap_h
#define histmap_h

#include "TFile.h"
#include "TH1.h"
#include "stringutil.h"
#include "printutil.h"

namespace RooUtil
{
    class HistMap
    {
        public:
            TFile* file;
            TH1* hist;
            int dimension;
            HistMap(TString histpath);
            ~HistMap();
            double eval(double);
            double eval(double, double);
            double eval(double, double, double);

    };
}


#endif
