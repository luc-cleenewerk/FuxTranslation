#include "headers/Stratum.hpp"

Stratum::Stratum(const Home &sp, int s, int l, int u):home(sp){

    home = sp;
    size = s;
    lower_bound = l;
    upper_bound = u;


    /// variable initialization todo depends on the species
    notes = IntVarArray(home, size, l, u);
    hIntervalsBrut = IntVarArray(home, size, -127, 127);
    hIntervalsAbs = IntVarArray(home, size, -127, 127);
    hIntervals = IntVarArray(home, size, 0, 11);
    m_intervals_brut = IntVarArray(home, size-1, -12, 12);
}