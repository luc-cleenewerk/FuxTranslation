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

    FirstSpecies(int cf_len, PartClass *lowest, PartClass *cf, int n_counterpoints, vector<PartClass *> upper);

    void create_h_intervals(int cf_len, PartClass *lowest);

    void init_cfb(int cf_len, PartClass *cf);

    void create_m_intervals(int cf_lowest);

};

#endif
