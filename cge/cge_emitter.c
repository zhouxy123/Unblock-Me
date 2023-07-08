#include "cge.h"

CGE_EMITTER cge_emitters[MAX_EMITTER];
int cge_emitter_count;

int cge_emitter_register() {
    if(cge_emitter_count>=MAX_EMITTER) return -1;
    CGE_EMITTER *et = &cge_emitters[cge_emitter_count];
    et->observer_count = 0;
    return cge_emitter_count++;
}

int cge_emitter_addob(int emitter_id, observer_handle func) {
    CGE_EMITTER *et = &cge_emitters[emitter_id];
    if(et->observer_count>=MAX_OBSERVER) return -1;
    et->observers[et->observer_count] = func;
    return et->observer_count++;

}

void cge_emitter_rmob(int emitter_id, observer_handle func) {
    CGE_EMITTER *et = &cge_emitters[emitter_id];
    observer_handle tmps[MAX_OBSERVER];
    int count = 0;
    for(int i=0; i<et->observer_count; i++) {
        if(et->observers[i] != func) {
            tmps[count] = et->observers[i];
            count++;
        }
    }
    memcpy(&et->observers[0], tmps, MAX_OBSERVER * sizeof(observer_handle));
    et->observer_count = count;
}

void cge_emitter_notify(int emitter_id, void *msg) {
    CGE_EMITTER *et = &cge_emitters[emitter_id];
    for(int i=0; i<et->observer_count; i++) {
        et->observers[i](msg);
    }
}
