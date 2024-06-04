#ifndef ALL_CONS
#define ALL_CONS

#include <vector>

#include "Part.hpp"
#include "Stratum.hpp"

using namespace Gecode;
using namespace std;

/**
 * Initialization and calculation of the hIntervals
*/
void link_harmonic_arrays_1st_species(const Home &home, Part part, vector<Stratum> lowest, vector<Stratum> upper, int list_index);

/**
 * Initialization and calculation of the boolean array determining if the cantus firmus is the bass
*/
void link_cfb_arrays_1st_species(const Home &home, int size, Part part, Part cf, int idx);

/**
 * Initialization and calculation of the melodic intervals
*/
void link_melodic_arrays_1st_species(const Home &home, Part part);

/**
 * Initialization and calculation of the motions between the notes
*/
void link_motions_arrays(const Home &home, Part part, vector<Stratum> lowest, int idx);

void link_p_cons_array(const Home &home, Part part);

/**
 * Constraint H1 and H9 : All harmonic intervals must be consonances
*/
void harmonic_intervals_consonance(const Home &home, Part part, IntSet pen);

/**
 * Constraint H6 : Imperfect consonances are preferred
*/
void imperfect_consonances_are_preferred(const Home &home, int size, Part part, int idx);

/**
 * Constraint H2 and H3 : The first and last harmonic intervals must be a perfect consonance (only 2 voices)
*/
void perfect_consonance_constraints(const Home &home, Part part);

/**
 * Constraint H4 : The key tone is tuned to the first note of the cantus firmus
*/
void key_tone_tuned_to_cantusfirmus(const Home &home, Part part, vector<Stratum> lowest);

/**
 * Constraint H5 : The voices cannot play the same note at the same time (except in the first and last measure) 
*/
void voices_cannot_play_same_note(const Home &home, vector<Part> parts);

/**
 * Constraint H7 and H8 : The harmonic interval of the penultimate note must be a major sixth or
 * a minor third depending on the cantus firmus pitch. When writing with three voices, the
 * harmonic interval must be either a minor third, a perfect fifth, a major sixth or an octave
*/
void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, Part part, IntVar NINE, IntVar THREE);

void prefer_harmonic_triads(const Home &home, vector<Part> parts, IntVarArray triad_costs);
void prefer_harmonic_triads_4v(const Home &home, vector<Stratum> upper, IntVarArray triad_costs);

void set_off_costs(const Home &home, Part part);

void set_step_costs(const Home &home, int size, Part part, int idx);

/**
 * Constraint M2 : Melodic intervals cannot exceed a minor sixth interval
*/
void melodic_intervals_not_exceed_minor_sixth(const Home &home, Part part);

/**
 * Constraint M4 : The notes of each part should be as diverse as possible (only 3 voices)
*/
void variety_cost_constraint(const Home &home, Part part);

/**
 * Constraint P1 : Perfect consonances cannot be reached by direct motion (cost when it is 3 voices)
*/
void no_direct_perfect_consonance(const Home &home, Part part, int n_species);

/**
 * Constraint H10 : Tenths are prohibited in the last chord (only 3 voices)
*/
void no_tenth_in_last_chord(const Home &home, vector<Stratum> upper, int list_index);

/**
 * Constraint H12 : Last chord cannot include a minor third (only 3 voices)
*/
void last_chord_no_minor_third(const Home &home, Part part);

/**
 * Constraint P3 : At the start of any measure, an octave cannot be reached by the lower voice going up
 * and the upper voice going down more than a third skip
*/
void no_battuta(const Home &home, Part part, Part cf);

/**
 * Constraint P4 : Successive perfect consonances should be avoided (only 3 voices)
*/
void avoid_perfect_consonances(const Home &home, int size, vector<Part> parts, IntVarArray succ_cost);

/**
 * Constraint P6 : It is prohibited that all parts move in the same direction (only 3 voices)
*/
void no_same_direction_3v(const Home &home, int size, vector<Part> parts);
void no_same_direction_4v(const Home &home, int size, vector<Part> parts);

