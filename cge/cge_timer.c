#include "cge.h"

CGE_TIMER cge_timers[MAX_TIMER];
int cge_timer_count;

int cge_timer_register(double t, void (*func)()) {
    if(cge_timer_count>=MAX_TIMER) return -1;
    CGE_TIMER *ct = &cge_timers[cge_timer_count];
    ct->count = ceil(t*FRAME);
    ct->stage = 0;
    ct->endcall = func;
    ct->exdata = NULL;
    return cge_timer_count++;
}

void cge_timer_fire(int timerid, void *exdata) {
    CGE_TIMER *ct = &cge_timers[timerid];
    ct->stage = ct->count;
    ct->exdata = exdata;
}

double cge_timer_getpercent(int timerid) {
    CGE_TIMER *ct = &cge_timers[timerid];
    return ct->stage * 1.0 / ct->count * 1.0;
}

void *cge_timer_getexdata(int timerid) {
    CGE_TIMER *ct = &cge_timers[timerid];
    return ct->exdata;
}

void _cge_timer_update() {
    for(int i=0; i<cge_timer_count; i++) {
        CGE_TIMER *ct = &cge_timers[i];
        if(ct->stage>0) {
            ct->stage--;
            if(ct->stage == 0) {
                if(ct->endcall) {
                    ct->endcall();
                }
            }
        }
    }
}
