#include "../../GiL_Library/c++/gecode_wrapper.cpp"
#include "problem-wrapper.hpp"
#include <iostream>

//je pense je dois accöder ces deux variables externement pour assurer le fonctionnement correcte je ne sais pas où elles sont définies
void* sp = computation_space(); 
int cf_len = 0; 

void* fux_cp_1st(void* counterpoint, int species) {

    std::cout << "########## FIRST SPECIES ##########" << std::endl;
    std::cout << "Initializing variables..." << std::endl;
    std::cout << "Creating harmonic intervals array..." << std::endl;
    
    int* vids = add_intVarArray(sp, cf_len, 0, 11);
}