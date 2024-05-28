#include "headers/all_constraints.hpp"
#include "headers/Utilities.hpp"

/**
  * Link the harmonic intervals arrays for the first species
  * @param home the problem
  * @param size the number of notes in the cf
  * @param cp the variable array for the counterpoint to be generated
  * @param hIntervalsCpCf the variable array for the harmonic intervals between the cp and the cf
  * @param cantusFirmus the cantus firmus
  */
void link_harmonic_arrays_1st_species(const Home &home, int size, vector<Part> parts,vector<Stratum> lowest, 
    vector<Stratum> upper){
    for(int j = 0; j < parts.size(); j++){
        for(int i = 0; i < size; i++){
            rel(home, parts[j].hIntervalsCpCf[0][i] == abs(parts[j].vector_notes[0][i] - lowest[0].notes[i])%12); //assigns the hIntervals
        }
    }

    if(parts.size()>2){
        for(int p = 0; p < parts.size()-1; p++){
            for(int i = 0; i < size; i++){
                //rel(home, expr(home, (lowest[0].notes[i]-lowest[0].notes[i])%12), IRT_EQ, lowest[0].hIntervalsBrut[i]);
                //abs(home, lowest[0].hIntervalsBrut[i], lowest[0].hIntervalsAbs[i]);
                rel(home, expr(home, (lowest[0].notes[i]-upper[p].notes[i])%12), IRT_EQ, upper[p].hIntervalsBrut[i]); //assigns the hIntervals between the lowest stratum
                abs(home, upper[p].hIntervalsBrut[i], upper[p].hIntervalsAbs[i]); //also creates the absolute hInterval
            }
        }
    }
}

void link_cfb_arrays_1st_species(const Home &home, int size, Part part, Part cf, int idx){
    for(int i = 0; i < size; i++){
        rel(home, part.vector_notes[idx][i], IRT_GQ, cf.notes[i], Reify(part.isCFB[idx][i])); //links the CFB values in the array
    }
}

void link_melodic_arrays_1st_species(const Home &home, int size, vector<Part> parts){
    //works
    for(int k = 0; k < parts.size(); k++){
        for(int i = 0; i < size-1; i++){
            if(k==0){
                rel(home, expr(home, parts[k].notes[i]-parts[k].notes[i+1]), IRT_EQ, parts[k].m_intervals_brut[0][i]); //assigns the melodic interval (brut)
                abs(home, parts[k].m_intervals_brut[0][i], parts[k].m_intervals[0][i]); //assigns the melodic interval (absolute)
            } else {
                rel(home, expr(home, parts[k].vector_notes[0][i]-parts[k].vector_notes[0][i+1]), IRT_EQ, parts[k].m_intervals_brut[0][i]); //assigns the melodic interval (brut)
                abs(home, parts[k].m_intervals_brut[0][i], parts[k].m_intervals[0][i]); //assigns the melodic interval (absolute)
            }
        }
    }
}

