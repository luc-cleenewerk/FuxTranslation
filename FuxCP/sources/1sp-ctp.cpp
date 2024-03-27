#include "../headers/1st-ctp.hpp"
#include "../headers/Utilities.hpp"
#include "../../GiL_Library/c++/space_wrapper.cpp"

FirstSpecies::FirstSpecies(int s, int l, int u, int species=1) {
    string message = "WSpace object created. ";
    size = s;
    lower_bound_domain = l;
    upper_bound_domain = u;
    message += "size = " + to_string(size) +  " domain : [" + to_string(lower_bound_domain) + "," +
            to_string(upper_bound_domain) + "].\n";

    // variable initialization
    hintervals = IntVarArray(*this, size, 0, 11);

    //add_h_cons_cst(this, s, 0, hintervals);

    //constraints
    distinct(*this, hintervals);

    //ALL HARMONIC INTERVALS MUST BE CONSONANCES
    for(int i = 0; i < sizeof(hintervals); i++){
        for(int j = hintervals[i].min(); j <= hintervals[i].max(); j++){
            set_member(this, sizeof(ALL_CONS), ALL_CONS, j);
        }
    }

    //branching
    //branch(*this, hintervals, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

FirstSpecies::FirstSpecies(FirstSpecies& s): Space(s){
    //IntVars update
    size = s.size;
    lower_bound_domain = s.lower_bound_domain;
    upper_bound_domain = s.upper_bound_domain;
    hintervals.update(*this, s.hintervals);
}

Space* FirstSpecies::copy(void) {
    return new FirstSpecies(*this);
}

