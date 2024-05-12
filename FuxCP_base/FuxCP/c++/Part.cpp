#include "headers/Part.hpp"
#include "headers/Utilities.hpp"

Part::Part(const Home &hme, vector<int> cf_notes, int s, int l, int u, int succ_cst):home(hme){
    home = hme;
    size = s;
    lower_bound = l;
    upper_bound = u;
    species = 0;
    succ = succ_cst;
    notes = IntVarArray(home, size, 0, 127);
    is_not_lowest = BoolVarArray(home, size, 0, 1);
    for(int i = 0; i < size; i++){
        notes[i] = IntVar(home, cf_notes[i], cf_notes[i]);
    }
    vector_notes = {IntVarArray(home, size, 0, 127), IntVarArray(home, size, 0, 127), IntVarArray(home, size, 0, 127), IntVarArray(home, size, 0, 127)};
    for(int i = 0; i < size; i++){
        vector_notes[0][i] = IntVar(home, cf_notes[i], cf_notes[i]);
    }
    m_intervals = IntVarArray(home, size-1, 0, 12);
    m_intervals_brut = IntVarArray(home, size-1, -12, 12);
    hIntervalsCpCf = {IntVarArray(home, size, 0, 11),IntVarArray(home, size, 0, 11),IntVarArray(home, size, 0, 11),IntVarArray(home, size, 0, 11)};
    isCFB = BoolVarArray(home, size, 0, 1);
    hIntervalsBrut = IntVarArray(home, size, -127, 127);
    motions = IntVarArray(home, size-1, -1, 2);
    motions_cost = IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost}));

    succ_cost = IntVarArray(home, size-2, IntSet({0, 2}));
}

Part::Part(const Home &hme, int s, int l, int u, int sp, vector<int> cf, vector<int> splist, int con, int obl, int dir, 
    int v_type, int t_off, vector<int> scle, vector<int> b_scale, int b_mode, int triad, vector<int> off, vector<int> melodic, 
    vector<int> general_parameters):home(hme){
    voice_type = v_type;
    home = hme;
    size = s;
    lower_bound = (6 * voice_type - 6) + cf[0];
    upper_bound = (6 * voice_type + 12) + cf[0];
    species = sp;
    cantusFirmus = cf;
    speciesList = splist;
    con_motion_cost = con;
    obl_motion_cost = obl;
    dir_motion_cost = dir;
    h_triad_cost = triad;
    variety_cost = general_parameters[4];
    tone_offset = t_off;
    scale = scle;
    borrowed_scale = b_scale;
    succ = general_parameters[3];
    h_fifth = general_parameters[1];
    h_octave = general_parameters[2];
    direct_move = general_parameters[6];
    off_cst = general_parameters[0];

    step_cost = melodic[0];
    third_cost = melodic[1];
    octave_cost = melodic[7];
    fourth_cost = melodic[2];
    tritone_cost = melodic[3];
    fifth_cost = melodic[4];
    sixth_cost = melodic[5];
    seventh_cost = melodic[6];
    

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

    const vector<int> off_key = intersection(cp_range, off);

    off_scale = off_key;

    /// variable initialization todo depends on the species
    notes = IntVarArray(home, size, IntSet(extended));

    const vector<int> chrom_scale = cp_range;

    if(species==1){
        if(b_mode!=0){
            notes[size-2] = IntVar(home, IntSet(chrom_scale));
        }
    }

    vector_notes = {IntVarArray(home, size, IntSet(extended)),IntVarArray(home, size, IntSet(extended)),IntVarArray(home, size, IntSet(extended)),IntVarArray(home, size, IntSet(extended))};

    if(species==1){
        if(b_mode!=0){
            vector_notes[0][size-2] = IntVar(home, IntSet(chrom_scale));
        }
    }
    if(species==2){
        if(b_mode!=0){
            vector_notes[2][size-2] = IntVar(home, IntSet(chrom_scale));
        }
    }

    hIntervalsCpCf = {IntVarArray(home, size, 0, 11),IntVarArray(home, size, 0, 11),IntVarArray(home, size, 0, 11),IntVarArray(home, size, 0, 11)};
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
    varietyArray = IntVarArray(home, 3*(size-2), IntSet({0, variety_cost}));
    direct_move_cost = IntVarArray(home, size-2, 0, 8);
    succ_cost = IntVarArray(home, size-2, IntSet({0, 2}));
    triad_costs = IntVarArray(home, size, IntSet({0, h_triad_cost}));
    is_off = BoolVarArray(home, size, 0, 1 );
    off_costs = IntVarArray(home, size, 0, 1); //cost here is 1 for now
    m_degrees_cost = IntVarArray(home, size, IntSet({0,1,2}));
    fifth_costs = IntVarArray(home, size, IntSet(0,1));
    octave_costs = IntVarArray(home, size, IntSet(0,1));

    for(int i = 0; i < size; i++){
        BoolVar memb = BoolVar(home, 0, 1);
        for(int k = 0; k < off_scale.size(); k++){
            rel(home, notes[i], IRT_EQ, off_scale[k], Reify(memb));
        }
        rel(home, is_off[i]==memb);
    }


}

IntVarArray Part::getNotes(){
    return notes;
}