#include "headers/Part.hpp"

Part::Part(const Home &hme, vector<int> cf_notes, int s, int l, int u):home(hme){
    home = hme;
    size = s;
    upper_bound = u;
    lower_bound = l;
    notes = IntVarArray(home, size, lower_bound, upper_bound);
    is_not_lowest = BoolVarArray(home, size, 0, 1);
    for(int i = 0; i < size; i++){
        notes[i] = IntVar(home, cf_notes[i], cf_notes[i]);
    }
    m_intervals = IntVarArray(home, size-1, 0, 12);
    m_intervals_brut = IntVarArray(home, size-1, -12, 12);
    hIntervalsCpCf = IntVarArray(home, size, 0, 11);
    hIntervalsBrut = IntVarArray(home, size, -127, 127);

    succ_cost = IntVarArray(home, size-2, IntSet({0, 2}));
}

Part::Part(const Home &hme, int s, int l, int u, int sp, vector<int> cf, int pcost, int mtricost, vector<int> splist, int con, int obl, int dir, 
    int var_cost):home(hme){
    home = hme;
    size = s;
    lower_bound = l;
    upper_bound = u;
    species = sp;
    cantusFirmus = cf;
    costpcons = pcost;
    costtritone = mtricost;
    speciesList = splist;
    con_motion_cost = con;
    obl_motion_cost = obl;
    dir_motion_cost = dir;
    variety_cost = var_cost;


    /// variable initialization todo depends on the species
    notes = IntVarArray(home, size, l, u);
    hIntervalsCpCf = IntVarArray(home, size, 0, 11);
    isCFB = BoolVarArray(home, size, 0, 1);
    m_intervals = IntVarArray(home, size-1, 0, 12);
    m_intervals_brut = IntVarArray(home, size-1, -12, 12);
    cf_m_intervals_brut = IntVarArray(home, size-1, -12, 12);
    P_cons_cost = IntVarArray(home, size, 0, 64);
    M_deg_cost = IntVarArray(home, size-1, 0, 64);
    motions = IntVarArray(home, size-1, -1, 2);
    motions_cost = IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost}));
    is_P_cons = BoolVarArray(home, size, 0, 1);
    is_not_lowest = BoolVarArray(home, size, 0, 1);
    hIntervalsBrut = IntVarArray(home, size, -127, 127);
    varietyArray = IntVarArray(home, 3*(size-2), IntSet({0, variety_cost}));
    direct_move_cost = IntVarArray(home, size-2, 0, 8);
    succ_cost = IntVarArray(home, size-2, IntSet({0, 2}));
}

IntVarArray Part::getNotes(){
    return notes;
}