void link_motions_arrays(const Home &home, Part part, Part cf, vector<Stratum> lowest, int idx){ //TODO : delete lowest from parameters, not used
            for(int i = 0; i < cf.size-1; i++){
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

void harmonic_intervals_consonance(const Home &home, vector<Part> parts, IntSet pen){ //nothing to change here, it's fine
    for(int p = 0; p < parts.size(); p++){
        for(int i = 0; i < parts[p].hIntervalsCpCf[0].size(); i++){
            if(i==parts[p].size-1 && parts[p].speciesList.size()>1){
                dom(home, parts[p].hIntervalsCpCf[0][i], major_h_triad); //major h triad constraint
            } else if(i==parts[p].size-2){
                dom(home, parts[p].hIntervalsCpCf[0][i], pen);
            } else {
                dom(home, parts[p].hIntervalsCpCf[0][i], consonances); //constraint : all intervals are consonances 
            }
        }
    }
}

void perfect_consonance_constraints(const Home &home, int size, vector<Part> parts){ //nothing to change here, it's fine
        for(int p = 0; p < parts.size(); p++){
            dom(home, parts[p].hIntervalsCpCf[0][0], perfect_consonance); //first is a perfect consonance
            dom(home, parts[p].hIntervalsCpCf[0][size-1], perfect_consonance); //last is a perfect consonance
        }
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

void key_tone_tuned_to_cantusfirmus(const Home &home, int size, vector<Part> parts){ //nothing to change here, it's fine
    for(int p = 1; p < parts.size(); p++){
        rel(home, (parts[p].isCFB[0][0] == 1) >> (parts[p].hIntervalsCpCf[0][0]==0)); //tuning the first to the key tune (if the cf is the bass)
        rel(home, (parts[p].isCFB[0][size-1] == 1) >> (parts[p].hIntervalsCpCf[0][size-1]==0)); //tuning the last to the key tune (if the cf is the bass)
    }
}

void voices_cannot_play_same_note(const Home &home, int size, vector<Part> parts){
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            for(int k = 0; k < 4; k++){
                if(k==0){ //add no unison
                    for(int i = 1; i < size-1; i++){
                        rel(home, parts[p1].vector_notes[k][i], IRT_NQ, parts[p2].vector_notes[k][i]);
                    }
                } else { //add no unison at all
                    for(int i = 0; i < size-1; i++){
                        rel(home, parts[p1].vector_notes[k][i], IRT_NQ, parts[p2].vector_notes[k][i]);
                    }
                }
            }
        }
    }
}

void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, int size, vector<Part> parts, IntVar NINE, IntVar THREE, vector<Stratum> upper){
    int p = size-2;
    //nothing to change here, it's fine
    if(parts.size()==3){ //if it is 3 voices
        for(int p = 0; p < parts.size(); p++){
            if(parts[p].is_not_lowest[size-2].assigned()){
                if(parts[p].is_not_lowest[size-2].val()==1){
                    //constraint that penultimate note is either a minor third, perfect fifth, major sixth or octave
                    dom(home, parts[p].hIntervalsCpCf[0][size-2], MIN3_PERF5_MAJ6_OCT);
                }
            }
        }
    } else { //else 2 voice constraints
        //ite(home, parts[0].is_not_lowest[size-2], NINE, THREE, parts[1].hIntervalsCpCf[0][size-2]);
        for(int p = 0; p < parts.size(); p++){
            if(parts[p].species==0){
                if(parts[p].penult_rule_check==1){
                    rel(home, parts[p].hIntervalsCpCf[0][size-2], IRT_EQ, THREE, Reify(parts[p].is_not_lowest[size-2], RM_IMP));
                }
            }
            if(parts[p].species==1){
                if(parts[p].penult_rule_check==1){
                    rel(home, parts[p].hIntervalsCpCf[0][size-2], IRT_EQ, NINE, Reify(parts[p].is_not_lowest[size-2], RM_IMP));
                }
            }
        }
    }
}

void no_tritonic_intervals(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int j = 0; j < size-1; j++){
            rel(home, (parts[p].m_intervals[0][j]==6) >> (parts[p].M_deg_cost[j]==parts[p].tritone_cost)); //if it is a tritone : set a cost
            rel(home, (parts[p].m_intervals[0][j]!=6) >> (parts[p].M_deg_cost[j]==0)); //if not, the cost is 0
        }
    }
}

void melodic_intervals_not_exceed_minor_sixth(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int j = 0; j < size-1; j++){
            rel(home, parts[p].m_intervals[0][j], IRT_LQ, 8); //constraint : must not exceed a minor sixth
        }
    }
}

