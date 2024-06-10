#include "headers/all_constraints.hpp"
#include "headers/Utilities.hpp"

void general_3v(const Home &home, vector<Part> parts, vector<Stratum> lowest, IntVarArray succ_cost, IntVarArray triad_cost){
    voices_cannot_play_same_note(home, parts);

    no_successive_ascending_sixths(home, parts[1].size, parts);

    key_tone_tuned_to_cantusfirmus(home, parts[0], lowest);

    no_same_direction_3v(home, parts[1].size, parts);  

    avoid_perfect_consonances(home, parts[1].size, parts, succ_cost);

    prefer_harmonic_triads(home, parts, triad_cost);
}

void general_4v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVarArray succ_cost, IntVarArray triad_cost){

    // H4 (and G10)
    key_tone_tuned_to_cantusfirmus(home, parts[0], lowest);

    // P7
    no_successive_ascending_sixths(home, parts[1].size, parts);

    // P6
    no_same_direction_4v(home, parts[1].size, parts);

    // P4
    avoid_perfect_consonances(home, parts[1].size, parts, succ_cost);

    // H8
    prefer_harmonic_triads_4v(home, upper, triad_cost);  // TODO add last measure allows doubling the octave
}

/**
  * Link the harmonic intervals arrays for the first species
  * @param home the problem
  * @param size the number of notes in the cf
  * @param cp the variable array for the counterpoint to be generated
  * @param hIntervalsCpCf the variable array for the harmonic intervals between the cp and the cf
  * @param cantusFirmus the cantus firmus
  */
void link_harmonic_arrays_1st_species(const Home &home, Part part, vector<Stratum> lowest, vector<Stratum> upper, int list_index){
    for(int i = 0; i < part.size; i++){
        rel(home, part.hIntervalsCpCf[0][i] == abs(part.vector_notes[0][i] - lowest[0].notes[i])%12); //assigns the hIntervals
    }
}

void link_cfb_arrays_1st_species(const Home &home, int size, Part part, Part cf, int idx){
    for(int i = 0; i < size; i++){
        rel(home, part.vector_notes[idx][i], IRT_GQ, cf.vector_notes[0][i], Reify(part.isCFB[idx][i])); //links the CFB values in the array
    }
}

void link_melodic_arrays_1st_species(const Home &home, Part part){
    for(int i = 0; i < part.size-1; i++){
        rel(home, expr(home, part.vector_notes[0][i+1]-part.vector_notes[0][i]), IRT_EQ, part.m_intervals_brut[0][i]); //assigns the melodic interval (brut)
        abs(home, part.m_intervals_brut[0][i], part.m_intervals[0][i]); //assigns the melodic interval (absolute)
    }
}

void link_motions_arrays(const Home &home, Part part, vector<Stratum> lowest, int idx){ 
            for(int i = 0; i < part.size-1; i++){
                //direct motions help creation
                BoolVar both_up = expr(home, (part.m_intervals_brut[idx][i]>0)&&(lowest[0].m_intervals_brut[i]>0)); //if both parts are going in the same direction
                BoolVar both_stay = expr(home, (part.m_intervals_brut[idx][i]==0)&&(lowest[0].m_intervals_brut[i]==0)); //if both parts are staying
                BoolVar both_down = expr(home, (part.m_intervals_brut[idx][i]<0)&&(lowest[0].m_intervals_brut[i]<0)); //if both parts are going down
                //oblique motions help creation
                BoolVar cf_stays_1 = expr(home, (part.m_intervals_brut[idx][i]>0)&&(lowest[0].m_intervals_brut[i]==0)); //if the lowest part stays and one goes up
                BoolVar cf_stays_2 = expr(home, (part.m_intervals_brut[idx][i]<0)&&(lowest[0].m_intervals_brut[i]==0)); //if the lowest part stays and one goes down
                BoolVar cp_stays_1 = expr(home, (part.m_intervals_brut[idx][i]==0)&&(lowest[0].m_intervals_brut[i]>0)); //if the lowest part goes up and one stays
                BoolVar cp_stays_2 = expr(home, (part.m_intervals_brut[idx][i]==0)&&(lowest[0].m_intervals_brut[i]<0)); //if the lowest part goes down and one stays
                //contrary motions help creation
                BoolVar cpd_cfu = expr(home, (part.m_intervals_brut[idx][i]<0)&&(lowest[0].m_intervals_brut[i]>0)); //if the cf goes up and the cp down
                BoolVar cpu_cfd = expr(home, (part.m_intervals_brut[idx][i]>0)&&(lowest[0].m_intervals_brut[i]<0)); //if the cf goes down and the cp up
                //direct constraints
                rel(home, ((both_up || both_stay || both_down) && (part.is_not_lowest[i]==1)) >> (part.motions[idx][i]==2));
                rel(home, ((both_up || both_stay || both_down) && (part.is_not_lowest[i]==1)) >> (part.motions_cost[idx][i]==part.dir_motion_cost));
                //oblique constraints
                rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (part.is_not_lowest[i]==1)) >> (part.motions[idx][i]==1));
                rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (part.is_not_lowest[i]==1)) >> (part.motions_cost[idx][i]==part.obl_motion_cost));
                //contrary constraints
                rel(home, ((cpd_cfu || cpu_cfd)&& (part.is_not_lowest[i]==1)) >> (part.motions[idx][i]==0));
                rel(home, ((cpd_cfu || cpu_cfd)&& (part.is_not_lowest[i]==1)) >> (part.motions_cost[idx][i]==part.con_motion_cost));
                //bass constraints
                BoolVar is_lowest = expr(home, part.is_not_lowest[i]==0);
                rel(home, part.motions[idx][i], IRT_EQ, -1, Reify(is_lowest));
                rel(home, part.motions_cost[idx][i], IRT_EQ, 0, Reify(is_lowest, RM_IMP));
                //rel(home, (part.is_not_lowest[i]==0) >> (part.motions[idx][i]==-1));
                //rel(home, (part.is_not_lowest[i]==0) >> (part.motions_cost[idx][i]==0));
            }
     }

void link_p_cons_array(const Home &home, Part part){ //nothing to change here, it's fine
    for(int i = 0; i < part.size; i++){
        rel(home, expr(home, part.hIntervalsCpCf[0][i]==0), BOT_OR, expr(home, part.hIntervalsCpCf[0][i]==7), part.is_P_cons[i]);
    }
}

