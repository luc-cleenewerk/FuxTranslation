#ifndef FIRST_HPP
#define FIRST_HPP

#include "all_constraints.hpp"

using namespace Gecode;
using namespace std;

void first_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, int species=1);

void first_species_3v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVarArray triad_costs, int species=1);

void first_species_4v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, int species=1);

#endif