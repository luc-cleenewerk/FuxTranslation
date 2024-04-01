#include "../headers/1st-ctp.hpp"
#include "../headers/Utilities.hpp"
#include <vector>
#include <iostream>

/// @brief 
/// @param counterpoint 
/// @param species 
FirstSpecies::FirstSpecies(int counterpoint, int species) {
    string message = "WSpace object created. ";
    
    std::cout << message;
    //branching
    //branch(*this, hintervals, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

FirstSpecies::FirstSpecies(FirstSpecies& s): Space(s){
    //IntVars update
    size = s.size;
    lower_bound_domain = s.lower_bound_domain;
    upper_bound_domain = s.upper_bound_domain;
}

Space* FirstSpecies::copy(void) {
    return new FirstSpecies(*this);
}