void harmonic_intervals_consonance(const Home &home, Part part, IntSet pen){ //nothing to change here, it's fine
    for(int i = 0; i < part.hIntervalsCpCf[0].size(); i++){
        if(i==part.size-1 && part.speciesList.size()>1){
            dom(home, part.hIntervalsCpCf[0][i], any_h_triad); // G9: h triad constraint (TODO LUC CHECK)
        } else if(i==part.size-2){
            dom(home, part.hIntervalsCpCf[0][i], pen);
        } else {
            dom(home, part.hIntervalsCpCf[0][i], consonances); //constraint : all intervals are consonances 
        }
    }
}

void perfect_consonance_constraints(const Home &home, Part part){ //nothing to change here, it's fine
    dom(home, part.hIntervalsCpCf[0][0], perfect_consonance); //first is a perfect consonance
    dom(home, part.hIntervalsCpCf[0][part.size-1], perfect_consonance); //last is a perfect consonance
}

void imperfect_consonances_are_preferred(const Home &home, int size, Part part, int idx){
        for(int i = 0; i < size; i++){
            //the two constraints set the cost : if it is either a unisson or a perfect fifth, the cost is set. else, it is 0
            rel(home, (part.hIntervalsCpCf[idx][i]==UNISSON) >> (part.octave_costs[idx][i]==part.h_octave));
            rel(home, (part.hIntervalsCpCf[idx][i]==PERFECT_FIFTH) >> (part.fifth_costs[idx][i]==part.h_fifth));
            rel(home, (part.hIntervalsCpCf[idx][i]!=UNISSON) >> (part.octave_costs[idx][i]==0));
            rel(home, (part.hIntervalsCpCf[idx][i]!=PERFECT_FIFTH) >> (part.fifth_costs[idx][i]==0));
        }
}

// TODO : pour moi, cette règle est mal formalisée par Anton. Ca devrait etre uniquement la basse qui est tuned à la premiere note du cf. PAS n'importe quel cp qui est en dessous du cf. 
void key_tone_tuned_to_cantusfirmus(const Home &home, Part part, vector<Stratum> lowest){ //nothing to change here, it's fine
    // for(int p = 1; p < parts.size(); p++){
    //     rel(home, (parts[p].isCFB[0][0] == 0) >> (parts[p].hIntervalsCpCf[0][0]==0)); //tuning the first to the key tune 
    //     rel(home, (parts[p].isCFB[0][size-1] == 0) >> (parts[p].hIntervalsCpCf[0][size-1]==0)); //tuning the last to the key tune 
    // }

    // corrected : 
    // for(int p = 1; p < parts.size(); p++){
    //     rel(home, (parts[p].is_not_lowest[0] == 0) >> ((abs(parts[p].vector_notes[0][0] - parts[0].vector_notes[0][0])%12) == 0));
    //     rel(home, (parts[p].is_not_lowest[size-1] == 0) >> ((abs(parts[p].vector_notes[0][0] - parts[0].vector_notes[0][size-1])%12) == 0)); // normally, the cf should have its last note be the same as its first. so we can link the last note of the bass to be equal to the last or first note of the cf, it will be the same.
    // }

        // corrected : 
    // for(int p = 1; p < parts.size(); p++){
    //     rel(home, (parts[p].is_not_lowest[0] == 0) >> (parts[p].vector_notes[0][0]%12 == parts[0].vector_notes[0][0]%12));
    //     rel(home, (parts[p].is_not_lowest[size-1] == 0) >> (parts[p].vector_notes[0][size-1]%12 == parts[0].vector_notes[0][size-1]%12)); // normally, the cf should have its last note be the same as its first. so we can link the last note of the bass to be equal to the last or first note of the cf, it will be the same.
        
    // }

    // even better formulation : (TODO check that is_not_lowest == 0  <==>  lowest stratum)
    // rel(home, expr(home, lowest[0].notes[0]%12), IRT_EQ, expr(home, parts[0].vector_notes[0][0]%12));
    // rel(home, expr(home, lowest[0].notes[size-1]%12), IRT_EQ, expr(home, parts[0].vector_notes[0][0]%12));

    // cout << "lowest : " << endl;
    // cout << lowest[0].notes[0] << endl; 

    rel(home, lowest[0].notes[0]%12 == part.vector_notes[0][0]%12);
    rel(home, lowest[0].notes[part.size-1]%12 == part.vector_notes[0][0]%12);   
    // cout << lowest[0].notes[0] << endl; 
}

void voices_cannot_play_same_note(const Home &home, vector<Part> parts){
    // TODO  check
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            for(int k = 0; k < 4; k++){
                if(k==0){ //add no unison
                    for(int i = 1; i < parts[p1].size-1; i++){
                        rel(home, parts[p1].vector_notes[k][i], IRT_NQ, parts[p2].vector_notes[k][i]);
                    }
                } else { //add no unison at all
                    for(int i = 0; i < parts[p1].size-1; i++){
                        rel(home, parts[p1].vector_notes[k][i], IRT_NQ, parts[p2].vector_notes[k][i]);
                    }
                }
            }
        }
    }
}

void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, Part part, IntVar NINE, IntVar THREE){
    int p = part.size-2;
    //nothing to change here, it's fine
    if(part.speciesList.size()==2){ //if it is 3 voices
        if(part.is_not_lowest[part.size-2].assigned()){
            if(part.is_not_lowest[part.size-2].val()==1){
                //constraint that penultimate note is either a minor third, perfect fifth, major sixth or octave
                dom(home, part.hIntervalsCpCf[0][part.size-2], MIN3_PERF5_MAJ6_OCT);
            }
        }
    } else { //else 2 voice constraints
        //ite(home, parts[0].is_not_lowest[size-2], NINE, THREE, parts[1].hIntervalsCpCf[0][size-2]);
        if(part.species==0){
            if(part.penult_rule_check==1){
                rel(home, part.hIntervalsCpCf[0][part.size-2], IRT_EQ, THREE, Reify(part.is_not_lowest[part.size-2], RM_IMP));
            }
        }
        if(part.species==1){
            if(part.penult_rule_check==1){
                rel(home, part.hIntervalsCpCf[0][part.size-2], IRT_EQ, NINE, Reify(part.is_not_lowest[part.size-2], RM_IMP));
            }
        }
    }
}

