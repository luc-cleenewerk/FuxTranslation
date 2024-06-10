#include "headers/Part.hpp"
#include "headers/Utilities.hpp"

Part::Part(const Home &hme, vector<int> cf_notes, int s, int succ_cst, vector<int> general_parameters):home(hme){
    home = hme;
    size = s;
    lower_bound = 1;
    upper_bound = 127;
    species = 0;
    succ = succ_cst;
    penult_rule_check = general_parameters[7];
    NINE = IntVar(home, 9, 9);
    THREE = IntVar(home, 3, 3);
    is_P_cons = BoolVarArray(home, size, 0, 1);
    is_not_lowest = BoolVarArray(home, size, 0, 1);
    vector_notes = {IntVarArray(home, size, 0, 127), IntVarArray(home, size, 0, 127), IntVarArray(home, size-1, 0, 127), IntVarArray(home, size, 0, 127)};
    for(int i = 0; i < size; i++){
        vector_notes[0][i] = IntVar(home, cf_notes[i], cf_notes[i]);
    }
    m_intervals = {IntVarArray(home, size-1, 0, 12),IntVarArray(home, size-1, 0, 12),IntVarArray(home, size-1, 0, 12),IntVarArray(home, size-1, 0, 12)};
    m_intervals_brut = {IntVarArray(home, size-1, -12, 12),IntVarArray(home, size-1, -12, 12),IntVarArray(home, size-1, -12, 12),IntVarArray(home, size-1, -12, 12)};
    hIntervalsCpCf = {IntVarArray(home, size, 0, 11),IntVarArray(home, size-1, 0, 11),IntVarArray(home, size-1, 0, 11),IntVarArray(home, size-1, 0, 11)};
    isCFB = {BoolVarArray(home, size, 0, 1),BoolVarArray(home, size, 0, 1),BoolVarArray(home, size, 0, 1),BoolVarArray(home, size, 0, 1)};
    hIntervalsBrut = IntVarArray(home, size, -127, 127);
    motions = {IntVarArray(home, size-1, -1, 2),IntVarArray(home, size-1, -1, 2),IntVarArray(home, size-1, -1, 2),IntVarArray(home, size-1, -1, 2)};
    motions_cost = {IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost})),
        IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost})),
        IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost})),
        IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost}))};

    succ_cost = IntVarArray(home, size-2, IntSet({0, 2}));
}

Part::Part(const Home &hme, int s, int sp, vector<int> cf, vector<int> splist, int con, int obl, int dir, 
    int v_type, int t_off, vector<int> scle, vector<int> b_scale, int b_mode, int triad, vector<int> melodic, 
    vector<int> general_parameters, vector<int> chrom, vector<int> specific):home(hme){
    voice_type = v_type;
    home = hme;
    size = s;
    lower_bound = (6 * voice_type - 6) + cf[0];
    upper_bound = (6 * voice_type + 12) + cf[0];
    // cout << "vtype : " << v_type << " , lb, ub : " << lower_bound << " , " << upper_bound << endl;
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
    penult_rule_check = general_parameters[7];

    step_cost = melodic[0];
    third_cost = melodic[1];
    octave_cost = melodic[7];
    fourth_cost = melodic[2];
    tritone_cost = melodic[3];
    fifth_cost = melodic[4];
    sixth_cost = melodic[5];
    seventh_cost = melodic[6];

    NINE = IntVar(home, 9, 9);
    THREE = IntVar(home, 3, 3);
    is_h1_poss = BoolVar(home, 0 ,1);
    is_h2_poss = BoolVar(home, 0, 1);
    is_not_triad = BoolVar(home, 0, 1);

    penult_sixth_cost = specific[0];
    cambiatta_cost = specific[1];
    m2_zero_cost = specific[4];

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

    const vector<int> chrom_scale = chrom;

    vector<int> o = {};

    for(int c = 0; c < chrom_scale.size(); c++){
        int t = 0;
        for(int sc = 0; sc < scale.size(); sc++){
            if(chrom_scale[c]==scale[sc]){
                t = 1;
                break;
            }
        }
        if(t==0){
            o.push_back(chrom_scale[c]);
        }
    }

    const vector<int> off_key = intersection(cp_range, o);

    off_scale = off_key;

    // last measure is a whole note, so size-1 for the three last IntVarArrays. 
    vector_notes = {IntVarArray(home, size, IntSet(extended)),IntVarArray(home, size-1, IntSet(extended)),IntVarArray(home, size-1, IntSet(extended)),IntVarArray(home, size-1, IntSet(extended))};

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
    if(species==3){
        if(b_mode!=0){
            vector_notes[3][size-2] = IntVar(home, IntSet(chrom_scale));
        }
    }
    create_solution_array();
    hIntervalsCpCf = {IntVarArray(home, size, 0, 11),IntVarArray(home, size-1, 0, 11),IntVarArray(home, size-1, 0, 11),IntVarArray(home, size-1, 0, 11)};
    isCFB = {BoolVarArray(home, size, 0, 1),BoolVarArray(home, size, 0, 1),BoolVarArray(home, size, 0, 1),BoolVarArray(home, size, 0, 1)};
    m_intervals = {IntVarArray(home, size-1, 0, 12),IntVarArray(home, size-1, 0, 12),IntVarArray(home, size-1, 0, 12),IntVarArray(home, size-1, 0, 12)};
    m_intervals_brut = {IntVarArray(home, size-1, -12, 12),IntVarArray(home, size-1, -12, 12),IntVarArray(home, size-1, -12, 12),IntVarArray(home, size-1, -12, 12)};
    motions = {IntVarArray(home, size-1, -1, 2),IntVarArray(home, size-1, -1, 2),IntVarArray(home, size-1, -1, 2),IntVarArray(home, size-1, -1, 2)};
    motions_cost = {IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost})),
        IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost})),
        IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost})),
        IntVarArray(home, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost}))};
    is_P_cons = BoolVarArray(home, size, 0, 1);
    is_not_lowest = BoolVarArray(home, size, 0, 1);
    varietyArray = IntVarArray(home, 3*(size-2), IntSet({0, variety_cost}));
    direct_move_cost = IntVarArray(home, size-2, IntSet({0, direct_move}));
    succ_cost = IntVarArray(home, size-2, IntSet({0, 2}));
    triad_costs = IntVarArray(home, size, IntSet({0, h_triad_cost}));
    is_off = BoolVarArray(home, sol_len, 0, 1);
    off_costs = IntVarArray(home, sol_len, IntSet({0, off_cst})); 
    m_degrees_cost = {IntVarArray(home, size-1, IntSet({0,1,2})),IntVarArray(home, size-1, IntSet({0,1,2})),IntVarArray(home, size-1, IntSet({0,1,2})),IntVarArray(home, size-1, IntSet({0,1,2}))};
    fifth_costs = {IntVarArray(home, size, IntSet(0,1)),IntVarArray(home, size, IntSet(0,1)),IntVarArray(home, size-1, IntSet(0,1)),IntVarArray(home, size, IntSet(0,1))};
    octave_costs = {IntVarArray(home, size, IntSet(0,1)),IntVarArray(home, size, IntSet(0,1)),IntVarArray(home, size-1, IntSet(0,1)),IntVarArray(home, size, IntSet(0,1))};
    penult_sixth = IntVar(home, IntSet({0, penult_sixth_cost}));

    //3rd species variables init
    hIntervalsToCf = {IntVarArray(home, size, 0, 11),IntVarArray(home, size-1, 0, 11),IntVarArray(home, size-1, 0, 11),IntVarArray(home, size-1, 0, 11)};

    if(species==1){
        create_member_array(0);
    } else if(species==2){
        create_member_array(2);
        cout << sol_len << endl;
    }
}

