#include "../headers/1st-ctp.hpp"
#include "../headers/Utilities.hpp"
#include <vector>
#include <iostream>

/*
To initialise a counterpoint, we need:

-cp-range (tone-pitch-cf, which is the first note of the cantus firmus) -> this is a range (duh)
-cp-domain
-chromatic-cp-domain
-extended-cp-domain
-off-domain
-voice-type
-species

--> RANGE IN LISP CAN BE REPRESENTED WITH AN INTVAR!!!

IntVar cp_range, IntVar cp_dom, IntVar chrom_cp_dom, IntVar ext_cp_dom, IntVar off_dom, int voice_type, int species

These are probably the best ways to cast a counterpoint structure into C++


*/

/// @brief 
/// @param counterpoint 
/// @param species 
FirstSpecies::FirstSpecies() {
    string message = "WSpace object created. ";
    
    std::cout << message;
    //branching
    //branch(*this, hintervals, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

FirstSpecies::FirstSpecies(int species, vector<int> notes) {
    string message = "WSpace object created. ";
    
    
    //branching
    //branch(*this, hintervals, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

FirstSpecies::FirstSpecies(vector<int> cpr, vector<int> cpd, vector<int> ccpd, vector<int> ecpd, 
        vector<int> o, int v, int s){
/*
    Counterpoint.cp_range = cp_range;
    Counterpoint.cp_dom = cp_dom;
    Counterpoint.chrom_cp_dom = chrom_cp_dom;
    Counterpoint.ext_cp_dom = ext_cp_dom;
    Counterpoint.off_dom = off_dom;
    Counterpoint.voice_type = voice_type;
    Counterpoint.species = species;
*/
    cp_range = IntVarArray(*this, cpr[0], cpr[1], cpr[2]);
    cp_dom = IntVarArray(*this, cpd[0], cpd[1], cpd[2]);
    chrom_cp_dom = IntVarArray(*this, ccpd[0], ccpd[1], ccpd[2]);;
    ext_cp_dom = IntVarArray(*this, ecpd[0], ecpd[1], ecpd[2]);;
    off_dom = IntVarArray(*this, o[0], o[1], o[2]);;
    voice_type = v;
    species = s;

    //constraints

    //branching
    branch(*this, cp_range, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, cp_dom, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, chrom_cp_dom, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, ext_cp_dom, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, off_dom, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

FirstSpecies::FirstSpecies(FirstSpecies& s): Space(s){
    //IntVars update
    cp_range.update(*this, s.cp_range);
    cp_dom.update(*this, s.cp_dom);
    chrom_cp_dom.update(*this, s.chrom_cp_dom);
    ext_cp_dom.update(*this, s.ext_cp_dom);
    off_dom.update(*this, s.off_dom);
}

Space* FirstSpecies::copy(void) {
    return new FirstSpecies(*this);
}

int* FirstSpecies::return_solution(){
    string message = "return_solution method. Solution : [";
    int size = cp_range.size()+cp_dom.size()+chrom_cp_dom.size()+ext_cp_dom.size()+off_dom.size();
    int* solution = new int[size];
    int j = 0;
    for(int i = 0; i < cp_range.size(); i++){
        solution[j] = cp_range[i].val();
        message += to_string(solution[j]) + " ";
        j++;
    }
    for(int i = 0; i < cp_dom.size(); i++){
        solution[j] = cp_dom[i].val();
        message += to_string(solution[j]) + " ";
        j++;
    }
    for(int i = 0; i < chrom_cp_dom.size(); i++){
        solution[j] = chrom_cp_dom[i].val();
        message += to_string(solution[j]) + " ";
        j++;
    }
    for(int i = 0; i < ext_cp_dom.size(); i++){
        solution[j] = ext_cp_dom[i].val();
        message += to_string(solution[j]) + " ";
        j++;
    }
    for(int i = 0; i < off_dom.size(); i++){
        solution[j] = off_dom[i].val();
        message += to_string(solution[j]) + " ";
        j++;
    }
    message += "]\n";
    return solution;
}

void FirstSpecies::print_solution(){
    for(int i = 0; i < cp_range.size(); i++){
        cout << cp_range[i].val() << " ";
    }
    for(int i = 0; i < cp_dom.size(); i++){
        cout << cp_dom[i].val() << " ";
    }
    for(int i = 0; i < chrom_cp_dom.size(); i++){
        cout << chrom_cp_dom[i].val() << " ";
    }
    for(int i = 0; i < ext_cp_dom.size(); i++){
        cout << ext_cp_dom[i].val() << " ";
    }
    for(int i = 0; i < off_dom.size(); i++){
        cout << off_dom[i].val() << " ";
    }
    cout << endl;
}

Base<FirstSpecies> *make_solver(FirstSpecies *pb) {
    
    Gecode::Search::Options opts;

    return new BAB<FirstSpecies>(pb, opts);
}

FirstSpecies* get_next_solution(Search::Base<FirstSpecies>* solver){
    FirstSpecies* next = solver->next();
    if(next==nullptr){
        std::cout << "SOLUTION POINTING TO NULL";
        return nullptr;
    }
    return next;
}