void no_direct_perfect_consonance(const Home &home, int size, vector<Part> parts, int n_species, vector<Stratum> upper){
    if(n_species==1){ //if 2 voices
        for(int j = 0; j < size-1; j++){
            for(int p = 0; p < parts.size(); p++){
                rel(home, parts[p].motions[0][j], IRT_NQ, 2, Reify(expr(home, parts[p].is_not_lowest[j+1]==1 && parts[p].is_P_cons[j+1]==1), RM_IMP));
            }
        }
    } else { //else if 3 voices
        for(int p = 1; p < parts.size(); p++){ //should be fine i guess
            if(parts[p].species==1){
                for(int j = 0; j < size-2; j++){
                    //set a cost when it is reached through direct motion, it is 0 when not
                    rel(home, (parts[p].motions[0][j]==2&&(parts[p].hIntervalsCpCf[0][j+1]==0||parts[p].hIntervalsCpCf[0][j+1]==7))>>(parts[p].direct_move_cost[j]==parts[p].direct_move));
                    rel(home, (parts[p].motions[0][j]!=2||(parts[p].hIntervalsCpCf[0][j+1]!=0&&parts[p].hIntervalsCpCf[0][j+1]!=7))>>(parts[p].direct_move_cost[j]==0));
                }
            }
        }
    }
}

void no_battuta(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
            for(int j = 0; j < size-1; j++){
            //constraints avoiding a battuta kind of motion
            rel(home, expr(home, ((parts[p].hIntervalsCpCf[0][j+1]==0)&&(parts[p].motions[0][j]==0)&&(parts[p].m_intervals_brut[0][j]<-4)&&(parts[p].is_not_lowest[j]==1))), IRT_EQ, 0);
            rel(home, expr(home, ((parts[p].hIntervalsCpCf[0][j+1]==0)&&(parts[p].motions[0][j]==0)&&(parts[0].m_intervals_brut[0][j]<-4)&&(parts[p].is_not_lowest[j]==0))), IRT_EQ, 0);
        }
    }
}

void no_tenth_in_last_chord(const Home &home, int size, vector<Part> parts, vector<Stratum> upper, vector<Stratum> lowest){ //TODO : delete lowest from parameters, not used

    for(int p = 0; p < parts.size()-1; p++){
        //prohibiting tenths by checking the hInterval
        rel(home, (upper[p].hIntervalsBrut[size-1]>12) >> (upper[p].hIntervals[size-1]!=3 && upper[p].hIntervals[size-1]!=4));
    }

}

void last_chord_no_minor_third(const Home &home, int size, vector<Part> parts){ //nothing to change here, it's fine
    for(int p = 1; p < parts.size(); p++){
        if(parts[p].is_not_lowest[size-1].assigned()){
            if(parts[p].is_not_lowest[size-1].val()==1){
                //prohibiting minor thirds in the last chord
                rel(home, parts[p].hIntervalsCpCf[0][size-1], IRT_NQ, 3);
            }
        }
    }
}

void variety_cost_constraint(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){ //cantusFirmus not taken into account
        int temp = 0;
        for(int j = 0; j < size-1; j++){
            int upbnd = 0;
            if(j+3<size){
                upbnd = j+4;
            } else {upbnd = size;}
            for(int k = j+1; k < upbnd;k++){
                //setting a cost if notes inside a window are the same in a part
                rel(home, (parts[p].vector_notes[0][j]==parts[p].vector_notes[0][k])>>(parts[p].varietyArray[temp]==parts[p].variety_cost));
                rel(home, (parts[p].vector_notes[0][j]!=parts[p].vector_notes[0][k])>>(parts[p].varietyArray[temp]==0));
                temp++;
            }
        }
    }
}