void Part::create_member_array(int idx){
    //creates is_off with the index being the same as the notes in the solution array, watch out for species higher than 1
        for(int i = 0; i < sol_len; i++){
            IntVarArray res = IntVarArray(home, off_scale.size(), 0, 1);
            IntVar sm = IntVar(home, 0, off_scale.size());
            for(int l = 0; l < off_scale.size(); l++){      // TODO il y a d'office une meilleure manière de faire que double boucle for
                BoolVar b1 = BoolVar(home, 0, 1);
                rel(home, solution_array[i], IRT_EQ, off_scale[l], Reify(b1));   // REIFY RM_PMI?
                ite(home, b1, IntVar(home, 1, 1), IntVar(home, 0, 0), res[l]);
            }
            IntVarArgs x(res.size());
            for(int t = 0; t < off_scale.size(); t++){
                x[t] = res[t];
            }
            rel(home, sm, IRT_EQ, expr(home, sum(x)));
            rel(home, sm, IRT_GR, 0, Reify(is_off[i]));  // REIFY RM_PMI?
        }
}

void Part::create_solution_array(){
    if(species==1){
        sol_len = size;
        solution_array = IntVarArray(home, sol_len, 0, 127);
        for(int n = 0; n < size; n++){
            solution_array[n] = vector_notes[0][n];
        }
    } else if(species==2){
        sol_len = size + (size-1);
        solution_array = IntVarArray(home, sol_len, 0, 127);
        int idx = 0;
        for(int n = 0; n < sol_len; n+=2){
            solution_array[n] = vector_notes[0][idx];
            idx++;
        }
        idx=0;
        for(int n = 1; n < sol_len; n+=2){
            solution_array[n] = vector_notes[2][idx];
            idx++;
        }
    } else if(species==3){
        sol_len = size + (3*(size-1));
        solution_array = IntVarArray(home, sol_len, 0, 127);
        int idx = 0;
        for(int n = 0; n < sol_len; n+=4){
            solution_array[n] = vector_notes[0][idx];
            idx++;
        }
        idx=0;
        for(int n = 1; n < sol_len; n+=4){
            solution_array[n] = vector_notes[1][idx];
            idx++;
        }
        idx=0;
        for(int n = 2; n < sol_len; n+=4){
            solution_array[n] = vector_notes[2][idx];
            idx++;
        }
        idx=0;
        for(int n = 3; n < sol_len; n+=4){
            solution_array[n] = vector_notes[3][idx];
            idx++;
        }
    }
}

void Part::create_is_cons(){
    for(int h = 0; h < 4; h++){
        for(int i = 0; i < is_consonant[h].size(); i++){
            IntVarArray res = IntVarArray(home, CONSONANCES.size(), 0, 1);
            IntVar sm = IntVar(home, 0, CONSONANCES.size());
            for(int l = 0; l < CONSONANCES.size(); l++){
                BoolVar b1 = BoolVar(home, 0, 1);
                rel(home, hIntervalsCpCf[h][i], IRT_EQ, CONSONANCES[l], Reify(b1));
                ite(home, b1, IntVar(home, 1, 1), IntVar(home, 0, 0), res[l]);
            }
            IntVarArgs x(res.size());
            for(int t = 0; t < CONSONANCES.size(); t++){
                x[t] = res[t];
            }
            rel(home, sm, IRT_EQ, expr(home, sum(x)));
            rel(home, sm, IRT_GR, 0, Reify(is_consonant[h][i]));
        }
    }
}