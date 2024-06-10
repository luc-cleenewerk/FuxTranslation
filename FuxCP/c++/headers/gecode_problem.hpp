#ifndef space_wrapper_hpp
#define space_wrapper_hpp

#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <exception>

#include "Part.hpp"
#include "Stratum.hpp"

using namespace Gecode;
using namespace Gecode::Search;
using namespace std;

/** Types of search engines */
enum {
    dfs_solver, //0
    bab_solver, //1
};

/*****************
 * Problem class *
 *****************/
class Problem: public IntMinimizeSpace {
protected:
    /// Input data
    int size;                       // The size of the variable array of interest
    int lower_bound_domain;
    int upper_bound_domain;
    int species;
    int costpcons;
    int costtritone;
    int con_motion_cost;
    int obl_motion_cost;
    int dir_motion_cost;
    int variety_cost;
    int tone_offset;
    int borrow_mode;
    int h_triad_cost;
    int n_unique_costs;
    int cost_size;

    unordered_map<string, int> prefs;
    IntVar global_cost;

    vector<int> cantusFirmus;
    vector<int> speciesList;
    vector<int> voice_types;
    vector<int> scale;
    vector<string> cost_names;

    vector<IntVarArray> counterpoints;
    vector<IntVarArray> hIntervals;
    vector<IntVarArray> cp_m_intervals;
    vector<IntVarArray> cp_m_intervals_brut;
    vector<BoolVarArray> cp_is_P_cons;
    vector<IntVarArray> sorted_voices;
    IntVarArray cost_factors;
    IntVarArray ordered_factors;
    vector<Part> parts;

    vector<Stratum> upper;
    vector<Stratum> lowest;

    vector<vector<string>> ordered_costs;
 
    /// variables
    IntVarArray cp;                 // The variables for the counterpoint
    IntVarArray hIntervalsCpCf;     // The intervals between the counterpoint and the cantus firmus
    BoolVarArray isCFB;
    IntVarArray m_intervals;
    IntVarArray m_intervals_brut;
    IntVarArray cf_m_intervals_brut;
    IntVarArray P_cons_cost;
    IntVarArray M_deg_cost;
    IntVarArray motions;
    IntVarArray motions_cost;
    BoolVarArray is_P_cons;
    IntVarArray triad_costs;
    IntVarArray succ_cost;

    IntVarArray vars;
    IntVarArray solution_array;
    IntVar empty;

public:
    /**
     * Constructor
     * @todo Modify this constructor depending on your problem. This constructor is where the problem is defined
     * @todo (variables, constraints, branching, ...)
     * @param s the size of the array of variables
     * @param l the lower bound of the domain of the variables
     * @param u the upper bound of the domain of the variables
     */
    Problem(vector<int> cf, int size, int n_cp, vector<int> splist, vector<int> voice_types, int borrow_mode, int min_skips, vector<int> general, 
        vector<int> motion, vector<int> melodic, vector<int> specific, vector<int> importance, int ton_off, vector<int> scale, int scale_size, 
        vector<int> chromatic, int chrom_size, vector<int> borrow, int borrow_size);

    /**
     * Copy constructor
     * @param s an instance of the Problem class
     * @todo modify this copy constructor to also copy any additional attributes you add to the class
     */
    Problem(Problem &s);

    /**
     * Returns the size of the problem
     * @return an integer representing the size of the vars array
     */
    int getSize();

    /**
     * Returns the values taken by the variables vars in a solution
     * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
     * @return an array of integers representing the values of the variables in a solution
     */
    int* return_solution();

    /**
     * Copy method
     * @return a copy of the current instance of the Problem class. Calls the copy constructor
     */
    virtual IntMinimizeSpace *copy(void);

    /**
     * Constrain method for bab search
     * @todo modify this function if you want to use branch and bound
     * @param _b a space to constrain the current instance of the Problem class with upon finding a solution
     */
    virtual void constrain(const IntMinimizeSpace& _b);

    virtual IntVar cost() const;

    /**
     * Prints the solution in the console
     */
    void print_solution();

    /**
     * toString method
     * @return a string representation of the current instance of the Problem class.
     * Right now, it returns a string "Problem object. size = <size>"
     * If a variable is not assigned when this function is called, it writes <not assigned> instead of the value
     * @todo modify this method to also print any additional attributes you add to the class
     */
    string toString();

    void create_strata();

};

/*************************
 * Search engine methods *
 *************************/

/**
 * Creates a search engine for the given problem
 * @todo Modify this function to add search options etc
 * @param pb an instance of the Problem class representing a given problem
 * @param type the type of search engine to create (see enumeration in headers/gecode_problem.hpp)
 * @return a search engine for the given problem
 */
Search::Base<Problem>* make_solver(Problem* pb, int type);

/**
 * Returns the next solution space for the problem
 * @param solver a solver for the problem
 * @return an instance of the Problem class representing the next solution to the problem
 */
Problem* get_next_solution_space(Search::Base<Problem>* solver);


/***********************
 * Auxiliary functions *
 ***********************/

/**
 * Write a text into a log file
 * @param message the text to write
 */
void writeToLogFile(const char* message);

string int_vector_to_string(vector<int> vector);

#endif