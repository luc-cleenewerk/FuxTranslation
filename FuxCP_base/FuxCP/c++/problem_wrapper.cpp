#include "headers/problem_wrapper.hpp"
#include "headers/gecode_problem.hpp"

#include "headers/Utilities.hpp"

/**
 * Wraps the Problem constructor.
 * @todo modify this to include any parameters your Problem constructor requires
 * @param size an integer representing the size of the problem
 * @param lower_bound_domain an integer representing the lower bound of the domain of the variables
 * @param upper_bound_domain an integer representing the upper bound of the domain of the variables
 * @return A pointer to a Problem object casted as a void*
 */
void* create_new_problem(int size, int species, int* cantusFirmus, 
    int* splist, int* motions, int* voice_type, int offset, int* scale, int* borrowed, 
    int borrow_mode, int* off_scale, int* melodic, int* general){
    vector<int> cf(int_pointer_to_vector(cantusFirmus, size));
    vector<int> sp(int_pointer_to_vector(splist, sizeof(splist)));
    vector<int> mot(int_pointer_to_vector(motions, sizeof(motions)));
    vector<int> vt(int_pointer_to_vector(voice_type, sizeof(voice_type)));
    vector<int> scle(int_pointer_to_vector(scale, sizeof(scale)));
    vector<int> brw(int_pointer_to_vector(borrowed, sizeof(borrowed)));
    vector<int> ofscle(int_pointer_to_vector(off_scale, sizeof(off_scale)));
    vector<int> mel(int_pointer_to_vector(melodic, sizeof(melodic)));
    vector<int> gen(int_pointer_to_vector(general, sizeof(general)));
    unordered_map<string, int> pr = {{"fifth", 7},{"borrow", 8},{"octave", 5},{"succ", 2},{"variety", 9},{"triad", 3},{"motion", 12},{"melodic", 13}};
    return (void*) new Problem(size, species, cf, sp, mot, 
        vt, offset, scle, brw, borrow_mode, ofscle, pr, mel, gen);
}

/**
 * returns the size of the problem
 * @param sp a void* pointer to a Problem object
 * @return an integer representing the size of the problem
 */
int get_size(void* sp){
    return static_cast<Problem*>(sp)->getSize();
}

/**
 * returns the values of the variables for a solution
 * @param sp a void* pointer to a Problem object
 * @return an int* pointer representing the values of the variables
 */
int* return_solution(void* sp){
    return static_cast<Problem*>(sp)->return_solution();
}

/**
 * creates a search engine for Problem objects
 * @param sp a void* pointer to a Problem object
 * @return a void* cast of a Base<Problem>* pointer
 */
void* create_solver(void* sp, int type){
    return (void*) make_solver(static_cast<Problem*>(sp), type);
}

/**
 * returns the next solution space, it should be bound. If not, it will return NULL.
 * @param solver a void* pointer to a Base<Problem>* pointer for the search engine of the problem
 * @return a void* cast of a Problem* pointer
 */
void* return_next_solution_space(void* solver){
    return (void*) get_next_solution_space(static_cast<DFS<Problem>*>(solver));
}