void melodic_intervals_not_exceed_minor_sixth(const Home &home, Part part){  // TODO check 4v
    if (part.speciesList.size() == 2){
        for(int j = 0; j < part.size-1; j++){
            rel(home, part.m_intervals[0][j], IRT_LQ, 8); //constraint : must not exceed a minor sixth
        }
    }else if (part.speciesList.size()==3){
        for(int j = 0; j < part.size-1; j++){
            rel(home, part.m_intervals[0][j] <= 8 || part.m_intervals[0][j] == 12); //constraint : must not exceed a minor sixth, or can be an octave jump
        }
    }
}

void no_direct_perfect_consonance(const Home &home, Part part, int n_species){
    if(n_species==1){ //if 2 voices
        for(int j = 0; j < part.size-1; j++){
            rel(home, part.motions[0][j], IRT_NQ, 2, Reify(expr(home, part.is_not_lowest[j+1]==1 && part.is_P_cons[j+1]==1), RM_IMP));
        }
    } else if(n_species>=2){ //else if 3+ voices
        //if(parts[p].species==1){
            for(int j = 0; j < part.size-2; j++){
                //set a cost when it is reached through direct motion, it is 0 when not
                rel(home, (part.motions[0][j]==2&&(part.hIntervalsCpCf[0][j+1]==0||part.hIntervalsCpCf[0][j+1]==7))>>(part.direct_move_cost[j]==part.direct_move));
                rel(home, (part.motions[0][j]!=2||(part.hIntervalsCpCf[0][j+1]!=0&&part.hIntervalsCpCf[0][j+1]!=7))>>(part.direct_move_cost[j]==0));
            }
        //}
    }
}

void no_battuta(const Home &home, Part part, Part cf){
    for(int j = 0; j < part.size-1; j++){
        //constraints avoiding a battuta kind of motion
        rel(home, expr(home, ((part.hIntervalsCpCf[0][j+1]==0)&&(part.motions[0][j]==0)&&(part.m_intervals_brut[0][j]<-4)&&(part.is_not_lowest[j]==1))), IRT_EQ, 0);
        rel(home, expr(home, ((part.hIntervalsCpCf[0][j+1]==0)&&(part.motions[0][j]==0)&&(cf.m_intervals_brut[0][j]<-4)&&(part.is_not_lowest[j]==0))), IRT_EQ, 0);
    }
}

void no_tenth_in_last_chord(const Home &home, vector<Stratum> upper, int list_index){ //TODO : delete lowest from parameters, not used
        //prohibiting tenths by checking the hInterval
        rel(home, (upper[list_index].hIntervalsBrut[upper[list_index].size-1]>12) >> 
            (upper[list_index].hIntervals[upper[list_index].size-1]!=3 && upper[list_index].hIntervals[upper[list_index].size-1]!=4));
}

void last_chord_no_minor_third(const Home &home, Part part){ //nothing to change here, it's fine
    if(part.is_not_lowest[part.size-1].assigned()){
        if(part.is_not_lowest[part.size-1].val()==1){
            //prohibiting minor thirds in the last chord
            rel(home, part.hIntervalsCpCf[0][part.size-1], IRT_NQ, 3);
        }
    }
}

void variety_cost_constraint(const Home &home, Part part){
    int temp = 0;
    for(int j = 0; j < part.size-1; j++){
        int upbnd = 0;
        if(j+3<part.size){
            upbnd = j+4;
        } else {upbnd = part.size;}
        for(int k = j+1; k < upbnd;k++){
            //setting a cost if notes inside a window are the same in a part
            rel(home, (part.vector_notes[0][j]==part.vector_notes[0][k])>>(part.varietyArray[temp]==part.variety_cost));
            rel(home, (part.vector_notes[0][j]!=part.vector_notes[0][k])>>(part.varietyArray[temp]==0));
            temp++;
        }
    }
}

void avoid_perfect_consonances(const Home &home, int size, vector<Part> parts, IntVarArray succ_cost){ //nothing to change here, it's fine
    int idx = 0;
    // cout << "SUCC SIZE :" + to_string(succ_cost.size()) << endl;
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            // cout << "Part 1 : " << to_string(parts[p1].species) << endl;
            // cout << "Part 2 : " << to_string(parts[p2].species) << endl;
            if(parts[p1].species!= 2 && parts[p2].species!=2){
                for(int i = 0; i < parts[p1].is_P_cons.size()-1; i++){
                    // cout << "IDX : " + to_string(idx) << endl;
                    rel(home, succ_cost[idx], IRT_EQ, parts[p2].succ, Reify(expr(home, parts[p1].is_P_cons[i]==1 && parts[p2].is_P_cons[i]==1)));
                    idx++;
                }        
            }
            else if(parts[p1].species==2){
                for(int i = 0; i < parts[p1].is_P_cons.size()-1; i++){
                    BoolVar case1 = expr(home, (parts[p1].is_P_cons[i]==1 && parts[p2].is_P_cons[i]==1) && 
                        (parts[p1].hIntervalsCpCf[0][i]!=PERFECT_FIFTH || parts[p2].hIntervalsCpCf[0][i]!=PERFECT_FIFTH));
                    BoolVar case2 = expr(home, (parts[p1].m_succ_intervals[0][i]!=MINOR_THIRD && parts[p1].m_succ_intervals[0][i]!=MAJOR_THIRD) && 
                        (parts[p1].hIntervalsCpCf[0][i]==PERFECT_FIFTH && parts[p2].hIntervalsCpCf[0][i]==PERFECT_FIFTH));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(home, succ_cost[idx], IRT_EQ, parts[p2].succ, Reify(expr(home, (case1==1 || case2==1))));
                    idx++;
                }
            }
            else if(parts[p2].species==2){
                // cout << "P cons size : " + to_string(parts[p1].is_P_cons.size()) << endl;
                // cout << "M succ size : " + to_string(parts[p2].m_succ_intervals[0].size()) << endl;
                for(int i = 0; i < parts[p1].is_P_cons.size()-1; i++){
                    BoolVar case1 = expr(home, (parts[p1].is_P_cons[i]==1 && parts[p2].is_P_cons[i]==1) && 
                        (parts[p1].hIntervalsCpCf[0][i]!=PERFECT_FIFTH || parts[p2].hIntervalsCpCf[0][i]!=PERFECT_FIFTH));
                    // cout << "i : " + to_string(i) << endl;
                    BoolVar case2 = expr(home, (parts[p2].m_succ_intervals[0][i]!=MINOR_THIRD && parts[p2].m_succ_intervals[0][i]!=MAJOR_THIRD) && 
                        (parts[p1].hIntervalsCpCf[0][i]==PERFECT_FIFTH && parts[p2].hIntervalsCpCf[0][i]==PERFECT_FIFTH));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(home, succ_cost[idx], IRT_EQ, parts[p2].succ, Reify(expr(home, (case1==1 || case2==1))));
                    idx++;
                }
            }
        }
    }
}

