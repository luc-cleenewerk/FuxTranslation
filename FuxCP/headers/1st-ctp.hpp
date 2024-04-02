#ifndef space_wrapper_hpp
#define space_wrapper_hpp

#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <exception>
#include <algorithm>
#include <iterator>

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

struct{
    vector<int> cp_range;
    vector<int> cp_dom;
    vector<int> chrom_cp_dom;
    vector<int> ext_cp_dom;
    vector<int> off_dom;
    int voice_type;
    int species;
}Counterpoint;

class FirstSpecies: public Space {

protected:
    IntVarArray cp_range;
    IntVarArray cp_dom;
    IntVarArray chrom_cp_dom;
    IntVarArray ext_cp_dom;
    IntVarArray off_dom;
    int voice_type;
    int species;

public:
    FirstSpecies();

    FirstSpecies(int species, vector<int> notes);

    FirstSpecies(vector<int> cpr, vector<int> cpd, vector<int> ccpd, vector<int> ecpd, 
        vector<int> o, int v, int s);

    FirstSpecies(FirstSpecies &s);

    virtual Space *copy(void);

    int* return_solution();
    void print_solution();
};

Base<FirstSpecies> *make_solver(FirstSpecies *pb);

FirstSpecies* get_next_solution(Search::Base<FirstSpecies>* solver);

#endif
