#include "tqdm.h"

void tqdm::progress( int curr, int tot) {
    if(curr%period == 0) {
        nupdates++;
        auto now = std::chrono::system_clock::now();
        double dt = ((std::chrono::duration<double>)(now - t_old)).count();
        double dt_tot = ((std::chrono::duration<double>)(now - t_first)).count();
        t_old = now;
        if (deq.size() >= smoothing) deq.erase(deq.begin());
        deq.push_back(dt);
        double avgdt = std::accumulate(deq.begin(),deq.end(),0.)/deq.size();
        float prate = (float)period/avgdt;
        // learn an appropriate period length to avoid spamming stdout
        // and slowing down the loop (try to update ~100 times a second
        // with a period that is a power of 10)
        if (nupdates > 10) {
            period = (int)( std::min(std::max(0.2*pow(10,floor(log10(curr/dt_tot))),10.0), 1e5));
        }
        float peta = (tot-curr)/prate;
        if (isatty(1)) {
            float pct = (float)curr/(tot*0.01);
            if( ( tot - curr ) <= period ) pct = 100.0;

            printf("\015 \033[32m ");
            float fills = ((float)curr / tot * width);
            int ifills = (int)fills;
            for (int i = 0; i < ifills; i++) {
                std::cout << bars[8];
                // printf("%s",bars[8]);
            }
            if (!inscreen) printf("%s",bars[(int)(8.0*(fills-ifills))]);
            for (int i = 0; i < width-ifills-1; i++) {
                std::cout << bars[0];
                // printf("%s",bars[0]);
            }
            printf(" \033[1m\033[31m%4.1f%% \033[34m ", pct);
            printf("[%d | %.2f kHz | %.0fs<%.0fs] ", curr,  prate/1000.0, dt_tot, peta);
            printf("\033[0m\033[32m\033[0m\015 ");
            if( ( tot - curr ) > period ) fflush(stdout);
            else std::cout << std::endl;
        }
    }
}
