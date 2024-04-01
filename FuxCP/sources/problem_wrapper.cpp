#include "../headers/1st-ctp.hpp"
#include <vector>

void* first_sp(vector<int> counterpoint, int species){
    return (void*) new FirstSpecies(counterpoint, species);
}