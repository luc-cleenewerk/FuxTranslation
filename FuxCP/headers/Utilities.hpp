#ifndef UTILITIES
#define UTILITIES

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "../../GiL_Library/c++/gecode_wrapper.cpp"

using namespace std;
using namespace Gecode;

int *PENULT_CONS_VAR = new int[0]; //check this again
int *P_CONS = new int[0,7];
int *IMP_CONS = new int[3,4,8,9];
int *ALL_CONS = new int[0,3,4,7,8,9];
bool PENULT_RULE_CHECK = true;

void* add_h_cons_cst(void* sp, int len, int penult_index, IntVarArray h_intervals, int *penult_dom_var = PENULT_CONS_VAR);

void* add_penult_dom_cst(void* sp, int h_interval, int *penult_dom_var);

void* create_h_intervals(void* sp, IntVarArray cp, IntVarArray cf, IntVarArray h_intervals);

#endif