void no_same_direction_3v(const Home &home, int size, vector<Part> parts){
    IntSet t = IntSet(0, 1);
    for(int n = 0; n < size-1; n++){
        rel(home, parts[0].motions[0][n]!=2 || parts[1].motions[0][n]!=2 || parts[2].motions[0][n]!=2);
    }
}

void no_same_direction_4v(const Home &home, int size, vector<Part> parts){
    IntSet t = IntSet(0, 1);
    for(int n = 0; n < size-1; n++){
        rel(home, parts[0].motions[0][n]!=2 || parts[1].motions[0][n]!=2 || parts[2].motions[0][n]!=2 || parts[3].motions[0][n]!=2);  
    }
}

void no_successive_ascending_sixths(const Home &home, int size, vector<Part> parts){ //nothing to change here, it's fine
        for(int v1=0; v1 < parts.size(); v1++){
            for(int v2=0; v2 < parts.size(); v2++){
                if(v1!=v2){
                    for(int j = 1; j < size-2; j++){
                        //constraint checks successive ascending sixths
                        rel(home, ((parts[v1].hIntervalsCpCf[0][j-1]!=8 && parts[v1].hIntervalsCpCf[0][j-1]!=9 && 
                        parts[v2].hIntervalsCpCf[0][j-1]!=8 && parts[v2].hIntervalsCpCf[0][j-1]!=9)||
                        (parts[v1].hIntervalsCpCf[0][j]!=8 && parts[v1].hIntervalsCpCf[0][j]!=9 &&
                        parts[v2].hIntervalsCpCf[0][j]!=8 && parts[v2].hIntervalsCpCf[0][j]!=9))||
                        (parts[v1].m_intervals_brut[0][j]>0)||(parts[v2].m_intervals_brut[0][j]>0));
                    }
                }
            }
        }
}

void prefer_harmonic_triads(const Home &home, vector<Part> parts, IntVarArray triad_costs){
    for(int i = 0; i < parts[0].size; i++){
        BoolVar is_h1_third = expr(home, parts[1].hIntervalsCpCf[0][i]==3 || parts[1].hIntervalsCpCf[0][i]==4);
        rel(home, is_h1_third, BOT_AND, expr(home, parts[2].hIntervalsCpCf[0][i]==7), parts[1].is_h1_poss);
        BoolVar is_h2_third = expr(home, parts[2].hIntervalsCpCf[0][i]==3 || parts[2].hIntervalsCpCf[0][i]==4);
        rel(home, is_h2_third, BOT_AND, expr(home, parts[1].hIntervalsCpCf[0][i]==7), parts[1].is_h2_poss);

        rel(home, expr(home, parts[1].is_h1_poss || parts[1].is_h2_poss), BOT_XOR, parts[1].is_not_triad, 1);
        rel(home, triad_costs[i], IRT_EQ, 0, Reify(expr(home, parts[1].is_h1_poss || parts[1].is_h2_poss), RM_IMP));
        rel(home, triad_costs[i], IRT_EQ, parts[1].h_triad_cost, Reify(parts[1].is_not_triad, RM_IMP));
    }
}


// TODO Modify for last measure
void prefer_harmonic_triads_4v(const Home &home, vector<Stratum> upper, IntVarArray triad_costs){
    for(int i = 0; i < upper[0].size; i++){

        IntVar H_b = upper[0].hIntervalsAbs[i];
        IntVar H_c = upper[1].hIntervalsAbs[i];
        IntVar H_d = upper[2].hIntervalsAbs[i];


        BoolVar H_b_is_third    = expr(home, H_b==3 || H_b==4);
        BoolVar H_b_is_fifth    = expr(home, H_b==7);
        BoolVar H_b_is_octave   = expr(home, H_b==0);

        BoolVar H_c_is_third    = expr(home, H_c==3 || H_c==4);
        BoolVar H_c_is_fifth    = expr(home, H_c==7);
        BoolVar H_c_is_octave   = expr(home, H_c==0);

        BoolVar H_d_is_third    = expr(home, H_d==3 || H_d==4);
        BoolVar H_d_is_fifth    = expr(home, H_d==7);
        BoolVar H_d_is_octave   = expr(home, H_d==0);


        // worst case : not a harmonic triad with at least a third and a fifth
        BoolVar no_fifth_or_no_third = expr(home, H_b_is_third + H_c_is_third + H_d_is_third == 0 || H_b_is_fifth + H_c_is_fifth + H_d_is_fifth == 0);
        BoolVar note_outside_harmonic_triad = expr(home, H_b_is_third + H_b_is_fifth + H_b_is_octave == 0 || H_c_is_third + H_c_is_fifth + H_c_is_octave == 0 || H_d_is_third + H_d_is_fifth + H_d_is_octave == 0);

        rel(home, (note_outside_harmonic_triad || no_fifth_or_no_third) >> (triad_costs[i] == not_harmonic_triad_cost));

        // now we are left with only combinations with at a third, a fifth, and another note of the harmonic triad. 
        // Doubling the fifth
        rel(home, expr(home, H_b_is_fifth + H_c_is_fifth + H_d_is_fifth == 2) >> (triad_costs[i] == double_fifths_cost));

        // Doubling the third, and ensure it is the same type of third (not a major and a minor)
        rel(home, expr(home, H_b_is_third + H_c_is_third + H_d_is_third == 2) >> (triad_costs[i] == double_thirds_cost));
        rel(home, (H_b_is_third && H_c_is_third) >> (H_b == H_c));
        rel(home, (H_b_is_third && H_d_is_third) >> (H_b == H_d));
        rel(home, (H_c_is_third && H_d_is_third) >> (H_c == H_d));

        // triad with octave
        rel(home, (H_b_is_fifth && H_c_is_third && H_d_is_octave) >> (triad_costs[i] == triad_with_octave_cost)); // 5 3 8
        rel(home, (H_b_is_third && H_c_is_octave && H_d_is_fifth) >> (triad_costs[i] == triad_with_octave_cost)); // 3 8 5
        rel(home, (H_b_is_third && H_c_is_fifth && H_d_is_octave) >> (triad_costs[i] == triad_with_octave_cost)); // 3 5 8
        rel(home, (H_b_is_octave && H_c_is_third && H_d_is_fifth) >> (triad_costs[i] == triad_with_octave_cost)); // 8 3 5
        rel(home, (H_b_is_octave && H_c_is_fifth && H_d_is_third) >> (triad_costs[i] == triad_with_octave_cost)); // 8 5 3

        // Best case : 5 8 3
        rel(home, (H_b_is_fifth && H_c_is_octave && H_d_is_third) >> (triad_costs[i] == 0)); 


    
    }
}

