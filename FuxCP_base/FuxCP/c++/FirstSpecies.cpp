/*#include "headers/FirstSpecies.hpp"

FirstSpecies::FirstSpecies(int s, int l, int u, int sp, vector<int> cf) {
    string message = "WSpace object created. ";
    size = s;
    lower_bound_domain = l;
    upper_bound_domain = u;
    species = sp;
    cantusFirmus = cf;
    
    // variable initialization todo depends on the species
    cp = IntVarArray(*this, size, l, u);
    h_intervals = IntVarArray(*this, size, 0, 11);

    //constraints todo depends on the cantus firmus
    distinct(*this, cp);

    //create_h_intervals
    for(IntVar p : cp){
        for(int q : cf){
            for(IntVar i : h_intervals){
                IntVar t1 = expr(*this, p - q);
                IntVar t2 = IntVar(*this, 0, 127);
                abs(*this, t1, t2);
                mod(*this, t2, IntVar(*this, 12, 12), i);
            }
        }
    }

    //CONSTRAINT 1
    for(int i = 0; i < size; i++){
        member(*this, IntVarArgs({IntVar(*this, 0, 0), IntVar(*this, 3,3), IntVar(*this, 4,4), IntVar(*this, 7, 7),
                IntVar(*this, 8,8),IntVar(*this, 9,9)}), h_intervals[i]);
    }

    //branching
    branch(*this, cp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, h_intervals, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    writeToLogFile(message.c_str());
}

FirstSpecies::FirstSpecies(FirstSpecies& s): Space(s){
    //IntVars update
    size = s.size;
    lower_bound_domain = s.lower_bound_domain;
    upper_bound_domain = s.upper_bound_domain;
    cp = s.cp;
    h_intervals = s.h_intervals;
    cp.update(*this, s.cp);
    h_intervals.update(*this, s.h_intervals);
}

int FirstSpecies::getSize(){
    string message = "getSize function called. size = " + to_string(size) + "\n";
    writeToLogFile(message.c_str());
    return size;
}

int* FirstSpecies::return_solution(){
    string message = "return_solution method. Solution : [";
    int* solution = new int[size];
    for(int i = 0; i < size; i++){
        solution[i] = cp[i].val();
        message += to_string(solution[i]) + " ";
    }
    message += "]\n";
    writeToLogFile(message.c_str());
    return solution;
}

Space* FirstSpecies::copy(void) {
    return new FirstSpecies(*this);
}

void FirstSpecies::constrain(const Space& _b) {
    const FirstSpecies &b = static_cast<const FirstSpecies &>(_b);
}

void FirstSpecies::print_solution(){
    for(int i = 0; i < size; i++){
        cout << cp[i].val() << " ";
    }
    cout << endl;
    for(int i = 0; i < size; i++){
        cout << h_intervals[i].val() << " ";
    }
    cout << endl;
}

Search::Base<FirstSpecies>* make_solver(FirstSpecies* pb, int type){
    string message = "make_solver function called. type of solver :\n" + to_string(type) + "\n";
    writeToLogFile(message.c_str());

    Gecode::Search::Options opts;
    /**@todo add here any options you want

    if (type == bab_solver)
        return new BAB<FirstSpecies>(pb, opts);
    else // default case
        return new DFS<FirstSpecies>(pb, opts);
}

/**
 * Returns the next solution space for the problem
 * @param solver a solver for the problem
 * @return an instance of the Problem class representing the next solution to the problem
 
FirstSpecies* get_next_solution_space(Search::Base<FirstSpecies>* solver){
    string message = "get_next_solution_space function called.\n";
    FirstSpecies* sol_space = solver->next();
    if (sol_space == nullptr)
        return NULL;
    message += sol_space->toString();
    writeToLogFile(message.c_str());
    return sol_space;
}*/