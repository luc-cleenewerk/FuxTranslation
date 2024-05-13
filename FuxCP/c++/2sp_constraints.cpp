#include "headers/2sp_constraints.hpp"
#include "headers/Utilities.hpp"

void link_harmonic_arrays_2nd_species(const Home &home, int size, Part part, vector<Stratum> lowest){

    for(int i = 0; i < size; i++){
        rel(home, part.hIntervalsCpCf[2][i] == abs(part.vector_notes[2][i] - lowest[0].notes[i])%12); //assigns the hIntervals     
        rel(home, part.hIntervalsBrut[i] == lowest[0].notes[i]-part.vector_notes[0][i]);
        abs(home, part.hIntervalsBrut[i], part.hIntervalsAbs[i]); 
    }

    }