#include "headers/1sp_constraints.hpp"
#include "headers/Utilities.hpp"

/*************************
 *      Constraints      *
 *************************/

 // todo move this into appropriate file (should be organised) for species and type of constraint (harmonic, melodic, motion, linking arrays,...)

/**
 * First species dispatcher for 2 voices. Calls necessary constraints given the species
*/
 void first_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, int for_species){

    link_harmonic_arrays_1st_species(home, parts[1].size, parts, lowest, upper);

    link_melodic_arrays_1st_species(home, parts[1].size, parts);

    for(int p = 0; p < parts.size(); p++){

        link_cfb_arrays_1st_species(home, parts[1].size, parts[p], parts[0], 0);

        link_motions_arrays(home, parts[p], parts[0], lowest, 0);
    }

    set_off_costs(home, parts[1].size, parts);

    set_step_costs(home, parts[1].size, parts);

    if(for_species==1){
        harmonic_intervals_consonance(home, parts, PENULT_CONS);
    } else if(for_species == 2){
        harmonic_intervals_consonance(home, parts, PENULT_THESIS);
    } else if(for_species==3){
        harmonic_intervals_consonance(home, parts, PENULT_Q);
    }

    voices_cannot_play_same_note(home, parts[1].size, parts);

    key_tone_tuned_to_cantusfirmus(home, parts[1].size, parts);

    no_chromatic_melodies(home, parts[1].size, parts);


    if(for_species==1 || for_species==2){
        perfect_consonance_constraints(home, parts[1].size, parts);
    }

    if(for_species==1){

        penultimate_note_must_be_major_sixth_or_minor_third(home, parts[1].size, parts);

        melodic_intervals_not_exceed_minor_sixth(home, parts[1].size, parts);

        no_direct_perfect_consonance(home, parts[1].size, parts, for_species);

        no_battuta(home, parts[1].size, parts);

        imperfect_consonances_are_preferred(home, parts[1].size, parts);

        no_tritonic_intervals(home, parts[1].size, parts);

    }
 }

 /**
 * First species dispatcher for 3 voices. Calls necessary constraints given the species
*/
 void first_species_3v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, int for_species){

    link_harmonic_arrays_1st_species(home, parts[1].size, parts, lowest, upper);

    link_melodic_arrays_1st_species(home, parts[1].size, parts);

    for(int p = 0; p < parts.size(); p++){

        link_cfb_arrays_1st_species(home, parts[1].size, parts[p], parts[0], 0);

        link_motions_arrays(home, parts[p], parts[0], lowest, 0);
    }

    set_off_costs(home, parts[1].size, parts);

    set_step_costs(home, parts[1].size, parts);

    if(for_species==1){
        harmonic_intervals_consonance(home, parts, PENULT_CONS);
    } else if(for_species == 2){
        harmonic_intervals_consonance(home, parts, PENULT_THESIS);
    } else if(for_species==3){
        harmonic_intervals_consonance(home, parts, PENULT_Q);
    }

    voices_cannot_play_same_note(home, parts[1].size, parts);

    key_tone_tuned_to_cantusfirmus(home, parts[1].size, parts);

    no_chromatic_melodies(home, parts[1].size, parts);  // in 3v too?

    // if(for_species==1 || for_species==2){                                        
    //     perfect_consonance_constraints(home, parts[1].size, parts);              // suspended in 3v
    // }


    // APPLY 3V GENERAL : 

    //voices cannot play the same note except on first and last beat already checked off in first species calls

    no_same_direction(home, parts[1].size, parts);

    last_chord_no_minor_third(home, parts[0].size, parts);

    no_tenth_in_last_chord(home, parts[1].size, parts, upper, lowest);

    //last chord must be a major triad -> already checked by h_consonances? or is it only 3 voices?

    last_chord_same_fundamental(home, lowest, parts);

    no_chromatic_melodies(home, parts[0].size, parts); //havent seen it with anton yet


    if(for_species==1){

        penultimate_note_must_be_major_sixth_or_minor_third(home, parts[1].size, parts);
    
        melodic_intervals_not_exceed_minor_sixth(home, parts[1].size, parts);

        no_direct_perfect_consonance(home, parts[1].size, parts, for_species);

        no_battuta(home, parts[1].size, parts);

        imperfect_consonances_are_preferred(home, parts[1].size, parts);

        no_tritonic_intervals(home, parts[1].size, parts);

        no_successive_ascending_sixths(home, parts[1].size, parts);

        variety_cost_constraint(home, parts[1].size, parts);

        avoid_perfect_consonances(home, parts[1].size, parts);

        prefer_harmonic_triads(home, parts[1].size, parts, lowest, upper);
    }
 }

 /**
 * First species dispatcher for 4 voices. Calls necessary constraints given the species
*/
 void first_species_4v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, int for_species){
    
    link_harmonic_arrays_1st_species(home, parts[1].size, parts, lowest, upper);

    link_melodic_arrays_1st_species(home, parts[1].size, parts);

    for(int p = 0; p < parts.size(); p++){

        link_cfb_arrays_1st_species(home, parts[1].size, parts[p], parts[0], 0);

        link_motions_arrays(home, parts[p], parts[0], lowest, 0);
    }
    
    set_off_costs(home, parts[1].size, parts);

    set_step_costs(home, parts[1].size, parts);

    // H1
    if(for_species==1){
        harmonic_intervals_consonance(home, parts, PENULT_CONS);
    } else if(for_species == 2){                              
        harmonic_intervals_consonance(home, parts, PENULT_THESIS);
    } else if(for_species==3){
        harmonic_intervals_consonance(home, parts, PENULT_Q);
    }

    // if(for_species <= 10){                                               // H5 : only for 2 or 3 voices, too restrictive for 4 voices. or maybe just relax it?
    //     voices_cannot_play_same_note(home, parts[1].size, parts);
    // }

    // H4
    key_tone_tuned_to_cantusfirmus(home, parts[1].size, parts);

    // G7 (G6 dans fux_rules 4v)
    // no_chromatic_melodies(home, parts[1].size, parts);  // TODO : relax in 4v (cost, not interdiction)

    // if(for_species==1 || for_species==6){
    //     penultimate_note_must_be_major_sixth_or_minor_third(home, parts[1].size, parts);    // suspended for 4 voices
    // }


    // APPLY 3V GENERAL : 

    //voices cannot play the same note except on first and last beat already checked off in first species calls

    // P6
    no_same_direction(home, parts[1].size, parts);

    // H12
    last_chord_no_minor_third(home, parts[0].size, parts);

    // no_tenth_in_last_chord(home, parts[1].size, parts, upper, lowest);       // suspended for 4 voices

    //last chord must be a major triad -> already checked by h_consonances? or is it only 3 voices?     TODO check 4v

    // G10 (G9 dans fux_rules 4v)
    last_chord_same_fundamental(home, lowest, parts);

    if(for_species==1){

        // M2/M6
        melodic_intervals_not_exceed_minor_sixth(home, parts[1].size, parts);   // TODO modify for 4 voices : should accept octave

        // P1
        no_direct_perfect_consonance(home, parts[1].size, parts, for_species);

        // P3
        no_battuta(home, parts[1].size, parts);     

        // H6
        imperfect_consonances_are_preferred(home, parts[1].size, parts);    // Only for first species?

        // M1
        no_tritonic_intervals(home, parts[1].size, parts);

        // P7
        no_successive_ascending_sixths(home, parts[1].size, parts);

        // M4
        variety_cost_constraint(home, parts[1].size, parts);

        // P4
        avoid_perfect_consonances(home, parts[1].size, parts);

        // H8
        // prefer_harmonic_triads(home, parts[1].size, parts, lowest, upper);              // TODO modify 4v
        }
 }