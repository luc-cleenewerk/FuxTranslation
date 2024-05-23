#include <vector>

#include "Part.hpp"
#include "Stratum.hpp"
#include "all_constraints.hpp"

using namespace Gecode;
using namespace std;

void link_harmonic_arrays_2nd_species(const Home &home, int size, Part part, vector<Stratum> lowest);

void link_melodic_arrays_2nd_species_next_meas(const Home &home, int size, Part part);

void link_melodic_arrays_2nd_species_in_meas(const Home &home, int size, Part part);

void link_m2_arrays_2nd_species(const Home &home, Part part);

void link_melodic_self_arrays_2nd_species(const Home &home, Part part);

void link_motions_arrays_2nd_species(const Home &home, Part part, Part cf, vector<Stratum> lowest);

void link_real_motions_arrays_2nd_species(const Home &home, Part part);

void link_ta_dim_array_2nd_species(const Home &home, Part part);

void link_cfb_array_2nd_species(const Home &home, int size, Part part, Part cf);

void link_is_neighbour_array_2nd_species(const Home &home, Part part, vector<Stratum> lowest);

void h_cons_arsis(const Home &home, Part part, IntSet pen);

void penult_cons(const Home &home, Part part, IntSet pen3, IntVar NINE, IntVar THREE);

void melodic_inter_arsis(const Home &home, Part part);

void no_chromatic_motion(const Home &home, Part part);

void no_unison_at_all(const Home &home, Part part, int variant);

void no_direct_move_perfect_consonance_2nd_species(const Home &home, Part part);