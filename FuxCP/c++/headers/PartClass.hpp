#ifndef PARTCLASS
#define PARTCLASS

#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Gecode;

class PartClass: public Space{
    public:

        //CONSTANT VARIABLES
        //Major triad (0,4,7)
        const IntSet MAJ_H_TRIAD = IntSet({0,4,7});
        //H triad (0,4,7)
        const IntSet H_TRIAD = IntSet({0,4,7});
        //PERFECT CONSONANCES (0,7)
        const IntVarArgs P_CONS = IntVarArgs({IntVar(*this,0,0), IntVar(*this,7,7)});
        //IMPERFECT CONSONANCES (3,4,8,9)
        const IntSet IMP_CONS = IntSet({3,4,8,9});
        //ALL CONSONANCES (0,3,4,7,8,9)
        const IntSet ALL_CONS = IntSet({0,3,4,7,8,9});
        const IntVarArgs ALL_CONS_VAR = IntVarArgs({IntVar(*this, 0, 0), IntVar(*this, 3,3), IntVar(*this, 4,4), IntVar(*this, 7, 7),
        IntVar(*this, 8,8),IntVar(*this, 9,9)});

        IntVarArray harmonic_intervals;
        vector<int> vec_notes;

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
        IntVarArray h_intervals_brut;
        vector<int> m_intervals;
        vector<int> motions;
        vector<int> motions_cost;
        vector<BoolVarArray> is_cf_lower_arr;
        vector<int> m2_intervals_brut;
        vector<int> m2_intervals;
        //some variables still not there, they'll come when we extend the code

        //3 voices variables
    
    public:
        PartClass(int n_measures, int species, vector<int> cf_notes);

        void init_m_intervals_brut(int n_measures);

        vector<IntVarArray> get_notes();

        PartClass(PartClass &s);

        virtual Space *copy(void);

        void set_notes(int n);
};

#endif