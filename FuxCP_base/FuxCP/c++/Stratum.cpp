#include "headers/Stratum.hpp"

Stratum::Stratum(const Home &sp, int s, int triad):home(sp){

    home = sp;
    size = s;
    h_triad_cost = triad;


    /// variable initialization todo depends on the species
    notes = IntVarArray(home, size, 0, 127);
    hIntervalsBrut = IntVarArray(home, size, -127, 127);
    hIntervalsAbs = IntVarArray(home, size, -127, 127);
    hIntervals = IntVarArray(home, size, 0, 11);
    m_intervals_brut = IntVarArray(home, size-1, -12, 12);
    triad_costs = IntVarArray(home, size, IntSet({0, h_triad_cost}));
}