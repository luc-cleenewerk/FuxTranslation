#include "../headers/Utilities.hpp"

PartClass::PartClass(int cf_len){

    init_m_intervals_brut(cf_len);
    h_intervals={};

}

void* PartClass::init_m_intervals_brut(int cf_len){
    IntVarArray array_temp = IntVarArray(*this, cf_len-1, -12, 12);
    m_intervals_brut = {array_temp, array_temp, array_temp, array_temp};
}

void* PartClass::create_h_intervals(int cf_len, PartClass *lowest){
    h_intervals[0] = IntVarArray(*this, cf_len-1, 0, 11); //corresponds to line 22 in original 1st-ctp.lisp
    IntVarArray note = notes[0];
    IntVarArray h_int = h_intervals[0];
    IntVarArray low = lowest->get_notes()[0];
    for(IntVar p : note){
        for(IntVar q : low){
            for(IntVar i : h_int){
                
            }
        }
    }
}

vector<IntVarArray> PartClass::get_notes(){
    return notes;
}

Base<PartClass> *make_solver(PartClass *pb) {
    
    Gecode::Search::Options opts;

    return new BAB<PartClass>(pb, opts);
}

PartClass* get_next_solution(Search::Base<PartClass>* solver){
    PartClass* next = solver->next();
    if(next==nullptr){
        std::cout << "SOLUTION POINTING TO NULL";
        return nullptr;
    }
    return next;
}