void set_off_costs(const Home &home, Part part){
    for(int i = 0; i < part.sol_len; i++){
        rel(home, (part.is_off[i]==1) >> (part.off_costs[i]==part.off_cst));
        rel(home, (part.is_off[i]==0) >> (part.off_costs[i]==0));
    }
}

void set_step_costs(const Home &home, int size, Part part, int idx){
        for(int i = 0; i < size-1; i++){
            rel(home, (part.m_intervals[idx][i]<3) >> (part.m_degrees_cost[idx][i]==part.step_cost));
            rel(home, (part.m_intervals[idx][i]==3 || part.m_intervals[idx][i]==4) >> (part.m_degrees_cost[idx][i]==part.third_cost));
            rel(home, (part.m_intervals[idx][i]==5) >> (part.m_degrees_cost[idx][i]==part.fourth_cost));
            rel(home, (part.m_intervals[idx][i]==6) >> (part.m_degrees_cost[idx][i]==part.tritone_cost));
            rel(home, (part.m_intervals[idx][i]==7) >> (part.m_degrees_cost[idx][i]==part.fifth_cost));
            rel(home, (part.m_intervals[idx][i]==8 || part.m_intervals[idx][i]==9) >> (part.m_degrees_cost[idx][i]==part.sixth_cost));
            rel(home, (part.m_intervals[idx][i]==10 || part.m_intervals[idx][i]==11) >> (part.m_degrees_cost[idx][i]==part.seventh_cost));
            rel(home, (part.m_intervals[idx][i]==12) >> (part.m_degrees_cost[idx][i]==part.octave_cost));
        }
}

void no_chromatic_melodies(const Home &home, Part part){
    for(int i = 0; i < part.size-2; i++){
        rel(home, expr(home, part.m_intervals_brut[0][i]==1), BOT_AND, expr(home, part.m_intervals_brut[0][i+1]==1), 0);
        rel(home, expr(home, part.m_intervals_brut[0][i]==-1), BOT_AND, expr(home, part.m_intervals_brut[0][i+1]==-1), 0);
    }
}

void last_chord_same_fundamental(const Home &home, vector<Stratum> lowest, vector<Part> parts){
    rel(home, expr(home, lowest[0].notes[parts[0].size-1]%12), IRT_EQ, expr(home, parts[0].vector_notes[0][0]%12));
}

/**
 * ========================================================================================================
 *                                      SECOND SPECIES CONSTRAINTS
 * ========================================================================================================
*/

void link_harmonic_arrays_2nd_species(const Home &home, int size, Part part, vector<Stratum> lowest){

    for(int i = 0; i < size; i++){
        if(i!=size-1){ //butlast in lisp code of anton
            rel(home, part.hIntervalsCpCf[2][i] == abs((part.vector_notes[2][i] - lowest[0].notes[i])%12)); //assigns the hIntervals
        }     
        rel(home, part.hIntervalsBrut[i] == lowest[0].notes[i]-part.vector_notes[0][i]);
        abs(home, part.hIntervalsBrut[i], part.hIntervalsAbs[i]); 
    }

}

void link_melodic_arrays_2nd_species_next_meas(const Home &home, int size, Part part){

    //from the first notes, take the ones EXCEPT the first note
    for(int i = 0; i < size-1; i++){
        rel(home, expr(home, part.vector_notes[0][i+1]-part.vector_notes[2][i]), IRT_EQ, part.m_intervals_brut[2][i]);
        abs(home, part.m_intervals_brut[2][i], part.m_intervals[2][i]);
    }

}

void link_melodic_arrays_2nd_species_in_meas(const Home &home, int size, Part part){
    //first notes cp (EXCEPT last note), third notes cp, first m_succ, first m_succ_brut
    for(int i = 0; i < size-1; i++){
        rel(home, expr(home, part.vector_notes[0][i]-part.vector_notes[2][i]), IRT_EQ, part.m_succ_intervals_brut[0][i]);
        abs(home, part.m_succ_intervals_brut[0][i], part.m_succ_intervals[0][i]);
    }
}

void link_m2_arrays_2nd_species(const Home &home, Part part){
    //from the SOLUTION ARRAY : one notes is from 0 to size-2, the other from 2 to size
    for(int i = 0; i < part.m2_len; i++){
        rel(home, expr(home, part.solution_array[i]-part.solution_array[i+2]), IRT_EQ, part.m2_intervals_brut[i]);
        abs(home, part.m2_intervals_brut[i], part.m2_intervals[i]);
    }
}

void link_melodic_self_arrays_2nd_species(const Home &home, Part part){
    //first note of solution array (EXCEPT LAST), all notes of solution array (EXCEPT FIRST)
    for(int i = 0; i < part.sol_len-1; i++){
        rel(home, expr(home, part.solution_array[i]-part.solution_array[i+1]), IRT_EQ, part.m_all_intervals_brut[i]);
        abs(home, part.m_all_intervals_brut[i], part.m_all_intervals[i]);
    }
}

void link_motions_arrays_2nd_species(const Home &home, Part part, vector<Stratum> lowest){
    link_motions_arrays(home, part, lowest, 2);
}

void link_real_motions_arrays_2nd_species(const Home &home, Part part){
    //m_succ_intervals, first motions, third motions, real motions, first motions cost, third motions cost, real motions cost
    for(int i = 0; i < part.size-1; i++){
        rel(home, (expr(home, part.m_succ_intervals[0][i]>4)==1)>>(part.real_motions[i]==part.motions[2][i]));
        rel(home, (expr(home, part.m_succ_intervals[0][i]>4)==0)>>(part.real_motions[i]==part.motions[0][i]));

        rel(home, (expr(home, part.m_succ_intervals[0][i]>4)==1)>>(part.real_motions_cost[i]==part.motions_cost[2][i]));
        rel(home, (expr(home, part.m_succ_intervals[0][i]>4)==0)>>(part.real_motions_cost[i]==part.motions_cost[0][i]));
    }
}

