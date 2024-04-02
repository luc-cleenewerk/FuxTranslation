#include "../headers/1st-ctp.hpp"
#include <vector>

extern "C" {
    void* first_sp(){
        auto* pb = new FirstSpecies();
        return (void*) pb;
    }

    void* create_solver(void* sp){
        return (void*) make_solver(static_cast<FirstSpecies *>(sp));
    }

    void* get_next(void* solver){
        return (void *) get_next_solution(static_cast<BAB<FirstSpecies>*>(solver));
    }
}