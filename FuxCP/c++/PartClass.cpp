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

    for(int i = 0; i < notes.size(); i++){
        branch(*this, notes[i], INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    }
}

PartClass::PartClass(PartClass &s) : Space(s){
    //update come in here
    h_intervals = s.h_intervals;
    notes = s.notes;
    for(int i = 0; i < notes.size(); i++){
        notes[i].update(*this, s.notes[i]);
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

Search::Base<PartClass>* make_solver(PartClass* pb, int type){
    string message = "make_solver function called. type of solver :\n" + to_string(type) + "\n";
    //writeToLogFile(message.c_str());

    Gecode::Search::Options opts;
    /**@todo add here any options you want*/

    if (type == bab_solver)
        return new BAB<PartClass>(pb, opts);
    else // default case
        return new DFS<PartClass>(pb, opts);
}

/**
 * Returns the next solution space for the problem
 * @param solver a solver for the problem
 * @return an instance of the Problem class representing the next solution to the problem
 */
PartClass* get_next_solution_space(Search::Base<PartClass>* solver){
    string message = "get_next_solution_space function called.\n";
    PartClass* sol_space = solver->next();
    if (sol_space == nullptr)
        return NULL;
    //message += sol_space->toString();
    //writeToLogFile(message.c_str());
    return sol_space;
}

void PartClass::print_solution(){
    //cout << voices[0]->h_intervals[0] << endl;
    //cout << voices[0]->notes[0] << endl;
    //cout << temp_notes << endl;
    
    for(int i = 0; i < notes.size(); i++){
        cout << notes[i] << endl;
    }
}