void avoid_perfect_consonances(const Home &home, int size, vector<Part> parts, IntVarArray succ_cost){ //nothing to change here, it's fine
    int idx = 0;
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            cout << "Part 1 : " << to_string(parts[p1].species) << endl;
            cout << "Part 2 : " << to_string(parts[p2].species) << endl;
            if(parts[p1].species!= 2 && parts[p2].species!=2){
                for(int i = 0; i < parts[p1].is_P_cons.size()-1; i++){
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
                cout << "P cons size : " + to_string(parts[p1].is_P_cons.size()) << endl;
                cout << "M succ size : " + to_string(parts[p2].m_succ_intervals[0].size()) << endl;
                for(int i = 0; i < parts[p1].is_P_cons.size()-1; i++){
                    BoolVar case1 = expr(home, (parts[p1].is_P_cons[i]==1 && parts[p2].is_P_cons[i]==1) && 
                        (parts[p1].hIntervalsCpCf[0][i]!=PERFECT_FIFTH || parts[p2].hIntervalsCpCf[0][i]!=PERFECT_FIFTH));
                    cout << "i : " + to_string(i) << endl;
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

void no_same_direction(const Home &home, int size, vector<Part> parts){
    IntSet t = IntSet(0, 1);
    for(int n = 0; n < size-1; n++){
        rel(home, (parts[0].motions[0][n]==0 || parts[0].motions[0][n]==1) || (parts[1].motions[0][n]==0 || parts[1].motions[0][n]==1) || 
            (parts[2].motions[0][n]==0 || parts[2].motions[0][n]==1));
    }
    //for(int p1 = 0; p1 < parts.size(); p1++){
    //    for(int p2 = p1+1; p2 < parts.size(); p2++){
    //        if(p1!=p2){
    //            
    //        }
    //    }
    //}
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

void prefer_harmonic_triads(const Home &home, int size, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVarArray triad_costs){
    for(int i = 0; i < size; i++){
        BoolVar is_h1_third = expr(home, parts[1].hIntervalsCpCf[0][i]==3 || parts[1].hIntervalsCpCf[0][i]==4);
        rel(home, is_h1_third, BOT_AND, expr(home, parts[2].hIntervalsCpCf[0][i]==7), parts[1].is_h1_poss);
        BoolVar is_h2_third = expr(home, parts[2].hIntervalsCpCf[0][i]==3 || parts[2].hIntervalsCpCf[0][i]==4);
        rel(home, is_h2_third, BOT_AND, expr(home, parts[1].hIntervalsCpCf[0][i]==7), parts[1].is_h2_poss);

        rel(home, expr(home, parts[1].is_h1_poss || parts[1].is_h2_poss), BOT_XOR, parts[1].is_not_triad, 1);
        rel(home, triad_costs[i], IRT_EQ, 0, Reify(expr(home, parts[1].is_h1_poss || parts[1].is_h2_poss), RM_IMP));
        rel(home, triad_costs[i], IRT_EQ, parts[1].h_triad_cost, Reify(parts[1].is_not_triad, RM_IMP));
    }
}

void set_off_costs(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < parts[p].sol_len; i++){
            rel(home, (parts[p].is_off[i]==1) >> (parts[p].off_costs[i]==parts[p].off_cst));
            rel(home, (parts[p].is_off[i]==0) >> (parts[p].off_costs[i]==0));
        }
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

void no_chromatic_melodies(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-2; i++){
            rel(home, expr(home, parts[p].m_intervals_brut[0][i]==1), BOT_AND, expr(home, parts[p].m_intervals_brut[0][i+1]==1), 0);
            rel(home, expr(home, parts[p].m_intervals_brut[0][i]==-1), BOT_AND, expr(home, parts[p].m_intervals_brut[0][i+1]==-1), 0);
        }
    }
}

void last_chord_same_fundamental(const Home &home, vector<Stratum> lowest, vector<Part> parts){
    rel(home, expr(home, lowest[0].notes[parts[0].size-1]%12), IRT_EQ, expr(home, parts[0].notes[0]%12));
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

void link_motions_arrays_2nd_species(const Home &home, Part part, Part cf, vector<Stratum> lowest){
    link_motions_arrays(home, part, cf, lowest, 2);
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

void penult_cons(const Home &home, Part part, IntSet pen3, IntVar NINE, IntVar THREE){
    if(part.speciesList.size()==1){ //if it is 2 voices
        if(part.penult_rule_check==1){
            ite(home, part.isCFB[2][part.isCFB.size()-1], NINE, THREE, part.hIntervalsCpCf[2][part.hIntervalsCpCf[2].size()-1]);
        }
    } else {
        dom(home, part.hIntervalsCpCf[2][part.hIntervalsCpCf[2].size()-1], pen3);
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