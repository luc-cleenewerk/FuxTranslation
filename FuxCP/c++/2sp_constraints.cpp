#include "headers/2sp_constraints.hpp"
#include "headers/Utilities.hpp"

void second_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE, 
    int list_index, int for_species){

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

    first_species_3v(home, parts, lowest, upper, triad_costs, succ_cost, for_species);

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

    avoid_perfect_consonances(home, parts[list_index].size, parts, succ_cost);

    variety_cost_constraint(home, parts[list_index].size, parts);

    prefer_harmonic_triads(home, parts[list_index].size, parts, lowest, upper, triad_costs);
}