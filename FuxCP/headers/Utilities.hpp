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


using namespace std;
using namespace Gecode;
using namespace Gecode::Search;

class PartClass: public Space{
    protected:

        int species; //0 for cf, 1 for 1st, 2 for 2nd, 3 for 3rd, 4 for 4th, 5 for 5th
        IntVarArray solution_array;
        int solution_len;

        //voice variables
        vector<int> cp_range;
        vector<int> cp_domain;
        vector<int> chromatic_cp_domain;
        vector<int> extended_cp_domain;
        vector<int> off_domain;
        int voice_type;

        //1st species variables
        vector<IntVarArray> notes;
        vector<IntVarArray> h_intervals;
        vector<IntVarArray> m_intervals_brut;
        vector<int> m_intervals;
        vector<int> motions;
        vector<int> motions_cost;
        vector<bool> is_cf_lower_arr;
        vector<int> m2_intervals_brut;
        vector<int> m2_intervals;
        //some variables still not there, they'll come when we extend the code

        //3 voices variables
    
    public:
        PartClass(int cf_len);

        void init_m_intervals_brut(int cf_len);

        vector<IntVarArray> get_notes();

        virtual Space *copy(void);

        void set_notes(int n);
};

Base<PartClass> *make_solver(PartClass *pb);

PartClass* get_next_solution(Search::Base<PartClass>* solver);

string intVar_to_string(const IntVar &var, bool absolute);

string intVarArray_to_string(IntVarArray vars);

#endif
