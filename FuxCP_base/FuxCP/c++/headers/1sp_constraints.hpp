#include <vector>

#include "Part.hpp"
#include "Stratum.hpp"

using namespace Gecode;
using namespace std;

/*************************
 *      Constraints      *
 *************************/
void link_harmonic_arrays_1st_species(const Home &home, int size, vector<Part> parts, vector<Stratum> lowest, 
    vector<Stratum> upper);

void link_cfb_arrays_1st_species(const Home &home, int size, vector<Part> parts);

void link_melodic_arrays_1st_species(const Home &home, int size, vector<Part> parts);

void link_motions_arrays(const Home &home, int size, int con_motion_cost, int obl_motion_cost, int dir_motion_cost,
     vector<Part> parts, vector<Stratum> lowest);

void imperfect_consonances_are_preferred(const Home &home, int size, vector<Part> parts, int costpcons);

void perfect_consonance_constraints(const Home &home, int size, vector<Part> parts, int n_species);

void key_tone_tuned_to_cantusfirmus(const Home &home, int size, vector<Part> parts);

void voices_cannot_play_same_note(const Home &home, int size, vector<Part> parts);

void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, int size, vector<Part> parts);

void no_tritonic_intervals(const Home &home, int size, int costtri, vector<Part> parts);

void melodic_intervals_not_exceed_minor_sixth(const Home &home, int size, vector<Part> parts);

void variety_cost_constraint(const Home &home, int size, vector<Part> parts);

void no_direct_perfect_consonance(const Home &home, int size, vector<Part> parts, int n_species);

void create_lowest(vector<int> cantusFirmus, vector<IntVarArray> counterpoints, IntVarArray lowest, int size);

void no_tenth_in_last_chord(const Home &home, int size, vector<Part> parts, vector<Stratum> upper, vector<Stratum> lowest);

void last_chord_no_minor_third(const Home &home, int size, vector<Part> parts);

void no_battuta(const Home &home, int size, vector<Part> parts);

void avoid_perfect_consonances(const Home &home, int size, vector<Part> parts);

void no_same_direction(const Home &home, int size, vector<Part> parts, int n_species);

void no_successive_ascending_sixths(const Home &home, int size, vector<Part> parts, int n_species);