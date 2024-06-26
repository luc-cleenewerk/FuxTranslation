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

#include "../headers/Utilities.hpp"

/** Types of search engines */
enum {
    dfs_solver, //0
    bab_solver, //1
};

class FirstSpecies: public PartClass {

protected:
    

public:

    FirstSpecies();

    FirstSpecies(int cf_len, PartClass *lowest);

    void create_h_intervals(int cf_len, PartClass *lowest);

};

#endif
