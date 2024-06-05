#include "headers/1sp_constraints.hpp"
#include "headers/Utilities.hpp"

/*************************
 *      Constraints      *
 *************************/

 // todo move this into appropriate file (should be organised) for species and type of constraint (harmonic, melodic, motion, linking arrays,...)

/**
 * First species dispatcher for 2 voices. Calls necessary constraints given the species
*/

 void first_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, int list_index, int for_species){

    for(int p = 0; p < parts.size(); p++){

        link_harmonic_arrays_1st_species(home, parts[p], lowest, upper, list_index-1); //also cf

        link_motions_arrays(home, parts[p], lowest, 0); //also cf

        link_p_cons_array(home, parts[p]); //also cf

        link_melodic_arrays_1st_species(home, parts[p]); //also cf

        link_cfb_arrays_1st_species(home, parts[1].size, parts[1], parts[0], 0); //also cf

        if(p!=0){

            set_step_costs(home, parts[1].size, parts[p], 0); //only cp

            set_off_costs(home, parts[p]); //only cp

            no_chromatic_melodies(home, parts[p]); //only cp

        }

        if(for_species==1){
            harmonic_intervals_consonance(home, parts[p], PENULT_CONS); //also cf
        } else if(for_species == 2){
            harmonic_intervals_consonance(home, parts[p], PENULT_THESIS); //also cf
        } else if(for_species==3){
            harmonic_intervals_consonance(home, parts[p], PENULT_Q);  //also cf
        }

        if(for_species==1 || for_species==2 || for_species==3){
            perfect_consonance_constraints(home, parts[p]); //also cf
        }

        if(for_species==1){

            penultimate_note_must_be_major_sixth_or_minor_third(home, parts[p], parts[0].NINE, parts[0].THREE); //also cf

            no_direct_perfect_consonance(home, parts[p], parts[1].speciesList.size()); //also cf

            if(p!=0){
                melodic_intervals_not_exceed_minor_sixth(home, parts[p]); //only cp

                no_battuta(home, parts[p], parts[0]); //only cp

                imperfect_consonances_are_preferred(home, parts[1].size, parts[p], 0); //only cp
            }
        }

    }

    voices_cannot_play_same_note(home, parts); //also cf (keep out of loop since the loop is important in the constraint)

    key_tone_tuned_to_cantusfirmus(home, parts[0], lowest); //only cf (keep out of loop)

 }

 /**
 * First species dispatcher for 3 voices. Calls necessary constraints given the species
*/
 void first_species_3v(const Home &home, Part part, Part cf, vector<Stratum> lowest, vector<Stratum> upper, IntVarArray triad_costs, int list_index, int for_species){

    link_harmonic_arrays_1st_species(home, part, lowest, upper, list_index-1);

    link_melodic_arrays_1st_species(home, part);

    link_cfb_arrays_1st_species(home, part.size, part, cf, 0);

    link_motions_arrays(home, part, lowest, 0);

    link_p_cons_array(home, part);

    if(part.species!=0){
        set_step_costs(home, part.size, part, 0); //cost

        set_off_costs(home, part);

        no_chromatic_melodies(home, part);

        last_chord_no_minor_third(home, part); //only cp

        no_tenth_in_last_chord(home, upper, list_index-1); //only upper, so if we don't include the cantusFirmus we have the size of the upper vector
    }

    if(for_species==1){
        harmonic_intervals_consonance(home, part, PENULT_CONS);
    } else if(for_species == 2){
        harmonic_intervals_consonance(home, part, PENULT_THESIS);
    } else if(for_species==3){
        harmonic_intervals_consonance(home, part, PENULT_Q);
    }

    if(for_species==1){

        penultimate_note_must_be_major_sixth_or_minor_third(home, part, cf.NINE, cf.THREE);

        no_direct_perfect_consonance(home, part, part.speciesList.size());
    
        if(part.species!=0){
            melodic_intervals_not_exceed_minor_sixth(home, part);

            no_battuta(home, part, cf);

            imperfect_consonances_are_preferred(home, part.size, part, 0);

            variety_cost_constraint(home, part); //only cp
        }
    }
 }

 /**
 * First species dispatcher for 4 voices. Calls necessary constraints given the species
*/
 void first_species_4v(const Home &home, Part part, Part cf, vector<Stratum> lowest, vector<Stratum> upper, IntVarArray triad_costs, 
    IntVarArray succ_cost, int list_index, int for_species){
    
    link_harmonic_arrays_1st_species(home, part, lowest, upper, list_index-1);

    link_melodic_arrays_1st_species(home, part);

    link_cfb_arrays_1st_species(home, part.size, part, cf, 0);

    link_motions_arrays(home, part, lowest, 0);   // P2 implemented in here

    link_p_cons_array(home, part);

    if(part.species!=0){
        set_step_costs(home, part.size, part, 0); // G8 and M1 implemented in here

        set_off_costs(home, part);

        // H12
        last_chord_no_minor_third(home, part);

        // G7
        // TODO add no_chromatic_melodies relaxation here
    }

    // H1, G9
    if(for_species==1){
        harmonic_intervals_consonance(home, part, consonances);
    } else if(for_species == 2){                              
        harmonic_intervals_consonance(home, part, PENULT_THESIS);
    } else if(for_species==3){
        harmonic_intervals_consonance(home, part, PENULT_Q);
    }

    if(for_species==1){
        
        // P1
        no_direct_perfect_consonance(home, part, part.speciesList.size());

        if(part.species!=0){
            
            // M2/M6
            melodic_intervals_not_exceed_minor_sixth(home, part);
            
            // P3
            no_battuta(home, part, cf);

            // H6
            imperfect_consonances_are_preferred(home, part.size, part, 0);

            // M4
            variety_cost_constraint(home, part); 
        }

    }

}

