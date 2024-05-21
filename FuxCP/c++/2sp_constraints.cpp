#include "headers/2sp_constraints.hpp"
#include "headers/Utilities.hpp"

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
        for(int i = 0; i < part.sol_len-4; i++){
            rel(home, part.solution_array[i], IRT_NQ, part.solution_array[i+1]);
        }
        //last solutionarray 3, rest (last solutionarray 3)
        //why does anton do that for 3 voices? fux specifically says there CAN be unison
    }
}

void no_direct_move_perfect_consonance_2nd_species(const Home &home, Part part){
    //real motions, is_p_cons_array, is_not_lowest
    
}