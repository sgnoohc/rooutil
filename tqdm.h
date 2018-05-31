#ifndef tqdm_h
#define tqdm_h

#include <unistd.h>
#include <chrono>
#include <ctime>
#include <numeric>
#include <ios>
#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

class tqdm {
    public:
        std::chrono::time_point<std::chrono::system_clock> t_first = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> t_old = std::chrono::system_clock::now();
        std::vector<double> deq;
        std::vector<const char*> bars = {" ", "▏", "▎", "▍", "▋", "▋", "▊", "▉", "▉"};
        bool inscreen = system("test $STY") == 0;
        int width = 40;
        int period = 1;
        int smoothing = 100;
        unsigned long nupdates = 0;
        void progress( int curr, int tot);
};

#endif
