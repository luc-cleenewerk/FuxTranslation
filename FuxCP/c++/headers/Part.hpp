#ifndef PART_hpp
#define PART_hpp

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

#include <set>

using namespace Gecode;
using namespace std;

class Part{
public:
    int size;
    int lower_bound;
    int upper_bound;
    int species;
    int con_motion_cost;
    int obl_motion_cost;
    int dir_motion_cost;
    int variety_cost;
    int voice_type;
    int tone_offset;
    int h_triad_cost;
    int step_cost;
    int third_cost;
    int fourth_cost;
    int tritone_cost;
    int fifth_cost;
    int sixth_cost;
    int seventh_cost;
    int octave_cost;
    int succ;
    int h_fifth;
    int h_octave;
    int direct_move;
    int off_cst;
    int m2_len;
    int sol_len;
    int total_m_len;

    Home home;

    vector<int> cantusFirmus;
    vector<int> speciesList;
    vector<int> scale;
    vector<int> borrowed_scale;
    vector<int> cp_range;
    vector<int> union_b_scale;
    vector<int> off_scale;

    IntVarArray notes; 
    vector<IntVarArray> vector_notes;
    vector<IntVarArray> hIntervalsCpCf;
    vector<BoolVarArray> isCFB;
    vector<IntVarArray> m_intervals;
    vector<IntVarArray> m_intervals_brut;
    vector<IntVarArray> m_succ_intervals;
    vector<IntVarArray> m_succ_intervals_brut;

    IntVarArray m2_intervals;
    IntVarArray m2_intervals_brut;
    IntVarArray m_all_intervals;
    IntVarArray m_all_intervals_brut;
    IntVarArray real_motions;
    IntVarArray real_motions_cost;
    BoolVarArray is_ta_dim;
    BoolVarArray is_neighbour;

    IntVarArray solution_array;

    IntVarArray P_cons_cost;
    IntVarArray M_deg_cost;
    vector<IntVarArray> motions;
    vector<IntVarArray> motions_cost;
    BoolVarArray is_P_cons;
    BoolVarArray is_not_lowest;
    IntVarArray hIntervalsBrut;
    IntVarArray hIntervalsAbs;
    IntVarArray varietyArray;
    IntVarArray direct_move_cost;
    IntVarArray succ_cost;
    IntVarArray triad_costs;
    vector<BoolVarArray> is_off;
    IntVarArray off_costs;
    IntVarArray m_degrees_cost;
    IntVarArray fifth_costs;
    IntVarArray octave_costs;

    Part(const Home &sp, const vector<int> cf_notes, int s, int succ_cst);

    Part(const Home &hme, int s, int sp, vector<int> cf, vector<int> speciesList, int con, int obl, 
        int dir, int voice_type, int t_off, vector<int> scle, vector<int> b_scale, int b_mode, int triad, vector<int> melodic
        , vector<int> general_params, vector<int> chrom);

    IntVarArray getNotes();

    void create_member_array(int idx);
};

#endif