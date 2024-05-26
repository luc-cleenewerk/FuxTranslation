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

    for(int p = 0; p < parts.size()-1; p++){
        for(int i = 0; i < size; i++){
            //rel(home, expr(home, (lowest[0].notes[i]-lowest[0].notes[i])%12), IRT_EQ, lowest[0].hIntervalsBrut[i]);
            //abs(home, lowest[0].hIntervalsBrut[i], lowest[0].hIntervalsAbs[i]);
            rel(home, expr(home, (lowest[0].notes[i]-upper[p].notes[i])%12), IRT_EQ, upper[p].hIntervalsBrut[i]); //assigns the hIntervals between the lowest stratum
            abs(home, upper[p].hIntervalsBrut[i], upper[p].hIntervalsAbs[i]); //also creates the absolute hInterval
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
                BoolVar both_up = expr(home, (part.m_intervals_brut[idx][i]>0)&&(cf.m_intervals_brut[0][i]>0)); //if both parts are going in the same direction
                BoolVar both_stay = expr(home, (part.m_intervals_brut[idx][i]==0)&&(cf.m_intervals_brut[0][i]==0)); //if both parts are staying
                BoolVar both_down = expr(home, (part.m_intervals_brut[idx][i]<0)&&(cf.m_intervals_brut[0][i]<0)); //if both parts are going down
                //oblique motions help creation
                BoolVar cf_stays_1 = expr(home, (part.m_intervals_brut[idx][i]>0)&&(cf.m_intervals_brut[0][i]==0)); //if the lowest part stays and one goes up
                BoolVar cf_stays_2 = expr(home, (part.m_intervals_brut[idx][i]<0)&&(cf.m_intervals_brut[0][i]==0)); //if the lowest part stays and one goes down
                BoolVar cp_stays_1 = expr(home, (part.m_intervals_brut[idx][i]==0)&&(cf.m_intervals_brut[0][i]>0)); //if the lowest part goes up and one stays
                BoolVar cp_stays_2 = expr(home, (part.m_intervals_brut[idx][i]==0)&&(cf.m_intervals_brut[0][i]<0)); //if the lowest part goes down and one stays
                //contrary motions help creation
                BoolVar cpd_cfu = expr(home, (part.m_intervals_brut[idx][i]<0)&&(cf.m_intervals_brut[0][i]>0)); //if the cf goes up and the cp down
                BoolVar cpu_cfd = expr(home, (part.m_intervals_brut[idx][i]>0)&&(cf.m_intervals_brut[0][i]<0)); //if the cf goes down and the cp up
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
                rel(home, (part.is_not_lowest[i]==0) >> (part.motions[idx][i]==-1));
                rel(home, (part.is_not_lowest[i]==0) >> (part.motions_cost[idx][i]==0));
            }
     }

