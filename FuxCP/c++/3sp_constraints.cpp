#include "headers/3sp_constraints.hpp"
#include "headers/Utilities.hpp"

void third_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE, IntVar ZERO,
    int list_index, int for_species){

    first_species_2v(home, parts, lowest, upper, 3);

    link_harmonic_arrays_3rd_species(home, parts[list_index].size, parts, lowest);

    link_harmonic_to_cf_3rd_species(home, parts[list_index].size, parts[list_index], parts[0]);

    //link_melodic_arrays_3rd_species_in_meas(home, parts[list_index].size, parts); //why you no work

    link_melodic_arrays_3rd_species_next_meas(home, parts[list_index].size, parts[list_index]);

    link_m2_arrays_3rd_species(home, parts[list_index]);

    link_melodic_self_arrays_3rd_species(home, parts[list_index]);

    //3.P1
    link_motions_arrays_3rd_species(home, parts[list_index], lowest);

    link_cfb_array_3rd_species(home, parts[list_index].size, parts[list_index], parts[0]);

    link_is_qn_linked_3rd_species(home, parts[list_index]);

    link_ta_dim_array_3rd_species(home, parts[list_index]);

    link_is_cons_array_3rd_species(home, parts[list_index]);

    link_ciambatta_3rd_species(home, parts[list_index]);

    //is_off already covered in Part.cpp

    /**
     * ==================================================================
     *                      HARMONIC CONSTRAINTS
     * ==================================================================
    */

    //3.H4
    penult_cons(home, parts[list_index], PENULT_CONS_3P, NINE, THREE, 3);

    //the third note of the penultimate measure must be below the fourth one.
    //rel(home, parts[list_index].m_succ_intervals_brut[2][parts[list_index].m_succ_intervals_brut[2].size()-1], IRT_GR, 1);
    //the second note and the third note of the penultimate measure must be distant by greater than 1 semi-tone from the fourth note
    //rel(home, parts[list_index].m2_intervals[parts[list_index].size-2], IRT_NQ, 1);

    five_consecutive_notes(home, parts[list_index]);

    any_disonant_note(home, parts[list_index]);

    /**
     * ==================================================================
     *                      MELODIC CONSTRAINTS
     * ==================================================================
    */

    //no_melodic_interval_between(home, parts[list_index]);

    //no chromatic melodies taken in 1sp_constraints call

    //marcels_rule(home, parts[list_index]);

    /**
     * ==================================================================
     *                       MOTION CONSTRAINTS
     * ==================================================================
    */

    no_direct_move_perfect_consonance_3rd_species(home, parts[list_index]);

    no_battuta_3rd_species(home, parts[list_index]);

    /**
     * ==================================================================
     *                          COST FACTORS
     * ==================================================================
    */



    for(int i = 0; i < 4; i++){
        int sz = parts[list_index].size;
        if(i>0){
            sz-=1;
        }
        for(int p = 1; p < parts.size(); p++){
            imperfect_consonances_are_preferred(home, sz, parts[p], i); //2 costs
        }
    }

    set_off_costs(home, parts[list_index].size, parts); //check this again if it is done correctly

    set_step_costs(home, parts[list_index].size, parts[list_index], 3);

    set_cambiata(home, parts[list_index], ZERO); //this has to be changed once the m_succ works correctly

    set_m2_cost(home, parts[list_index]);

}