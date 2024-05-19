#include <vector>

#include "Part.hpp"
#include "Stratum.hpp"
#include "1sp_constraints.hpp"

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