#ifndef space_wrapper_hpp
#define space_wrapper_hpp

#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <exception>

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

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
class Problem: public Space {
protected:
    /// Input data
    int size;                       // The size of the variable array of interest
    int lower_bound_domain;
    int upper_bound_domain;
    int species;

    vector<int> cantusFirmus;

    /// variables
    IntVarArray cp;                 // The variables for the counterpoint
    IntVarArray hIntervalsCpCf;     // The intervals between the counterpoint and the cantus firmus
    BoolVarArray isCFB;
    IntVarArray m_intervals;
    IntVarArray m_intervals_brut;

public:
    /**
     * Constructor
     * @todo Modify this constructor depending on your problem. This constructor is where the problem is defined
     * @todo (variables, constraints, branching, ...)
     * @param s the size of the array of variables
     * @param l the lower bound of the domain of the variables
     * @param u the upper bound of the domain of the variables
     */
    Problem(int s, int l, int u, int sp, vector<int> cf);

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
    virtual Space *copy(void);

    /**
     * Constrain method for bab search
     * @todo modify this function if you want to use branch and bound
     * @param _b a space to constrain the current instance of the Problem class with upon finding a solution
     */
    virtual void constrain(const Space& _b);

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

};


/*************************
 *      Constraints      *
 *************************/

// todo move this into appropriate file (should be organised)
void link_harmonic_arrays_1st_species(const Home &home, int size, IntVarArray cp, IntVarArray hIntervalsCpCf, vector<int> cantusFirmus);

void link_cfb_arrays_1st_species(const Home &home, int size, IntVarArray cp, vector<int> cantusFirmus, BoolVarArray isCFB);

void link_melodic_arrays_1st_species(const Home &home, int size, IntVarArray cp, IntVarArray m_intervals, IntVarArray m_intervals_brut);

void perfect_consonance_constraints(const Home &home, int size, IntVarArray hIntervalsCpCf);

void key_tone_tuned_to_cantusfirmus(const Home &home, int size, BoolVarArray isCFB, IntVarArray hIntervalsCpCf);

void voices_cannot_play_same_note(const Home &home, int size, IntVarArray cp, vector<int> cantusFirmus);

void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, int size, IntVarArray hIntervalsCpCf, BoolVarArray isCFB);

void melodic_intervals_not_exceed_minor_sixth(const Home &home, int size, IntVarArray m_intervals);
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