void link_ta_dim_array_2nd_species(const Home &home, Part part){
    //first m_succ_intervals, first m_intervals, third m_intervals, is_ta_dim
    for(int i = 0; i < part.size-1; i++){
        rel(home, part.is_ta_dim[i], IRT_EQ, expr(home, ((part.m_intervals[0][i]==3)||(part.m_intervals[0][i]==4))&&(part.m_succ_intervals[0][i]<=2)
            &&(part.m_intervals[2][i]<=2)));
    }
}

void link_cfb_array_2nd_species(const Home &home, int size, Part part, Part cf){
    link_cfb_arrays_1st_species(home, size, part, cf, 2);
}

void link_is_neighbour_array_2nd_species(const Home &home, Part part, vector<Stratum> lowest){
    //h_intervals_abs (butlast), is_not_lowest (butlast), first m_intervals_brut of lowest, neighbour array
    for(int i = 0; i < part.size-1; i++){
        rel(home, part.is_neighbour[i], IRT_EQ, expr(home, (part.hIntervalsAbs[i]<=4) && (part.is_not_lowest[i]==(lowest[0].m_intervals_brut[i]>=0))));
    }
}

void h_cons_arsis(const Home &home, Part part, IntSet pen){
    for(int i = 0; i < part.size-1; i++){
        if(i==part.size-2){
            if(part.penult_rule_check==1){
                dom(home, part.hIntervalsCpCf[2][i], pen);
            }
        } else {
            for(int d = 0; d < DIS_VEC.size(); d++){
                rel(home, part.hIntervalsCpCf[2][i], IRT_EQ, DIS_VEC[d], Reify(part.is_ta_dim[i], RM_PMI));
            }
        }
    }
}

void penult_cons(const Home &home, Part part, IntSet pen3, IntVar NINE, IntVar THREE, int idx){
    if(part.speciesList.size()==1){ //if it is 2 voices
        if(part.penult_rule_check==1){
            ite(home, part.isCFB[idx][part.isCFB.size()-1], NINE, THREE, part.hIntervalsCpCf[idx][part.hIntervalsCpCf[idx].size()-1]);
        }
    } else {
        dom(home, part.hIntervalsCpCf[idx][part.hIntervalsCpCf[idx].size()-1], pen3);
    }
}

void melodic_inter_arsis(const Home &home, Part part){ //from here on solutions drastically decrease wesh
    //first m_succ, is_neighbour
    for(int i = 0; i < part.is_neighbour.size(); i++){
        rel(home, part.m_succ_intervals[0][i], IRT_EQ, 12, Reify(part.is_neighbour[i], RM_PMI));
        rel(home, expr(home, part.m_succ_intervals[0][i]==10 || part.m_succ_intervals[0][i]==11), BOT_OR, expr(home, part.m_succ_intervals[0][i]==9), 0);
    }
}

void no_chromatic_motion(const Home &home, Part part){
    //m_all_intervals_brut EXCEPT first, m2_intervals_brut
    for(int i = 0; i < part.m2_intervals_brut.size(); i++){
        rel(home, expr(home, part.m_all_intervals_brut[i+1]==1), BOT_AND, expr(home, part.m2_intervals_brut[i]==2), 0);
        rel(home, expr(home, part.m_all_intervals_brut[i+1]==-1), BOT_AND, expr(home, part.m2_intervals_brut[i]==-2), 0);
    }
}

void no_unison_at_all(const Home &home, Part part, int variant){
    //solutionarray, rest of solution array
    if(variant==2){
        for(int i = 0; i < part.sol_len-1; i++){
            rel(home, part.solution_array[i], IRT_NQ, part.solution_array[i+1]);
        }
    }
    else if(variant==7){
        //butlast solutionarray 3, rest (butlast solutionarray 3)
        bool check = false;
        for(int s = 0; s < part.speciesList.size(); s++){
            if(part.speciesList[s]==3){
                check = true;
            }
        }
        if(!check){
            for(int i = 0; i < part.sol_len-4; i++){
                rel(home, part.solution_array[i], IRT_NQ, part.solution_array[i+1]);
            }
        }
        //last solutionarray 3, rest (last solutionarray 3)
        //why does anton do that for 3 voices? fux specifically says there CAN be unison
    }
}

void no_direct_move_perfect_consonance_2nd_species(const Home &home, Part part){
    //real motions, is_p_cons_array, is_not_lowest
    if(part.speciesList.size()==1){
        for(int i = 0; i < part.size-1; i++){
            rel(home, part.real_motions[i], IRT_NQ, 2, Reify(expr(home, part.is_not_lowest[i+1] && part.is_P_cons[i+1]), RM_IMP));
        }
    } else { //else if 3 voices
        
        for(int j = 0; j < part.size-2; j++){
            //set a cost when it is reached through direct motion, it is 0 when not
            rel(home, (part.real_motions[j]==2&&(part.hIntervalsCpCf[0][j+1]==0||part.hIntervalsCpCf[0][j+1]==7))>>(part.direct_move_cost[j]==part.direct_move));
            rel(home, (part.real_motions[j]!=2||(part.hIntervalsCpCf[0][j+1]!=0&&part.hIntervalsCpCf[0][j+1]!=7))>>(part.direct_move_cost[j]==0));
        }
    }
}

void no_battuta_2nd_species(const Home &home, Part part){ //rechekc this constraint later on, i think it is wrong
    for(int j = 0; j < part.size-1; j++){
        //constraints avoiding a battuta kind of motion
        rel(home, expr(home, ((part.hIntervalsCpCf[0][j+1]==0)&&(part.motions[2][j]==0)&&(part.m_intervals_brut[2][j]<-4)&&(part.is_not_lowest[j]==1))), IRT_EQ, 0);
        //here add cantusFirmus melodic interval
        rel(home, expr(home, ((part.hIntervalsCpCf[0][j+1]==0)&&(part.motions[2][j]==0)&&(part.m_intervals_brut[2][j]<-4)&&(part.is_not_lowest[j]==0))), IRT_EQ, 0);
    }
}

