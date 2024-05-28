#ifndef THIRD_HPP
#define THIRD_HPP

#include <vector>
#include "1sp_constraints.hpp"

using namespace Gecode;
using namespace std;

void third_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE,
    int list_index, int for_species=3);

#endif