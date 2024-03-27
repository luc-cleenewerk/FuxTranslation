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

#include "../../GiL_Library/c++/space_wrapper.cpp"

using namespace Gecode;
using namespace Gecode::Search;
using namespace std;

/** Types of search engines */
enum {
    dfs_solver, //0
    bab_solver, //1
};

class FirstSpecies: public Space {

protected:
    IntVarArray hintervals;
    int size;
    int lower_bound_domain;
    int upper_bound_domain;

public:
    FirstSpecies(int size, int l, int u, int species = 1);

    FirstSpecies(FirstSpecies &s);

    virtual Space *copy(void);
};

#endif