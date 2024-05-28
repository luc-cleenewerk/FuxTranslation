#include <vector>

#include "1sp_constraints.hpp"

using namespace Gecode;
using namespace std;

void second_species_2v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE, 
    int list_index, int species=2);

void second_species_3v(const Home &home, vector<Part> parts, vector<Stratum> lowest, vector<Stratum> upper, IntVar NINE, IntVar THREE, 
    int list_index, IntVarArray triad_costs, IntVarArray succ_cost, int for_species=2);