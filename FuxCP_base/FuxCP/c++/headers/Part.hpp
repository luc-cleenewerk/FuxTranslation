#ifndef PART_hpp
#define PART_hpp

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

using namespace Gecode;
using namespace std;

class Part{
public:
    int size;
    int lower_bound;
    int upper_bound;
    int species;
    int costpcons;
    int costtritone;
    int con_motion_cost;
    int obl_motion_cost;
    int dir_motion_cost;
    int variety_cost;

    Home home;

    vector<int> cantusFirmus;
    vector<int> speciesList;

    IntVarArray notes; 
    IntVarArray hIntervalsCpCf;
    BoolVarArray isCFB;
    IntVarArray m_intervals;
    IntVarArray m_intervals_brut;
    IntVarArray cf_m_intervals_brut;
    IntVarArray P_cons_cost;
    IntVarArray M_deg_cost;
    IntVarArray motions;
    IntVarArray motions_cost;
    BoolVarArray is_P_cons;
    BoolVarArray is_not_lowest;
    IntVarArray hIntervalsBrut;
    IntVarArray hIntervalsAbs;
    IntVarArray varietyArray;
    IntVarArray direct_move_cost;
    IntVarArray succ_cost;

    Part(const Home &sp, const vector<int> cf_notes, int s, int l, int u);

    Part(const Home &hme, int s, int l, int u, int sp, vector<int> cf, int pcost, int mtricost, vector<int> speciesList, int con, int obl, 
        int dir, int var_cost);

    IntVarArray getNotes();
};

#endif