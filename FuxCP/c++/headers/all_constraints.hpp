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
void link_harmonic_arrays_1st_species(const Home &home, int size, vector<Part> parts, vector<Stratum> lowest, 
    vector<Stratum> upper);

/**
 * Initialization and calculation of the boolean array determining if the cantus firmus is the bass
*/
void link_cfb_arrays_1st_species(const Home &home, int size, Part part, Part cf, int idx);

/**
 * Initialization and calculation of the melodic intervals
*/
void link_melodic_arrays_1st_species(const Home &home, int size, vector<Part> parts);

/**
 * Initialization and calculation of the motions between the notes
*/
void link_motions_arrays(const Home &home, Part part, Part cf, vector<Stratum> lowest, int idx);

/**
 * Constraint H1 : All harmonic intervals must be consonances
*/
void harmonic_intervals_consonance(const Home &home, vector<Part> parts, IntSet pen);

/**
 * Constraint H6 : Imperfect consonances are preferred
*/
void imperfect_consonances_are_preferred(const Home &home, int size, vector<Part> parts);

/**
 * Constraint H2 and H3 : The first and last harmonic intervals must be a perfect consonance (only 2 voices)
*/
void perfect_consonance_constraints(const Home &home, int size, vector<Part> parts);

/**
 * Constraint H4 : The key tone is tuned to the first note of the cantus firmus
*/
void key_tone_tuned_to_cantusfirmus(const Home &home, int size, vector<Part> parts);

/**
 * Constraint H5 : The voices cannot play the same note at the same time (except in the first and last measure) 
*/
void voices_cannot_play_same_note(const Home &home, int size, vector<Part> parts);

/**
 * Constraint H7 and H8 : The harmonic interval of the penultimate note must be a major sixth or
 * a minor third depending on the cantus firmus pitch. When writing with three voices, the
 * harmonic interval must be either a minor third, a perfect fifth, a major sixth or an octave
*/
void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, int size, vector<Part> parts);

void prefer_harmonic_triads(const Home &home, int size, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper);

void set_off_costs(const Home &home, int size, vector<Part> parts);

void set_step_costs(const Home &home, int size, vector<Part> parts);

/**
 * Constraint M1 : Tritone melodic intervals are forbidden
*/
void no_tritonic_intervals(const Home &home, int size, vector<Part> parts);

/**
 * Constraint M2 : Melodic intervals cannot exceed a minor sixth interval
*/
void melodic_intervals_not_exceed_minor_sixth(const Home &home, int size, vector<Part> parts);

/**
 * Constraint M4 : The notes of each part should be as diverse as possible (only 3 voices)
*/
void variety_cost_constraint(const Home &home, int size, vector<Part> parts);

/**
 * Constraint P1 : Perfect consonances cannot be reached by direct motion (cost when it is 3 voices)
*/
void no_direct_perfect_consonance(const Home &home, int size, vector<Part> parts, int n_species);

/**
 * Constraint H10 : Tenths are prohibited in the last chord (only 3 voices)
*/
void no_tenth_in_last_chord(const Home &home, int size, vector<Part> parts, vector<Stratum> upper, vector<Stratum> lowest);

/**
 * Constraint H12 : Last chord cannot include a minor third (only 3 voices)
*/
void last_chord_no_minor_third(const Home &home, int size, vector<Part> parts);

/**
 * Constraint P3 : At the start of any measure, an octave cannot be reached by the lower voice going up
 * and the upper voice going down more than a third skip
*/
void no_battuta(const Home &home, int size, vector<Part> parts);

/**
 * Constraint P4 : Successive perfect consonances should be avoided (only 3 voices)
*/
void avoid_perfect_consonances(const Home &home, int size, vector<Part> parts);

/**
 * Constraint P6 : It is prohibited that all parts move in the same direction (only 3 voices)
*/
void no_same_direction(const Home &home, int size, vector<Part> parts);

/**
 * Constraint P7 : It is prohibited to use successive ascending sixths on a direct upwards motion (only 3 voices)
*/
void no_successive_ascending_sixths(const Home &home, int size, vector<Part> parts);

#endif