/**
 * Constraint P7 : It is prohibited to use successive ascending sixths on a direct upwards motion (only 3 voices)
*/
void no_successive_ascending_sixths(const Home &home, int size, vector<Part> parts);

/**
 * G7 : Chromatic melodies are forbidden
*/
void no_chromatic_melodies(const Home &home, Part part);

/**
 * G10 : Last chord uses the same fundamental as the one of the scale used throughout the composition
*/
void last_chord_same_fundamental(const Home &home, vector<Stratum> lowest, vector<Part> parts);

/**
 * ========================================================================================================
 *                                      SECOND SPECIES CONSTRAINTS
 * ========================================================================================================
*/

void link_harmonic_arrays_2nd_species(const Home &home, int size, Part part, vector<Stratum> lowest);

void link_melodic_arrays_2nd_species_next_meas(const Home &home, int size, Part part);

void link_melodic_arrays_2nd_species_in_meas(const Home &home, int size, Part part);

void link_m2_arrays_2nd_species(const Home &home, Part part);

void link_melodic_self_arrays_2nd_species(const Home &home, Part part);

void link_motions_arrays_2nd_species(const Home &home, Part part, vector<Stratum> lowest);

void link_real_motions_arrays_2nd_species(const Home &home, Part part);

void link_ta_dim_array_2nd_species(const Home &home, Part part);

void link_cfb_array_2nd_species(const Home &home, int size, Part part, Part cf);

void link_is_neighbour_array_2nd_species(const Home &home, Part part, vector<Stratum> lowest);

void h_cons_arsis(const Home &home, Part part, IntSet pen);

void penult_cons(const Home &home, Part part, IntSet pen3, IntVar NINE, IntVar THREE, int idx);

void melodic_inter_arsis(const Home &home, Part part);

void no_chromatic_motion(const Home &home, Part part);

void no_unison_at_all(const Home &home, Part part, int variant);

void no_direct_move_perfect_consonance_2nd_species(const Home &home, Part part);

void no_battuta_2nd_species(const Home &home, Part part);

void set_penult_sixth_cost(const Home &home, Part part);

/**
 * ========================================================================================================
 *                                      THIRD SPECIES CONSTRAINTS
 * ========================================================================================================
*/

void link_harmonic_arrays_3rd_species(const Home &home, int size, vector<Part> parts, vector<Stratum> lowest);

void link_harmonic_to_cf_3rd_species(const Home &home, int size, Part part, Part cf);

void link_melodic_arrays_3rd_species_in_meas(const Home &home, int size, vector<Part> parts);

void link_melodic_arrays_3rd_species_next_meas(const Home &home, int size, Part part);

void link_m2_arrays_3rd_species(const Home &home, Part part);

void link_melodic_self_arrays_3rd_species(const Home &home, Part part);

void link_motions_arrays_3rd_species(const Home &home, Part part, vector<Stratum> lowest);

void link_cfb_array_3rd_species(const Home &home, int size, Part part, Part cf);

void link_is_qn_linked_3rd_species(const Home &home, Part part);

void link_ta_dim_array_3rd_species(const Home &home, Part part);

void link_is_cons_array_3rd_species(const Home &home, Part part);

void link_ciambatta_3rd_species(const Home &home, Part part);

/**
 * 3.H1
*/
void five_consecutive_notes(const Home &home, Part part);

/**
 * 3.H2
*/
void any_disonant_note(const Home &home, Part part);

/**
 * 3.H3 Cambiata
*/
void set_cambiata(const Home &home, Part part, IntVar ZERO);

/**
 * 3.M1 M2 cost
*/
void set_m2_cost(const Home &home, Part part);

void no_melodic_interval_between(const Home &home, Part part);

void marcels_rule(const Home &home, Part part);

void no_direct_move_perfect_consonance_3rd_species(const Home &home, Part part);

void no_battuta_3rd_species(const Home &home, Part part);






/**
 * ========================================================================================================
 *                                      TEST FUNCTIONS
 * ========================================================================================================
*/
void test_4v_fux(const Home &home, vector<Part> parts);

#endif