void set_penult_sixth_cost(const Home &home, Part part){
    //penult first h-Interval, IRT_NQ, 7, penult_thesis_cost, penult_sixth
    rel(home, (part.hIntervalsCpCf[0][part.size-2]!=7) >> (part.penult_sixth==part.penult_sixth_cost));
    rel(home, (part.hIntervalsCpCf[0][part.size-2]==7) >> (part.penult_sixth==0));
}

/**
 * ========================================================================================================
 *                                      THIRD SPECIES CONSTRAINTS
 * ========================================================================================================
*/

void link_harmonic_arrays_3rd_species(const Home &home, int size, vector<Part> parts, vector<Stratum> lowest){
    //notes cp, butlast first notes lowest, hIntervalsCpCf
    for(int j = 0; j < parts.size(); j++){
        for(int h = 1; h <= 3; h++){
            for(int i = 0; i < size; i++){
                if(i!=size-1){ //butlast in lisp code of anton
                    rel(home, parts[j].hIntervalsCpCf[h][i] == abs((parts[j].vector_notes[h][i] - lowest[0].notes[i])%12)); //assigns the hIntervals
                }     
            }
        }
    }
}

void link_harmonic_to_cf_3rd_species(const Home &home, int size, Part part, Part cf){
    //notes cp, butlast cf, hIntervalsToCf
    for(int h = 1; h <= 3; h++){
        for(int i = 0; i < size; i++){
            if(i!=size-1){ //butlast in lisp code of anton
                rel(home, part.hIntervalsToCf[h][i] == abs((part.vector_notes[h][i] - cf.vector_notes[0][i])%12)); //assigns the hIntervals
            }     
        }
    }
}

void link_melodic_arrays_3rd_species_in_meas(const Home &home, int size, vector<Part> parts){
    //vector_notes i-1 cp, vector_notes i cp, i-1 m_succ, i-1 m_succ_brut
    rel(home, parts[1].m_succ_intervals_brut[0][3], IRT_EQ, 60);
    abs(home, parts[1].m_succ_intervals_brut[0][3], parts[1].m_succ_intervals[0][3]); 
    for(int p = 1; p < parts.size(); p++){
        for(int h = 1; h <= 3; h++){
            for(int i = 0; i < size-1; i++){
                
            }
        }
    }
}

void link_melodic_arrays_3rd_species_next_meas(const Home &home, int size, Part part){
    //fourth notes, first notes (except first), fourth m-intervals, fourth m-intervals-brut
    for(int i = 0; i < size-1; i++){
        rel(home, part.m_intervals_brut[3][i] == part.vector_notes[3][i]-part.vector_notes[0][i+1]);
        abs(home, part.m_intervals_brut[3][i], part.m_intervals[3][i]); 
    }
}

void link_m2_arrays_3rd_species(const Home &home, Part part){
    link_m2_arrays_2nd_species(home, part);
}

void link_melodic_self_arrays_3rd_species(const Home &home, Part part){
    link_melodic_self_arrays_2nd_species(home, part);
}

void link_motions_arrays_3rd_species(const Home &home, Part part, vector<Stratum> lowest){
    link_motions_arrays(home, part, lowest, 3);
}

void link_cfb_array_3rd_species(const Home &home, int size, Part part, Part cf){
    link_cfb_arrays_1st_species(home, size-1, part, cf, 3);
}

void link_is_qn_linked_3rd_species(const Home &home, Part part){
    //m_all_intervals, m_all_intervals_brut, is_qn_linked
    //m1=i ; m2 = i+1; m3 = i+2; m4 = i+3
    //mb1 = i; mb2 = i+1; mb3 = i+2; mb4 = i+3
    //b = is_qn_linked
    for(int i = 0; i <= part.m_all_intervals.size()-3; i++){
        if(i%4==0){
            rel(home, expr(home, (part.m_all_intervals[i]<=2 && part.m_all_intervals[i+1]<=2 && part.m_all_intervals[i+2]<=2 && part.m_all_intervals[i+3]<=2) && 
           (part.m_all_intervals_brut[i]>0 == part.m_all_intervals_brut[i+1]>0 == part.m_all_intervals_brut[i+2]>0 == part.m_all_intervals_brut[i+3]>0)), 
           IRT_EQ, 1, Reify(part.is_qn_linked[i/4]));
        }
    }
}

void link_ta_dim_array_3rd_species(const Home &home, Part part){
    //second m_succ, collect-by-4(m2-intervals) (every 4th element, like i=0 -> i, i+4, i+8, 0 is the offset), third m-succ, is_ta_dim
    for(int i = 0; i < part.size-1; i++){
        rel(home, part.is_ta_dim[i], IRT_EQ, expr(home, ((part.m2_intervals[i*4]==3)||(part.m2_intervals[i*4]==4))&&(part.m_succ_intervals[1][i]<=2)
            &&(part.m_succ_intervals[2][i]<=2)));
    }
}

void link_is_cons_array_3rd_species(const Home &home, Part part){
    part.create_is_cons();
}

void link_ciambatta_3rd_species(const Home &home, Part part){
    //second is_consonant, third is_consonant, second m_succ, is_not_ciambatta
    for(int i = 0; i < part.size-1; i++){
        rel(home, expr(home, part.is_consonant[1][i]==1 && part.is_consonant[2][i]), BOT_AND, 
            expr(home, part.m_succ_intervals[1][i]<=2), part.is_not_ciambatta[i]);
    }
}

void five_consecutive_notes(const Home &home, Part part){
    //third is_consonant, is_qn_array
    for(int i = 0; i < part.is_qn_linked.size(); i++){
        rel(home, part.is_qn_linked[i], BOT_IMP, part.is_consonant[2][i], 1);
    }
}

void any_disonant_note(const Home &home, Part part){
    //second is_consonant, third is_consonant, fourth is_consonant, is_ta_dim
    for(int i = 0; i < part.is_ta_dim.size(); i++){
        rel(home, expr(home, part.is_consonant[2][i]==1 || (part.is_consonant[1][i] && part.is_consonant[3][i])), BOT_AND, 
            expr(home, part.is_consonant[2][i] || part.is_ta_dim[i]), 1);
    }
}

