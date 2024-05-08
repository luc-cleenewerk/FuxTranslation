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
void* create_new_problem(int size, int lower_bound_domain, int upper_bound_domain, int species, int* cantusFirmus, int pcost, int mtricost, 
    int* splist, int con, int obl, int dir, int var_cost){
    vector<int> cf(int_pointer_to_vector(cantusFirmus, size));
    vector<int> sp(int_pointer_to_vector(splist, sizeof(splist)));

    // Hardcoded some values for now
    return (void*) new Problem(size, lower_bound_domain, upper_bound_domain, species, cf, pcost, mtricost, sp, con, obl, dir, var_cost, {3, 1}, 0, {0,2,4,5,7,9,11,12,14,16,17,19,21,23,24,26,28,29,31,33,35,36,38,40,41,43,45,47,48,50,52,53,55,57,59,60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,86,88,89,91,93,95,96,98,100,101,103,105,107,108,110,112,113,115,117,119,120,122,124,125,127}, {0,5,9,11,12,17,21,23,24,29,33,35,36,41,45,47,48,53,57,59,60,65,69,71,72,77,81,83,84,89,93,95,96,101,105,107,108,113,117,119,120,125}, 0, 1);
}

/**
 * returns the size of the problem
 * @param sp a void* pointer to a Problem object
 * @return an integer representing the size of the problem
 */
int get_size(void* sp){
    return static_cast<Problem*>(sp)->getSize();
}

int test_cffi(int n){
    writeToLogFile("test_cffi");
    return n+1;
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