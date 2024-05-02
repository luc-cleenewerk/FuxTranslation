#include "headers/Part.hpp"
#include "headers/Utilities.hpp"

Part::Part(const Home &hme, vector<int> cf_notes, int s, int l, int u):home(hme){
    home = hme;
    size = s;
    lower_bound = (6 * voice_type - 6) + cf_notes[0];
    upper_bound = (6 * voice_type + 12) + cf_notes[0];
    notes = IntVarArray(home, size, 0, 127);
    is_not_lowest = BoolVarArray(home, size, 0, 1);
    for(int i = 0; i < size; i++){
        notes[i] = IntVar(home, cf_notes[i], cf_notes[i]);
    }
    m_intervals = IntVarArray(home, size-1, 0, 12);
    m_intervals_brut = IntVarArray(home, size-1, -12, 12);
    hIntervalsCpCf = IntVarArray(home, size, 0, 11);
    isCFB = BoolVarArray(home, size, 0, 1);
    hIntervalsBrut = IntVarArray(home, size, -127, 127);
    motions = IntVarArray(home, size-1, -1, 2);
    motions_cost = IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost}));

    succ_cost = IntVarArray(home, size-2, IntSet({0, 2}));
}

Part::Part(const Home &hme, int s, int l, int u, int sp, vector<int> cf, int pcost, int mtricost, vector<int> splist, int con, int obl, int dir, 
    int var_cost, int v_type, int t_off, vector<int> scle, vector<int> b_scale, int b_mode):home(hme){
    voice_type = v_type;
    home = hme;
    size = s;
    lower_bound = (6 * voice_type - 6) + cf[0];
    upper_bound = (6 * voice_type + 12) + cf[0];
    species = sp;
    cantusFirmus = cf;
    costpcons = pcost;
    costtritone = mtricost;
    speciesList = splist;
    con_motion_cost = con;
    obl_motion_cost = obl;
    dir_motion_cost = dir;
    variety_cost = var_cost;
    tone_offset = t_off;
    scale = scle;
    borrowed_scale = b_scale;

    //cp_range : WORKS
    //extended : INVERTED BUT SHOULD WORK THE SAME
    
    for(int i = lower_bound; i <upper_bound; i++){
        cp_range.push_back(i);
    }
    set<int> union_set;
    for(int i = 0; i < scale.size(); i++){
        union_set.insert(scale[i]);
    }
    for(int i = 0; i < borrowed_scale.size(); i++){
        union_set.insert(borrowed_scale[i]);
    }
    for(auto it = union_set.begin(); it != union_set.end(); it++){
        union_b_scale.push_back(*it);
    }
    
    const vector<int> extended = intersection(cp_range, union_b_scale);

    /// variable initialization todo depends on the species
    notes = IntVarArray(home, size, IntSet(extended));

    const vector<int> chrom_scale = cp_range;

    if(species==1){
        if(b_mode!=0){
            notes[size-2] = IntVar(home, IntSet(chrom_scale));
        }
    }

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