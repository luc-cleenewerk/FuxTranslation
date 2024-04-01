#include "../headers/1st-ctp.hpp"
#include <vector>

extern "C" {
    void* first_sp(int counterpoint, int species){
        return (void*) new FirstSpecies(counterpoint, species);
    }
}