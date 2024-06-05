#include "headers/2sp_constraints.hpp"
#include "headers/Utilities.hpp"

void second_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE, 
    int list_index, int for_species){

    link_harmonic_arrays_2nd_species(home, parts[list_index].size, parts[list_index], lowest);

    link_melodic_arrays_2nd_species_next_meas(home, parts[list_index].size, parts[list_index]);

    link_melodic_arrays_2nd_species_in_meas(home, parts[list_index].size, parts[list_index]);

    link_m2_arrays_2nd_species(home, parts[list_index]);

    link_melodic_self_arrays_2nd_species(home, parts[list_index]);

    link_motions_arrays_2nd_species(home, parts[list_index], lowest);
    
    link_real_motions_arrays_2nd_species(home, parts[list_index]);

    link_ta_dim_array_2nd_species(home, parts[list_index]);

    link_cfb_array_2nd_species(home, parts[list_index].size-1, parts[list_index], parts[0]);

    link_is_neighbour_array_2nd_species(home, parts[list_index], lowest);

    first_species_2v(home, parts, lowest, upper, for_species);

    h_cons_arsis(home, parts[list_index], PENULT_CONS);

    penult_cons(home, parts[list_index], PENULT_CONS_3P, NINE, THREE, 2);

    melodic_inter_arsis(home, parts[list_index]);

    no_chromatic_motion(home, parts[list_index]);

    no_unison_at_all(home, parts[list_index], for_species);

    no_direct_move_perfect_consonance_2nd_species(home, parts[list_index]);

    no_battuta_2nd_species(home, parts[list_index]);

    imperfect_consonances_are_preferred(home, parts[list_index].size, parts[list_index], 0);
    imperfect_consonances_are_preferred(home, parts[list_index].size-1, parts[list_index], 2);

    set_step_costs(home, parts[list_index].size, parts[list_index], 2);

    set_penult_sixth_cost(home, parts[list_index]);

}

void second_species_3v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE, 
    int list_index, IntVarArray triad_costs, IntVarArray succ_cost, int for_species){

    link_harmonic_arrays_2nd_species(home, parts[list_index].size, parts[list_index], lowest);

    link_melodic_arrays_2nd_species_next_meas(home, parts[list_index].size, parts[list_index]);

    link_melodic_arrays_2nd_species_in_meas(home, parts[list_index].size, parts[list_index]);

    link_m2_arrays_2nd_species(home, parts[list_index]);

    link_melodic_self_arrays_2nd_species(home, parts[list_index]);

    link_motions_arrays_2nd_species(home, parts[list_index], lowest);
    
    link_real_motions_arrays_2nd_species(home, parts[list_index]);

    link_ta_dim_array_2nd_species(home, parts[list_index]);

    link_cfb_array_2nd_species(home, parts[list_index].size-1, parts[list_index], parts[0]);

    link_is_neighbour_array_2nd_species(home, parts[list_index], lowest);

    first_species_3v(home, parts[list_index], parts[0], lowest, upper, triad_costs, for_species);

    h_cons_arsis(home, parts[list_index], PENULT_CONS);

    penult_cons(home, parts[list_index], PENULT_CONS_3P, NINE, THREE, 2);

    melodic_inter_arsis(home, parts[list_index]);

    no_chromatic_motion(home, parts[list_index]);

    no_unison_at_all(home, parts[list_index], 7);

    no_direct_move_perfect_consonance_2nd_species(home, parts[list_index]);

    no_battuta_2nd_species(home, parts[list_index]);

    imperfect_consonances_are_preferred(home, parts[list_index].size, parts[list_index], 0);
    imperfect_consonances_are_preferred(home, parts[list_index].size-1, parts[list_index], 2);

    set_step_costs(home, parts[list_index].size, parts[list_index], 2);

    set_penult_sixth_cost(home, parts[list_index]);

    variety_cost_constraint(home, parts[list_index]);
}


void second_species_4v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE, 
    int list_index, IntVarArray triad_costs, IntVarArray succ_cost, int for_species){

    link_harmonic_arrays_2nd_species(home, parts[list_index].size, parts[list_index], lowest);

    link_melodic_arrays_2nd_species_next_meas(home, parts[list_index].size, parts[list_index]);

    link_melodic_arrays_2nd_species_in_meas(home, parts[list_index].size, parts[list_index]);

    link_m2_arrays_2nd_species(home, parts[list_index]);

    link_melodic_self_arrays_2nd_species(home, parts[list_index]);

    link_motions_arrays_2nd_species(home, parts[list_index], lowest);
    
    link_real_motions_arrays_2nd_species(home, parts[list_index]);

    link_ta_dim_array_2nd_species(home, parts[list_index]);

    link_cfb_array_2nd_species(home, parts[list_index].size-1, parts[list_index], parts[0]);

    link_is_neighbour_array_2nd_species(home, parts[list_index], lowest);

    first_species_3v(home, parts[list_index], parts[0], lowest, upper, triad_costs, for_species);

    h_cons_arsis(home, parts[list_index], PENULT_CONS);

    penult_cons(home, parts[list_index], PENULT_CONS_3P, NINE, THREE, 2);

    melodic_inter_arsis(home, parts[list_index]);

    no_chromatic_motion(home, parts[list_index]);

    no_unison_at_all(home, parts[list_index], 7);

    no_direct_move_perfect_consonance_2nd_species(home, parts[list_index]);

    no_battuta_2nd_species(home, parts[list_index]);

    imperfect_consonances_are_preferred(home, parts[list_index].size, parts[list_index], 0);
    imperfect_consonances_are_preferred(home, parts[list_index].size-1, parts[list_index], 2);

    set_step_costs(home, parts[list_index].size, parts[list_index], 2);

    set_penult_sixth_cost(home, parts[list_index]);

    variety_cost_constraint(home, parts[list_index]);


}