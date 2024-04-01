#include "../headers/1st-ctp.hpp"
#include <vector>

void* create_new_first_problem(vector<int> counterpoint, int species){
    return (void*) new FirstSpecies(counterpoint, species);
}