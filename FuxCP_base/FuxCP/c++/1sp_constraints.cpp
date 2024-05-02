#include "headers/1sp_constraints.hpp"
#include "headers/Utilities.hpp"

/*************************
 *      Constraints      *
 *************************/

 // todo move this into appropriate file (should be organised) for species and type of constraint (harmonic, melodic, motion, linking arrays,...)

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
            rel(home, parts[j].hIntervalsCpCf[i] == abs(parts[j].notes[i] - lowest[0].notes[i])); //assigns the hIntervals
        }
    }

    for(int p = 0; p < parts.size()-1; p++){
        for(int i = 0; i < size; i++){
            rel(home, expr(home, lowest[0].notes[i]-upper[p].notes[i]), IRT_EQ, upper[p].hIntervalsBrut[i]); //assigns the hIntervals between the lowest stratum
            abs(home, upper[p].hIntervalsBrut[i], upper[p].hIntervalsAbs[i]); //also creates the absolute hInterval
        }
    }
}

void link_cfb_arrays_1st_species(const Home &home, int size, vector<Part> parts){
    for(int k = 1; k < parts.size(); k++){
        for(int i = 0; i < size; i++){
            rel(home, parts[k].notes[i], IRT_GQ, parts[0].notes[i], Reify(parts[k].isCFB[i], RM_EQV)); //links the CFB values in the array
        }
    }
}

void link_melodic_arrays_1st_species(const Home &home, int size, vector<Part> parts){
    //works
    for(int k = 0; k < parts.size(); k++){
        for(int i = 0; i < size-1; i++){
            rel(home, expr(home, parts[k].notes[i]-parts[k].notes[i+1]), IRT_EQ, parts[k].m_intervals_brut[i]); //assigns the melodic interval (brut)
            abs(home, parts[k].m_intervals_brut[i], parts[k].m_intervals[i]); //assigns the melodic interval (absolute)
        }
    }
}

void link_motions_arrays(const Home &home, int size, int con_motion_cost, int obl_motion_cost, int dir_motion_cost, vector<Part> parts, 
     vector<Stratum> lowest){
        for(int p = 1; p < parts.size(); p++){
            for(int i = 0; i < size-1; i++){
                //direct motions help creation
                BoolVar both_up = expr(home, (parts[p].m_intervals_brut[i]>0)&&(parts[0].m_intervals_brut[i]>0)); //if both parts are going in the same direction
                BoolVar both_stay = expr(home, (parts[p].m_intervals_brut[i]==0)&&(parts[0].m_intervals_brut[i]==0)); //if both parts are staying
                BoolVar both_down = expr(home, (parts[p].m_intervals_brut[i]<0)&&(parts[0].m_intervals_brut[i]<0)); //if both parts are going down
                //oblique motions help creation
                BoolVar cf_stays_1 = expr(home, (parts[p].m_intervals_brut[i]>0)&&(parts[0].m_intervals_brut[i]==0)); //if the lowest part stays and one goes up
                BoolVar cf_stays_2 = expr(home, (parts[p].m_intervals_brut[i]<0)&&(parts[0].m_intervals_brut[i]==0)); //if the lowest part stays and one goes down
                BoolVar cp_stays_1 = expr(home, (parts[p].m_intervals_brut[i]==0)&&(parts[0].m_intervals_brut[i]>0)); //if the lowest part goes up and one stays
                BoolVar cp_stays_2 = expr(home, (parts[p].m_intervals_brut[i]==0)&&(parts[0].m_intervals_brut[i]<0)); //if the lowest part goes down and one stays
                //contrary motions help creation
                BoolVar cpd_cfu = expr(home, (parts[p].m_intervals_brut[i]<0)&&(parts[0].m_intervals_brut[i]>0)); //if the cf goes up and the cp down
                BoolVar cpu_cfd = expr(home, (parts[p].m_intervals_brut[i]>0)&&(parts[0].m_intervals_brut[i]<0)); //if the cf goes down and the cp up
                //direct constraints
                rel(home, ((both_up || both_stay || both_down) && (parts[p].isCFB[i]==1)) >> (parts[p].motions[i]==2));
                rel(home, ((both_up || both_stay || both_down) && (parts[p].isCFB[i]==1)) >> (parts[p].motions_cost[i]==dir_motion_cost));
                //oblique constraints
                rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (parts[p].isCFB[i]==1)) >> (parts[p].motions[i]==1));
                rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (parts[p].isCFB[i]==1)) >> (parts[p].motions_cost[i]==obl_motion_cost));
                //contrary constraints
                rel(home, ((cpd_cfu || cpu_cfd)&& (parts[p].isCFB[i]==1)) >> (parts[p].motions[i]==0));
                rel(home, ((cpd_cfu || cpu_cfd)&& (parts[p].isCFB[i]==1)) >> (parts[p].motions_cost[i]==con_motion_cost));
                //bass constraints
                rel(home, (parts[p].isCFB[i]==0) >> (parts[p].motions[i]==-1));
                rel(home, (parts[p].isCFB[i]==0) >> (parts[p].motions_cost[i]==0));
            }
        }
     }

