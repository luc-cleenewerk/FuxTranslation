#include "../headers/Utilities.hpp"

/*void* add_h_cons_cst(void* sp, int len, int penult_index, IntVarArray h_intervals, int *penult_dom_var = PENULT_CONS_VAR){
    for(int i = 0; i < len; i++){
        IntVar h_interval = h_intervals[i];
        if(i == penult_index){
            add_penult_dom_cst(sp, h_interval, penult_dom_var);
            if(h_interval != NULL){
                set_member(sp, sizeof(ALL_CONS), ALL_CONS, h_interval);
            }
        }
    }
}

void* add_penult_dom_cst(void* sp, IntVar h_interval, int *penult_dom_var){
    if(PENULT_RULE_CHECK){
        set_member(sp, sizeof(penult_dom_var), penult_dom_var);
    }
}/*

void* create_h_intervals(void* sp, IntVarArray cp, IntVarArray cf, IntVarArray h_intervals){
    for(int p = 0; p < cp.size(); p++){
        for(int q = 0; q < cf.size(); q++){
            for(int i = 0; i < h_intervals.size(); i++){
                rel(sp, abs(cp[p]-cf[q])%12, IRT_EQ, h_intervals[i]);
            }
        }
    }
}*/
