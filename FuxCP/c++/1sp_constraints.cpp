#include "headers/1sp_constraints.hpp"
#include "headers/Utilities.hpp"

/*************************
 *      Constraints      *
 *************************/

 // todo move this into appropriate file (should be organised) for species and type of constraint (harmonic, melodic, motion, linking arrays,...)

/**
 * First species dispatcher. Calls necessary constraints for necessary species
*/
 void first_species(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, int variant){

    link_harmonic_arrays_1st_species(home, parts[1].size, parts, lowest, upper);

    link_melodic_arrays_1st_species(home, parts[1].size, parts);

    for(int p = 0; p < parts.size(); p++){

        link_cfb_arrays_1st_species(home, parts[1].size, parts[p], parts[0], 0);

        link_motions_arrays(home, parts[p], parts[0], lowest, 0);
    }

    if(variant==1 || variant==6){
        harmonic_intervals_consonance(home, parts, PENULT_CONS);
    } else if(variant == 2 || variant==7){
        harmonic_intervals_consonance(home, parts, PENULT_THESIS);
    } else if(variant==3 || variant==8){
        harmonic_intervals_consonance(home, parts, PENULT_Q);
    }

    voices_cannot_play_same_note(home, parts[1].size, parts);

    key_tone_tuned_to_cantusfirmus(home, parts[1].size, parts);

    if(variant==1 || variant==2){
        perfect_consonance_constraints(home, parts[1].size, parts);
    }

    if(variant==1 || variant==6){
        penultimate_note_must_be_major_sixth_or_minor_third(home, parts[1].size, parts);
    }

    if(variant==1 || variant==6){
        melodic_intervals_not_exceed_minor_sixth(home, parts[1].size, parts);

        no_direct_perfect_consonance(home, parts[1].size, parts, variant);

        no_battuta(home, parts[1].size, parts);
    }

    if(variant==1 || variant==6){
        imperfect_consonances_are_preferred(home, parts[1].size, parts);

        no_tritonic_intervals(home, parts[1].size, parts);

        set_off_costs(home, parts[1].size, parts);

        set_step_costs(home, parts[1].size, parts);

        if(variant==6){
            no_successive_ascending_sixths(home, parts[1].size, parts);

            variety_cost_constraint(home, parts[1].size, parts);

            avoid_perfect_consonances(home, parts[1].size, parts);

            prefer_harmonic_triads(home, parts[1].size, parts, lowest, upper);
        }
    }
 }