void link_p_cons_array(const Home &home, Part part, int variant){ //nothing to change here, it's fine
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

void imperfect_consonances_are_preferred(const Home &home, int size, vector<Part> parts, vector<Stratum> upper){
    for(int p = 0; p < parts.size()-1; p++){
        for(int i = 0; i < size; i++){
            //the two constraints set the cost : if it is either a unisson or a perfect fifth, the cost is set. else, it is 0
            rel(home, (upper[p].hIntervalsAbs[i]==UNISSON) >> (parts[p+1].octave_costs[i]==parts[p+1].h_octave));
            rel(home, (upper[p].hIntervalsAbs[i]==PERFECT_FIFTH) >> (parts[p+1].fifth_costs[i]==parts[p+1].h_fifth));
            rel(home, (upper[p].hIntervalsAbs[i]!=UNISSON) >> (parts[p+1].octave_costs[i]==0));
            rel(home, (upper[p].hIntervalsAbs[i]!=PERFECT_FIFTH) >> (parts[p+1].fifth_costs[i]==0));
        }
    }
}

void key_tone_tuned_to_cantusfirmus(const Home &home, int size, vector<Part> parts){ //nothing to change here, it's fine
    for(int p = 1; p < parts.size(); p++){
        rel(home, (parts[0].is_not_lowest[0] == 1) >> (parts[p].hIntervalsCpCf[0][0]==0)); //tuning the first to the key tune (if the cf is the bass)
        rel(home, (parts[0].is_not_lowest[size-1] == 1) >> (parts[p].hIntervalsCpCf[0][size-1]==0)); //tuning the last to the key tune (if the cf is the bass)
    }
}

void voices_cannot_play_same_note(const Home &home, int size, vector<Part> parts){
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = 0; p2 < parts.size(); p2++){
            if(p2!=p1){
                for(int i = 1; i < size-1; i++){
                    rel(home, parts[p1].vector_notes[0][i], IRT_NQ, parts[p2].vector_notes[0][i]); //constraint that notes are different
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
        ite(home, parts[0].is_not_lowest[size-2], THREE, NINE, upper[0].hIntervalsAbs[size-2]);
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
            //it cannot go from either unisson or a perfect fifth to another one of these, then it is prohibited
            rel(home, (upper[0].hIntervalsAbs[j+1]==0 || upper[0].hIntervalsAbs[j+1]==7) >> (parts[1].motions[0][j]!=2));
            //if(j!=size-2){
            //    rel(home, parts[1].direct_move_cost[j], IRT_EQ, 0);
            //}
        }
    } else { //else if 3 voices
        for(int p = 1; p < parts.size(); p++){ //should be fine i guess
            for(int j = 0; j < size-2; j++){
                //set a cost when it is reached through direct motion, it is 0 when not
                rel(home, (parts[p].motions[0][j]==2&&(parts[p].hIntervalsCpCf[0][j+1]==0||parts[p].hIntervalsCpCf[0][j+1]==7))>>(parts[p].direct_move_cost[j]==parts[p].direct_move));
                rel(home, (parts[p].motions[0][j]!=2||(parts[p].hIntervalsCpCf[0][j+1]!=0&&parts[p].hIntervalsCpCf[0][j+1]!=7))>>(parts[p].direct_move_cost[j]==0));
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

void avoid_perfect_consonances(const Home &home, int size, vector<Part> parts){ //nothing to change here, it's fine
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = 0; p2 < parts.size(); p2++){
            if(p1!=p2){
                for(int j = 0; j < size-2;j++){
                    //constraint adding a cost if a perfect consonance is detected in successive order
                    rel(home, ((parts[p1].hIntervalsCpCf[0][j]==0||parts[p1].hIntervalsCpCf[0][j]==7) && 
                        (parts[p2].hIntervalsCpCf[0][j]==0||parts[p2].hIntervalsCpCf[0][j]==7) &&
                        (parts[p1].hIntervalsCpCf[0][j+1]==0||parts[p1].hIntervalsCpCf[0][j+1]==7) &&
                        (parts[p2].hIntervalsCpCf[0][j+1]==0||parts[p2].hIntervalsCpCf[0][j+1]==7)) >> (parts[p1].succ_cost[j]==parts[p1].succ));
                    //cost is 0 if it is not the case
                    rel(home, ((parts[p1].hIntervalsCpCf[0][j]!=0&&parts[p1].hIntervalsCpCf[0][j]!=7) || 
                        (parts[p2].hIntervalsCpCf[0][j]!=0&&parts[p2].hIntervalsCpCf[0][j]!=7) ||
                        (parts[p1].hIntervalsCpCf[0][j+1]!=0&&parts[p1].hIntervalsCpCf[0][j+1]!=7) ||
                        (parts[p2].hIntervalsCpCf[0][j+1]!=0&&parts[p2].hIntervalsCpCf[0][j+1]!=7)) >> (parts[p1].succ_cost[j]==0));
                }
            }
        }
    }
}

void no_same_direction(const Home &home, int size, vector<Part> parts){
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = 0; p2 < parts.size(); p2++){
            if(p1!=p2){
                for(int n = 0; n < size-2; n++){
                    rel(home, expr(home, parts[p1].motions[0][n]==2), BOT_AND, expr(home, parts[p2].motions[0][n]==2), 0);
                }
            }
        }
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

void prefer_harmonic_triads(const Home &home, int size, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper){
    for(int i = 0; i < size; i++){
        rel(home, ((upper[0].hIntervalsAbs[i]==3||upper[0].hIntervalsAbs[i]==4)&& upper[1].hIntervalsAbs[i]==7) >> 
        (upper[0].triad_costs[i]==0 && upper[1].triad_costs[i]==0));
        rel(home, ((upper[0].hIntervalsAbs[i]!=3 && upper[0].hIntervalsAbs[i]!=4) || upper[1].hIntervalsAbs[i]!=7) >> 
        (upper[0].triad_costs[i]==upper[0].h_triad_cost && upper[1].triad_costs[i]==0)); //TODO : put triad costs as general array, not strat specific
    }
}

void set_off_costs(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            rel(home, (parts[p].is_off[0][i]==1) >> (parts[p].off_costs[i]==parts[p].off_cst));
            rel(home, (parts[p].is_off[0][i]==0) >> (parts[p].off_costs[i]==0));
        }
    }
}

void set_step_costs(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            rel(home, (parts[p].m_intervals[0][i]<3) >> (parts[p].m_degrees_cost[i]==parts[p].step_cost));
            rel(home, (parts[p].m_intervals[0][i]==3 || parts[p].m_intervals[0][i]==4) >> (parts[p].m_degrees_cost[i]==parts[p].third_cost));
            rel(home, (parts[p].m_intervals[0][i]==5) >> (parts[p].m_degrees_cost[i]==parts[p].fourth_cost));
            rel(home, (parts[p].m_intervals[0][i]==6) >> (parts[p].m_degrees_cost[i]==parts[p].tritone_cost));
            rel(home, (parts[p].m_intervals[0][i]==7) >> (parts[p].m_degrees_cost[i]==parts[p].fifth_cost));
            rel(home, (parts[p].m_intervals[0][i]==8 || parts[p].m_intervals[0][i]==9) >> (parts[p].m_degrees_cost[i]==parts[p].sixth_cost));
            rel(home, (parts[p].m_intervals[0][i]==10 || parts[p].m_intervals[0][i]==11) >> (parts[p].m_degrees_cost[i]==parts[p].seventh_cost));
            rel(home, (parts[p].m_intervals[0][i]==12) >> (parts[p].m_degrees_cost[i]==parts[p].octave_cost));
        }
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