void set_cambiata(const Home &home, Part part, IntVar ZERO){
    //is-nit-cambiatta, cambiatta_cost
    for(int j = 0; j < part.size-1; j++){
        //ite(home, part.is_not_ciambatta[j], part.cambiatta_cost, ZERO, part.not_cambiatta_cost[j]);
        //rel(home, (part.is_not_ciambatta[j]==1) >> (part.not_cambiatta_cost[j]==part.cambiatta_cost));
        //rel(home, (part.is_not_ciambatta[j]==0) >> (part.not_cambiatta_cost[j]==0));
        rel(home, part.not_cambiatta_cost[j], IRT_EQ, 0);
    }
}

void set_m2_cost(const Home &home, Part part){
    for(int j = 0; j < part.m2_len; j++){
        rel(home, (part.m2_intervals[j]==0) >> (part.m2_eq_zero_costs[j]==part.m2_zero_cost));
        rel(home, (part.m2_intervals[j]!=0) >> (part.m2_eq_zero_costs[j]==0));
    }
}

void no_melodic_interval_between(const Home &home, Part part){
    for(int h = 0; h < part.m_succ_intervals.size(); h++){
        for(int i = 0; i < part.m_succ_intervals[h].size(); i++){
            rel(home, part.m_succ_intervals[h][i], IRT_NQ, 9);
            rel(home, part.m_succ_intervals[h][i], IRT_NQ, 10);
            rel(home, part.m_succ_intervals[h][i], IRT_NQ, 11);
        }
    }
    for(int i = 0; i < part.m_intervals[3].size(); i++){
        rel(home, part.m_intervals[3][i], IRT_NQ, 9);
        rel(home, part.m_intervals[3][i], IRT_NQ, 10);
        rel(home, part.m_intervals[3][i], IRT_NQ, 11);
    }
}

void marcels_rule(const Home &home, Part part){
    //m_all_intervals, m_all_intervals_brut
    for(int i = 0; i < part.m_all_intervals.size()-1; i++){
        BoolVar b_contrary = expr(home, part.m_all_intervals_brut[i] > 0 == part.m_all_intervals_brut[i+1] < 0);
        BoolVar b_skip = expr(home, part.m_all_intervals[i] > 2);
        rel(home, part.m_all_intervals[i+1], IRT_LQ, 2, Reify(b_skip, RM_IMP));
        rel(home, b_skip, BOT_IMP, b_contrary, 1);
    }
}

void no_direct_move_perfect_consonance_3rd_species(const Home &home, Part part){
    //fourth motions, is_p_cons_array, is_not_lowest
    if(part.speciesList.size()==1){
        for(int i = 0; i < part.size-1; i++){
            rel(home, part.motions[3][i], IRT_NQ, 2, Reify(expr(home, part.is_not_lowest[i+1] && part.is_P_cons[i+1]), RM_IMP));
        }
    }
}

void no_battuta_3rd_species(const Home &home, Part part){
    //fourth motions, first hIntervalsToCf, fourth m_intervals_brut, fourth is_cf_lower
    for(int j = 0; j < part.size-1; j++){
        //constraints avoiding a battuta kind of motion
        rel(home, expr(home, ((part.hIntervalsToCf[0][j+1]==0)&&(part.motions[3][j]==0)&&(part.m_intervals_brut[3][j]<-4)&&(part.is_not_lowest[j]==1))), IRT_EQ, 0);
        //here add cantusFirmus melodic interval
        rel(home, expr(home, ((part.hIntervalsToCf[0][j+1]==0)&&(part.motions[3][j]==0)&&(part.m_intervals_brut[3][j]<-4)&&(part.is_not_lowest[j]==0))), IRT_EQ, 0);
    }
}


















/**
 * ========================================================================================================
 *                                      TEST FUNCTIONS
 * ========================================================================================================
*/

// fig 160, page 110
void test_4v_fux(const Home &home, vector<Part> parts, IntVarArray solutionArray){

    cout << "test 4v fux function" << endl;

    // cp3  fa la do si si re do mi re do# re 
    // cp3  65 69 72 71 71 74 72 76 74 73 74

    // cf : re fa mi re sol fa la sol fa mi re
    // cf : 62 65 64 62 67 65 69 67 65 64 62

    // cp2  la fa sol sol sol la la do la la la
    // cp2  57 53 55 55 55 57 57 60 57 57 57

    // cp1  50 50 48 55 52 50 53 48 50 45 50

    vector<int> cp1 = {50, 50, 48, 55, 52, 50, 53, 48, 50, 45, 50};
    vector<int> cp2 = {57, 53, 55, 55, 55, 57, 57, 60, 57, 57, 57};
    vector<int> cp3 = {65, 69, 72, 71, 71, 74, 72, 76, 74, 73, 74};    

    // for (int i = 0; i < 11; i++)
    // {
    //     rel(home, parts[1].vector_notes[0][i] == cp1[i]);
    //     rel(home, parts[2].vector_notes[0][i] == cp2[i]);
    //     rel(home, parts[3].vector_notes[0][i] == cp3[i]);
    // }


    vector<int> sol_arr = {50, 50, 48, 55, 52, 50, 53, 48, 50, 45, 50, 57, 53, 55, 55, 55, 57, 57, 60, 57, 57, 57, 65, 69, 72, 71, 71, 74, 72, 76, 74, 73, 74};
    for (int i = 0; i < sol_arr.size(); i++)
    {
        rel(home, solutionArray[i] == sol_arr[i]);
    }
}



void test_4v_2sp_fux(const Home &home, vector<Part> parts){

    cout << "test 4v 2sp fux function enabled " << endl;

    // cf : re fa mi re so fa la so fa mi re
    //      62 65 64 62 67 65 69 67 65 64 62

    // cp1  la re so so si re do mi re do# re
    //    : 57 62 55 55 59 62 60 64 62 61 62

    // cp2  -- fa | la si | do sol | si la | sol mi | la fa | mi fa | mi do | la fa |  la  |   la  
    //      -- 53   57 59   60 55    59 57   55  52   57 53   52 53   52 60   57 53    57      57 

    // cp3  re re do so mi re la do re la re
    //      50 50 48 55 52 50 45 48 50 45 50 

    vector<int> cp1 = {};
    vector<int> cp2 = {};
    vector<int> cp3 = {};


    for (int i = 0; i < 11; i++)
    {
        // cout << parts[1].vector_notes[0][i] << endl;
        rel(home, parts[1].vector_notes[0][i] == cp1[i]);
        rel(home, parts[2].vector_notes[0][i] == cp2[i]);
        rel(home, parts[3].vector_notes[0][i] == cp3[i]);
    }
}