#ifndef Stratum_hpp
#define Stratum_hpp

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

using namespace Gecode;
using namespace std;

class Stratum{
public:
    int size;
    int lower_bound;
    int upper_bound;

    Home home;

    IntVarArray notes; 
    IntVarArray hIntervalsBrut;
    IntVarArray hIntervalsAbs;
    IntVarArray hIntervals;
    IntVarArray m_intervals_brut;

    Stratum(const Home &sp, int s, int l, int u);

};

#endif