void harmonic_intervals_consonance(const Home &home, vector<Part> parts){
    for(int p = 0; p < parts.size(); p++){
        for(int i = 0; i < parts[p].hIntervalsCpCf.size()-1; i++){
            dom(home, parts[p].hIntervalsCpCf[i], consonances); //constraint : all intervals are consonances 
        }
        dom(home, parts[p].hIntervalsCpCf[parts[p].hIntervalsCpCf.size()-1], major_h_triad); //major h triad constraint
    }
}

void perfect_consonance_constraints(const Home &home, int size, vector<Part> parts, int n_species){
    if(n_species==1){
        for(int p = 0; p < parts.size(); p++){
            dom(home, parts[p].hIntervalsCpCf[0], perfect_consonance); //first is a perfect consonance
            dom(home, parts[p].hIntervalsCpCf[size-1], perfect_consonance); //last is a perfect consonance
        }
    }
}

void imperfect_consonances_are_preferred(const Home &home, int size, vector<Part> parts, int costpcons){
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            //the two constraints set the cost : if it is either a unisson or a perfect fifth, the cost is set. else, it is 0
            rel(home, (parts[p].hIntervalsCpCf[i]==UNISSON || parts[p].hIntervalsCpCf[i]==PERFECT_FIFTH) >> (parts[p].P_cons_cost[i]==costpcons));
            rel(home, (parts[p].hIntervalsCpCf[i]!=UNISSON && parts[p].hIntervalsCpCf[i]!=PERFECT_FIFTH) >> (parts[p].P_cons_cost[i]==0));
        }
    }
}

void key_tone_tuned_to_cantusfirmus(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        rel(home, (parts[p].isCFB[0] == 0) >> (parts[p].hIntervalsCpCf[0]==0)); //tuning the first to the key tune (if the cf is the bass)
        rel(home, (parts[p].isCFB[size-1] == 0) >> (parts[p].hIntervalsCpCf[size-1]==0)); //tuning the last to the key tune (if the cf is the bass)
    }
}

void voices_cannot_play_same_note(const Home &home, int size, vector<Part> parts){
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = 0; p2 < parts.size(); p2++){
            if(p2!=p1){
                for(int i = 1; i < size-1; i++){
                    rel(home, parts[p1].notes[i], IRT_NQ, parts[p2].notes[i]); //constraint that notes are different
                }
            }
        }
    }
}

void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, int size, vector<Part> parts){
    int p = size-1;
    
    if(parts.size()==3){ //if it is 3 voices
        for(int p = 0; p < parts.size(); p++){
            if(parts[p].is_not_lowest[size-2].assigned()){
                if(parts[p].is_not_lowest[size-2].val()==1){
                    //constraint that penultimate note is either a minor third, perfect fifth, major sixth or octave
                    dom(home, parts[p].hIntervalsCpCf[size-2], MIN3_PERF5_MAJ6_OCT);
                }
            }
        }
    } else { //else 2 voice constraints
        rel(home, parts[1].hIntervalsCpCf[p], IRT_EQ, 3, Reify(parts[0].is_not_lowest[p], RM_IMP)); //constraint : minor third
        rel(home, parts[1].hIntervalsCpCf[p], IRT_EQ, 9, Reify(parts[0].is_not_lowest[p], RM_IMP)); //constraint : major sixth
    }
}

void no_tritonic_intervals(const Home &home, int size, int costtri, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int j = 0; j < size-1; j++){
            rel(home, (parts[p].m_intervals[j]==6) >> (parts[p].M_deg_cost[j]==costtri)); //if it is a tritone : set a cost
            rel(home, (parts[p].m_intervals[j]!=6) >> (parts[p].M_deg_cost[j]==0)); //if not, the cost is 0
        }
    }
}

void melodic_intervals_not_exceed_minor_sixth(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
        for(int j = 0; j < size-1; j++){
            rel(home, parts[p].m_intervals[j], IRT_LQ, 8); //constraint : must not exceed a minor sixth
        }
    }
}

void no_direct_perfect_consonance(const Home &home, int size, vector<Part> parts, int n_species){
    if(n_species==1){ //if 2 voices
        for(int j = 0; j < size-1; j++){
            //it cannot go from either unisson or a perfect fifth to another one of these, then it is prohibited
            rel(home, (parts[1].hIntervalsCpCf[j]==0 || parts[1].hIntervalsCpCf[j]==7 || parts[1].hIntervalsCpCf[j+1]==0 || parts[1].hIntervalsCpCf[j+1]==7) >> 
            (parts[1].motions[j]!=2));
        }
    } else { //else if 3 voices
        for(int p = 1; p < parts.size(); p++){
            for(int j = 0; j < size-2; j++){
                //set a cost when it is reached through direct motion, it is 0 when not
                rel(home, (parts[p].motions[j]==2&&(parts[p].hIntervalsCpCf[j+1]==0||parts[p].hIntervalsCpCf[j+1]==7))>>(parts[p].direct_move_cost[j]==8));
                rel(home, (parts[p].motions[j]!=2||(parts[p].hIntervalsCpCf[j+1]!=0&&parts[p].hIntervalsCpCf[j+1]!=7))>>(parts[p].direct_move_cost[j]==0));
            }
        }
    }
}

