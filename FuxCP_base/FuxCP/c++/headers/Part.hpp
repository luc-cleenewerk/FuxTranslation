#ifndef PART_hpp
#define PART_hpp

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

#include <set>

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
    int voice_type;
    int tone_offset;
    int h_triad_cost;
    int step_cost;
    int third_cost;
    int fourth_cost;
    int tritone_cost;
    int fifth_cost;
    int sixth_cost;
    int seventh_cost;
    int octave_cost;

    Home home;

    vector<int> cantusFirmus;
    vector<int> speciesList;
    vector<int> scale;
    vector<int> borrowed_scale;
    vector<int> cp_range;
    vector<int> union_b_scale;
    vector<int> off_scale;

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
    IntVarArray triad_costs;
    BoolVarArray is_off;
    IntVarArray off_costs;
    IntVarArray m_degrees_cost;
    IntVarArray fifth_costs;
    IntVarArray octave_costs;

    Part(const Home &sp, const vector<int> cf_notes, int s, int l, int u);

    Part(const Home &hme, int s, int l, int u, int sp, vector<int> cf, int pcost, int mtricost, vector<int> speciesList, int con, int obl, 
        int dir, int var_cost, int voice_type, int t_off, vector<int> scle, vector<int> b_scale, int b_mode, int triad, vector<int> off);

    IntVarArray getNotes();
};

#endif