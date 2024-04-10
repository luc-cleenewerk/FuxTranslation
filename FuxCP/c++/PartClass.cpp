#include "headers/PartClass.hpp"


PartClass::PartClass(int cf_len, int species, vector<int> cf_notes){

    //branch come here
    init_m_intervals_brut(cf_len);
    notes = {IntVarArray(*this, cf_len, 0, 120),
            IntVarArray(*this, cf_len, 0, 120),
            IntVarArray(*this, cf_len, 0, 120),
            IntVarArray(*this, cf_len, 0, 120)};

    h_intervals = {};
    harmonic_intervals = IntVarArray(*this, cf_len, 0, 11);
    h_intervals.push_back(harmonic_intervals);

    for(IntVar p : notes[0]){
        for(int q : cf_notes){
            for(IntVarArray i_arr : h_intervals){
                for(IntVar i : i_arr){
                    IntVar t1 = expr(*this, p-q);
                    IntVar t2 = IntVar(*this, 0, 127);
                    abs(*this, t2, t1);
                    mod(*this, t2, IntVar(*this, 12,12), i);
                    member(*this, ALL_CONS_VAR, i);
                }
            }
        }
    }

    for(int i = 0; i < h_intervals.size(); i++){
        branch(*this, h_intervals[i], INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    }
}

PartClass::PartClass(PartClass &s) : Space(s){
    //update come in here
    h_intervals = s.h_intervals;
    for(int i = 0; i < h_intervals.size(); i++){
        h_intervals[i].update(*this, s.h_intervals[i]);
    }
    for(int i = 0; i < is_cf_lower_arr.size(); i++){
        is_cf_lower_arr[i].update(*this, s.is_cf_lower_arr[0]);
    }
    for(int i = 0; i < notes.size(); i++){
        notes[i].update(*this, s.notes[0]);
    }
}

Space* PartClass::copy(void){
    return new PartClass(*this);
};

void PartClass::init_m_intervals_brut(int cf_len){
    IntVarArray array_temp = IntVarArray(*this, cf_len-1, -12, 12);
    m_intervals_brut = {array_temp, array_temp, array_temp, array_temp};
}

vector<IntVarArray> PartClass::get_notes(){
    return notes;
}