void no_battuta(const Home &home, int size, vector<Part> parts){
    for(int p = 1; p < parts.size(); p++){
            for(int j = 0; j < size-1; j++){
            //constraints avoiding a battuta kind of motion
            rel(home, expr(home, !((parts[p].hIntervalsCpCf[j+1]==0)&&(parts[p].motions[j]==0)&&(parts[p].m_intervals_brut[j]<-4)&&(parts[p].isCFB[j]==1))));
            rel(home, expr(home, !((parts[p].hIntervalsCpCf[j+1]==0)&&(parts[p].motions[j]==0)&&(parts[0].m_intervals_brut[j]<-4)&&(parts[p].isCFB[j]==0))));
        }
    }
}

void no_tenth_in_last_chord(const Home &home, int size, vector<Part> parts, vector<Stratum> upper, vector<Stratum> lowest){

    for(int p = 0; p < parts.size()-1; p++){
        //prohibiting tenths by checking the hInterval
        rel(home, (upper[p].hIntervalsBrut[size-1]>12) >> (upper[p].hIntervals[size-1]!=3 && upper[p].hIntervals[size-1]!=4));
    }

}

void last_chord_no_minor_third(const Home &home, int size, vector<Part> parts){
    for(int p = 0; p < parts.size(); p++){
        if(parts[p].is_not_lowest[size-1].assigned()){
            if(parts[p].is_not_lowest[size-1].val()==1){
                //prohibiting minor thirds in the last chord
                rel(home, parts[p].hIntervalsCpCf[size-1], IRT_NQ, 3);
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
                rel(home, (parts[p].notes[j]==parts[p].notes[k])>>(parts[p].varietyArray[temp]==1));
                rel(home, (parts[p].notes[j]!=parts[p].notes[k])>>(parts[p].varietyArray[temp]==0));
                temp++;
            }
        }
    }
}

void avoid_perfect_consonances(const Home &home, int size, vector<Part> parts){
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = 0; p2 < parts.size(); p2++){
            if(p1!=p2){
                for(int j = 0; j < size-2;j++){
                    //constraint adding a cost if a perfect consonance is detected in successive order
                    rel(home, ((parts[p1].hIntervalsCpCf[j]==0||parts[p1].hIntervalsCpCf[j]==7) && 
                        (parts[p2].hIntervalsCpCf[j]==0||parts[p2].hIntervalsCpCf[j]==7) &&
                        (parts[p1].hIntervalsCpCf[j+1]==0||parts[p1].hIntervalsCpCf[j+1]==7) &&
                        (parts[p2].hIntervalsCpCf[j+1]==0||parts[p2].hIntervalsCpCf[j+1]==7)) >> (parts[p1].succ_cost[j]==2));
                    //cost is 0 if it is not the case
                    rel(home, ((parts[p1].hIntervalsCpCf[j]!=0&&parts[p1].hIntervalsCpCf[j]!=7) || 
                        (parts[p2].hIntervalsCpCf[j]!=0&&parts[p2].hIntervalsCpCf[j]!=7) ||
                        (parts[p1].hIntervalsCpCf[j+1]!=0&&parts[p1].hIntervalsCpCf[j+1]!=7) ||
                        (parts[p2].hIntervalsCpCf[j+1]!=0&&parts[p2].hIntervalsCpCf[j+1]!=7)) >> (parts[p1].succ_cost[j]==0));
                }
            }
        }
    }
}

void no_same_direction(const Home &home, int size, vector<Part> parts, int n_species){
    if(n_species==2){ //if it is 2 voices
        for(int j = 0; j < size-2; j++){
            //avoid direct motions
            rel(home, parts[1].motions[j]!=2&&parts[2].motions[j]!=2);
        }
    }
}

void no_successive_ascending_sixths(const Home &home, int size, vector<Part> parts, int n_species){
    if(n_species==2){ //if it is 2 voices
        for(int v1=0; v1 < parts.size(); v1++){
            for(int v2=0; v2 < parts.size(); v2++){
                if(v1!=v2){
                    for(int j = 1; j < size-2; j++){
                        //constraint checks successive ascending sixths
                        rel(home, ((parts[v1].hIntervalsCpCf[j-1]!=8 && parts[v1].hIntervalsCpCf[j-1]!=9 && 
                        parts[v2].hIntervalsCpCf[j-1]!=8 && parts[v2].hIntervalsCpCf[j-1]!=9)||
                        (parts[v1].hIntervalsCpCf[j]!=8 && parts[v1].hIntervalsCpCf[j]!=9 &&
                        parts[v2].hIntervalsCpCf[j]!=8 && parts[v2].hIntervalsCpCf[j]!=9))||
                        (parts[v1].m_intervals_brut[j]>0)||(parts[v2].m_intervals_brut[j]>0));
                    }
                }
            }
        }
    }
}