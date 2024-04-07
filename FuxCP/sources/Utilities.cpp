#include "../headers/Utilities.hpp"

PartClass::PartClass(int cf_len){

    //branch come here

    init_m_intervals_brut(cf_len);
    notes = {IntVarArray(*this, cf_len-1, 0, 120),
            IntVarArray(*this, cf_len-1, 0, 120),
            IntVarArray(*this, cf_len-1, 0, 120),
            IntVarArray(*this, cf_len-1, 0, 120)};

}

PartClass::PartClass(PartClass &s) : Space(s){
    //update come in here
    for(int i = 0; i < h_intervals.size(); i++){
        h_intervals[i].update(*this, s.h_intervals[0]);
    }
    for(int i = 0; i < is_cf_lower_arr.size(); i++){
        is_cf_lower_arr[i].update(*this, s.is_cf_lower_arr[0]);
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

void PartClass::set_notes(int n){ //setting them to the value of 1 for the sake of testing
    notes = {IntVarArray(*this, 16, 0, 120),
    IntVarArray(*this, 16, 0, 120),
    IntVarArray(*this, 16, 0, 120),
    IntVarArray(*this, 16, 0, 120)};

    for(int i = 0; i < notes.size(); i++){
        for(int j = 0; j < 16; j++){
            Gecode::rel(*this, notes[i][j] == n);
        }
    }
}

string intVar_to_string(const IntVar &var, bool absolute) {
    if (var.assigned()){
        if(absolute)
            return to_string(abs(var.val()));
        return to_string(var.val());
    }
    return "<not assigned>";
}

string intVarArray_to_string(IntVarArray vars){
    int s = vars.size();
    string res = "{";
    for(int i = 0; i < s; i++){
        res += intVar_to_string(vars[i], false);
        if(i != s - 1)
            res += ", ";
    }
    res += "}";
    return res;
}