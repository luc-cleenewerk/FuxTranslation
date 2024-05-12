#include "headers/gecode_problem.hpp"
#include "headers/Utilities.hpp"
#include "headers/1sp_constraints.hpp"
#include "headers/2sp_constraints.hpp"

/***********************************************************************************************************************
 *                                          Problem class methods                                                      *
 ***********************************************************************************************************************/

/**
 * Constructor
 * @todo Modify this constructor depending on your problem. This constructor is where the problem is defined
 * @todo (variables, constraints, branching, ...)
 * @param size the size of the array of variables
 * @param l the lower bound of the domain of the variables
 * @param u the upper bound of the domain of the variables
 */
Problem::Problem(int s, int sp, vector<int> cf, vector<int> splist, vector<int> motion_params,
    vector<int> v_types, int t_off, vector<int> scle, vector<int> b_scale, int b_mode, vector<int> off, 
    unordered_map<string, int> pr, vector<int> melodic, vector<int> general_params){
    string message = "WSpace object created. ";
    size = s;
    n_unique_costs = 0;
    species = sp;
    cantusFirmus = cf;
    speciesList = splist;
    con_motion_cost = motion_params[2];
    obl_motion_cost = motion_params[1];
    dir_motion_cost = motion_params[0];
    voice_types = v_types;
    tone_offset = t_off;
    scale = scle;
    borrow_mode = b_mode;
    prefs = pr;
    cost_names = {"fifth", "octave", "borrow", "melodic", "motion", "variety", "succ", "triad"};
    ordered_factors = IntVarArray(*this, 14, 0, 100);
    for(int i = 0; i < 14; i++){
        vector<string> tmp = {};
        ordered_costs.push_back(tmp);
    }

    P_cons_cost = IntVarArray(*this, size, 0, 1000);
    vars = IntVarArray(*this, 14, 0, 100);
    int solution_len = 0;
    for(int p = 0; p < splist.size(); p++){
        solution_len+=splist[p]*size;
    }
    solution_array = IntVarArray(*this, solution_len, 0, 127);
    //initializing the cost_factors list
    int highest_species = 0;
    for(int s = 0; s < splist.size(); s++){
        if(splist[s]>highest_species){
            highest_species = s;
        }
    }
    if(s==1){ //if the cp is of species 1
        cost_factors.push_back(IntVarArray(*this, 8, 0, 100));
    } else {
        cost_factors.push_back(IntVarArray(*this, 8, 0, 100));
    }

    //parts contains the cantusFirmus in the first position, the rest are the counterpoints
    parts.push_back(Part(*this, cf, s, 0, 127, general_params[3])); //putting the cantusFirmus in first position
    for(int i = 0; i < splist.size(); i++){
        parts.push_back(Part(*this, s,0,127,splist[i],cantusFirmus,splist,con_motion_cost,obl_motion_cost,dir_motion_cost, voice_types[i], 
            tone_offset, scale, b_scale, b_mode, general_params[5], off, melodic, general_params)); //adding the counterpoints
    }

    //for fifth cost, octave cost, off key cost, Melodic cost, motion cost, variety cost, successive cost and triad costs
    IntVarArgs y(splist.size()*size);
    int index = 0;

    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            y[index] = parts[p].fifth_costs[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][0], IRT_EQ, expr(*this, sum(y)));

    IntVarArgs oc(splist.size()*size);
    index = 0;

    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            oc[index] = parts[p].octave_costs[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][1], IRT_EQ, expr(*this, sum(oc)));

    IntVarArgs z(splist.size()*size);
    index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            z[index] = parts[p].off_costs[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][2], IRT_EQ, expr(*this, sum(z)));

    IntVarArgs x((splist.size()*size)-splist.size());
    index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            x[index] = parts[p].m_degrees_cost[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][3], IRT_EQ, expr(*this, sum(x)));

    IntVarArgs pc((splist.size()*size)-splist.size());
    index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            pc[index] = parts[p].motions_cost[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][4], IRT_EQ, expr(*this, sum(pc)));

    IntVarArgs v((splist.size()*size)-splist.size());
    index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            v[index] = parts[p].varietyArray[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][5], IRT_EQ, expr(*this, sum(v)));

    IntVarArgs scc((splist.size()*size)-2*splist.size());
    index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-2; i++){
            scc[index] = parts[p].succ_cost[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][6], IRT_EQ, expr(*this, sum(scc)));

    //lowest is the lowest stratum for each note
    
    lowest.push_back(Stratum(*this, size, general_params[5]));
    lowest[0].notes = IntVarArray(*this, size);

    //upper contains the upper strata for each note

    for(int j = 0; j < parts.size()-1; j++){
        upper.push_back(Stratum(*this, size, general_params[5]));
        upper[j].notes = IntVarArray(*this, size);
    }

    //creation of the strata and putting the correct notes in each strata

    for(int i = 0; i < size; i++){

        IntVarArray voices = IntVarArray(*this, parts.size(), 0, 120);
        IntVarArray temp_hInterval = IntVarArray(*this, size, 0, 11);

        for(int j = 0; j < parts.size(); j++){
            if(j==0){
                rel(*this, voices[j], IRT_EQ, parts[j].getNotes()[i]); //getting the notes
            } else {
                rel(*this, voices[j], IRT_EQ, parts[j].vector_notes[0][i]); //getting the notes
            }
            rel(*this, temp_hInterval[j], IRT_EQ, parts[j].hIntervalsCpCf[0][i]);
        }
        
        IntVarArray order = IntVarArray(*this, parts.size(), 0, parts.size()-1);
        sorted_voices.push_back(IntVarArray(*this, parts.size(), 0, 120)); //sorting the voices
        
        rel(*this, lowest[0].hIntervals[i], IRT_EQ, temp_hInterval[i]); //linking the hInterval in the strata with the one in the cf / cp

        sorted(*this, voices, sorted_voices[i], order); //sorting the voices

        rel(*this, lowest[0].notes[i], IRT_EQ, sorted_voices[i][0]); //putting the lowest note in the lowest stratum

        for(int j = 0; j < upper.size(); j++){
            rel(*this, upper[j].notes[i], IRT_EQ, sorted_voices[i][j+1]); //the rest go in the upper strata
        }

        rel(*this, lowest[0].notes[i], IRT_NQ, parts[0].notes[i], Reify(parts[0].is_not_lowest[i])); //determmining if the cf is the lowest strata for each note

        if(parts.size()==2){
            rel(*this, parts[0].is_not_lowest[i], IRT_EQ, 0, Reify(parts[1].is_not_lowest[i]));
        }
        if(parts.size()==3){
            BoolVar temp = expr(*this, (parts[0].is_not_lowest[i]==0)&&(lowest[0].notes[i]!=parts[1].vector_notes[0][i]));

            rel(*this, temp, IRT_EQ, 1, Reify(parts[1].is_not_lowest[i])); //else it is the cp1
            rel(*this, expr(*this, parts[1].is_not_lowest[i]!=parts[0].is_not_lowest[i]), IRT_EQ, parts[2].is_not_lowest[i]); //else it is the cp2 (in 3 voices)
        }

        if(i > 0){

            vector<IntVarArray> corresponding_m_intervals;

            for(int j = 0; j < parts.size(); j++){
                corresponding_m_intervals.push_back(parts[j].m_intervals_brut);
            }

            rel(*this, corresponding_m_intervals[0][i-1], IRT_EQ, lowest[0].m_intervals_brut[i-1], Reify(parts[0].is_not_lowest[i]));
            rel(*this, corresponding_m_intervals[1][i-1], IRT_EQ, lowest[0].m_intervals_brut[i-1], Reify(parts[1].is_not_lowest[i]));
            if(parts.size()==3){
                rel(*this, corresponding_m_intervals[2][i-1], IRT_EQ, lowest[0].m_intervals_brut[i-1], Reify(parts[2].is_not_lowest[i]));
            }
        }

    }

    IntVarArgs tr(upper.size()*size);
    index = 0;
    for(int p = 0; p < upper.size(); p++){
        for(int i = 0; i < size; i++){
            tr[index] = upper[p].triad_costs[i];
            index++;
        }
    }
    rel(*this, cost_factors[0][7], IRT_EQ, expr(*this, sum(tr)));

    for(const auto& entry : prefs){
        int val = entry.second-1;
        ordered_costs[val].push_back(entry.first);
    }

    for(int i = 0; i < 14; i++){
        if(!ordered_costs[i].empty()){
            for(int k = 0; k < ordered_costs[i].size(); k++){
                IntVar to_add;
                for(int t = 0; t < cost_names.size(); t++){
                    if(cost_names[t]==ordered_costs[i][k]){
                        to_add = cost_factors[0][t];
                    }
                }
                ordered_factors[n_unique_costs] = to_add;
                n_unique_costs++;
            }
        }
    }
    /// constraints

    link_harmonic_arrays_1st_species(*this, size, parts, lowest, upper);

    link_cfb_arrays_1st_species(*this, size, parts);

    link_melodic_arrays_1st_species(*this, size, parts);

    link_motions_arrays(*this, size, con_motion_cost, obl_motion_cost, dir_motion_cost, parts, lowest);
    
    harmonic_intervals_consonance(*this, parts);

    perfect_consonance_constraints(*this, size, parts, speciesList.size());

    imperfect_consonances_are_preferred(*this, size, parts, P_cons_cost);

    key_tone_tuned_to_cantusfirmus(*this, size, parts);

    voices_cannot_play_same_note(*this, size, parts);

    penultimate_note_must_be_major_sixth_or_minor_third(*this, size, parts);

    no_tritonic_intervals(*this, size, parts);

    melodic_intervals_not_exceed_minor_sixth(*this, size, parts);

    no_direct_perfect_consonance(*this, size, parts, speciesList.size());

    no_battuta(*this, size, parts);

    set_off_costs(*this, size, parts);

    set_step_costs(*this, size, parts);

    if(speciesList.size()==2){

        no_tenth_in_last_chord(*this, size, parts, upper, lowest);

        variety_cost_constraint(*this, size, parts);

        avoid_perfect_consonances(*this, size, parts);

        prefer_harmonic_triads(*this, size, parts, lowest, upper);
    }

    no_same_direction(*this, size, parts, speciesList.size());

    no_successive_ascending_sixths(*this, size, parts, speciesList.size());

    //going through parts to check for second species
    for(int p = 1; p < parts.size(); p++){
        if(parts[p].species==2){ //if it is a second species -> do modifications from the first species

            parts[p].hIntervalsAbs = IntVarArray(*this, size, 0, 127);
            parts[p].hIntervalsBrut = IntVarArray(*this, size, -127, 127);

            link_harmonic_arrays_2nd_species(*this, size, parts[p], lowest);
        }
    }
    //todo add other constraints
    

    /// branching
    branch(*this, lowest[0].notes, INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    create_solution_array(size, solution_array, parts);
    branch(*this, solution_array, INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    writeToLogFile(message.c_str()); /// to debug when using in OM, otherwise just print it's easier
}

/**
 * Copy constructor
 * @param s an instance of the Problem class
 * @todo modify this copy constructor to also copy any additional attributes you add to the class
 */
Problem::Problem(Problem& s): IntLexMinimizeSpace(s){
    //IntVars update
    size = s.size;
    species = s.species;
    cantusFirmus = s.cantusFirmus;
    speciesList = s.speciesList;
    con_motion_cost = s.con_motion_cost;
    obl_motion_cost = s.obl_motion_cost;
    dir_motion_cost = s.dir_motion_cost;
    variety_cost = s.variety_cost;
    parts = s.parts;
    lowest = s.lowest;
    upper = s.upper;
    sorted_voices = s.sorted_voices;
    voice_types = s.voice_types;
    tone_offset = s.tone_offset;
    scale = s.scale;
    borrow_mode = s.borrow_mode;
    cost_factors = s.cost_factors;
    h_triad_cost = s.h_triad_cost;
    cost_names = s.cost_names;
    prefs = s.prefs;
    ordered_costs = s.ordered_costs;
    n_unique_costs = s.n_unique_costs;
    P_cons_cost.update(*this, s.P_cons_cost);
    vars.update(*this, s.vars);
    solution_array.update(*this, s.solution_array);
    ordered_factors.update(*this, s.ordered_factors);

    parts[0].home = s.parts[0].home;
    parts[0].size = s.parts[0].size;
    parts[0].upper_bound = s.parts[0].upper_bound;
    parts[0].lower_bound = s.parts[0].lower_bound;
    parts[0].hIntervalsCpCf = s.parts[0].hIntervalsCpCf;

    parts[0].notes.update(*this, s.parts[0].notes);
    parts[0].m_intervals.update(*this, s.parts[0].m_intervals);
    parts[0].m_intervals_brut.update(*this, s.parts[0].m_intervals_brut);
    parts[0].is_not_lowest.update(*this, s.parts[0].is_not_lowest);
    for(int h = 0; h < 4; h++){
        parts[0].hIntervalsCpCf[h].update(*this, s.parts[0].hIntervalsCpCf[h]);
    }
    parts[0].succ_cost.update(*this, s.parts[0].succ_cost);
    parts[0].motions.update(*this, s.parts[0].motions);
    parts[0].motions_cost.update(*this, s.parts[0].motions_cost);
    parts[0].isCFB.update(*this, s.parts[0].isCFB);

    for(int i = 0; i < cost_factors.size(); i++){
        cost_factors[i].update(*this, s.cost_factors[i]);
    }

    for(int p = 1; p < parts.size(); p++){
        parts[p].home = s.parts[p].home;
        parts[p].size = s.parts[p].size;
        parts[p].lower_bound = s.parts[p].lower_bound;
        parts[p].upper_bound = s.parts[p].upper_bound;
        parts[p].species = s.parts[p].species;
        parts[p].cantusFirmus = s.parts[p].cantusFirmus;
        parts[p].speciesList = s.parts[p].speciesList;
        parts[p].con_motion_cost = s.parts[p].con_motion_cost;
        parts[p].obl_motion_cost = s.parts[p].obl_motion_cost;
        parts[p].dir_motion_cost = s.parts[p].dir_motion_cost;
        parts[p].variety_cost = s.parts[p].variety_cost;
        parts[p].voice_type = s.parts[p].voice_type;
        parts[p].tone_offset = s.parts[p].tone_offset;
        parts[p].scale = s.parts[p].scale;
        parts[p].borrowed_scale = s.parts[p].borrowed_scale;
        parts[p].cp_range = s.parts[p].cp_range;
        parts[p].union_b_scale = s.parts[p].union_b_scale;
        parts[p].h_triad_cost = s.parts[p].h_triad_cost;
        parts[p].off_scale = s.parts[p].off_scale;
        parts[p].step_cost = s.parts[p].step_cost;
        parts[p].third_cost = s.parts[p].third_cost;
        parts[p].fourth_cost = s.parts[p].fourth_cost;
        parts[p].tritone_cost = s.parts[p].tritone_cost;
        parts[p].fifth_cost = s.parts[p].fifth_cost;
        parts[p].sixth_cost = s.parts[p].sixth_cost;
        parts[p].seventh_cost = s.parts[p].seventh_cost;
        parts[p].octave_cost = s.parts[p].octave_cost;
        parts[p].vector_notes = s.parts[p].vector_notes;
        parts[p].hIntervalsCpCf = s.parts[p].hIntervalsCpCf;
        parts[p].succ = s.parts[p].succ;
        parts[p].h_fifth = s.parts[p].h_fifth;
        parts[p].h_octave = s.parts[p].h_octave;
        parts[p].direct_move = s.parts[p].direct_move;
        parts[p].off_cst = s.parts[p].off_cst;

        parts[p].notes.update(*this, s.parts[p].notes);
        parts[p].m_intervals.update(*this, s.parts[p].m_intervals);
        parts[p].m_intervals_brut.update(*this, s.parts[p].m_intervals_brut);
        parts[p].isCFB.update(*this, s.parts[p].isCFB);
        parts[p].is_not_lowest.update(*this, s.parts[p].is_not_lowest);
        parts[p].hIntervalsAbs.update(*this, s.parts[p].hIntervalsAbs);
        parts[p].hIntervalsBrut.update(*this, s.parts[p].hIntervalsBrut);
        for(int h = 0; h < 4; h++){
            parts[p].hIntervalsCpCf[h].update(*this, s.parts[p].hIntervalsCpCf[h]);
        }
        parts[p].P_cons_cost.update(*this, s.parts[p].P_cons_cost);
        parts[p].M_deg_cost.update(*this, s.parts[p].M_deg_cost);
        parts[p].varietyArray.update(*this, s.parts[p].varietyArray);
        parts[p].motions.update(*this, s.parts[p].motions);
        parts[p].direct_move_cost.update(*this, s.parts[p].direct_move_cost);
        parts[p].succ_cost.update(*this, s.parts[p].succ_cost);
        parts[p].triad_costs.update(*this, s.parts[p].triad_costs);
        parts[p].is_off.update(*this, s.parts[p].is_off);
        parts[p].off_costs.update(*this, s.parts[p].off_costs);
        parts[p].m_degrees_cost.update(*this, s.parts[p].m_degrees_cost);
        parts[p].fifth_costs.update(*this, s.parts[p].fifth_costs);
        parts[p].octave_costs.update(*this, s.parts[p].octave_costs);
        for(int n = 0; n < 4; n++){
            parts[p].vector_notes[n].update(*this, s.parts[p].vector_notes[n]);
        }
    }
    for(int p = 0; p < sorted_voices.size(); p++){
        sorted_voices[p].update(*this, s.sorted_voices[p]);
    }

    lowest[0].home = s.lowest[0].home;
    lowest[0].size = s.lowest[0].size;
    lowest[0].h_triad_cost = s.lowest[0].h_triad_cost;
    lowest[0].hIntervalsBrut.update(*this, s.lowest[0].hIntervalsBrut);
    lowest[0].hIntervalsAbs.update(*this, s.lowest[0].hIntervalsAbs);
    lowest[0].notes.update(*this, s.lowest[0].notes);
    lowest[0].hIntervals.update(*this, s.lowest[0].hIntervals);
    lowest[0].m_intervals_brut.update(*this, s.lowest[0].m_intervals_brut);
    lowest[0].triad_costs.update(*this, s.lowest[0].triad_costs);

    for(int p = 0; p < upper.size(); p++){
        upper[p].home = s.upper[p].home;
        upper[p].size = s.upper[p].size;
        upper[p].h_triad_cost = s.upper[p].h_triad_cost;

        upper[p].hIntervals.update(*this, s.upper[p].hIntervals);
        upper[p].notes.update(*this, s.upper[p].notes);
        upper[p].hIntervalsBrut.update(*this, s.upper[p].hIntervalsBrut);
        upper[p].hIntervalsAbs.update(*this, s.upper[p].hIntervalsAbs);
        upper[p].triad_costs.update(*this, s.upper[p].triad_costs);
    }
}

/**
 * Returns the size of the problem
 * @return an integer representing the size of the vars array
 */
int Problem::getSize(){
    string message = "getSize function called. size = " + to_string(size) + "\n";
    writeToLogFile(message.c_str());
    return size;
}

/**
 * Returns the values taken by the variables cp in a solution
 * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
 * @return an array of integers representing the values of the variables in a solution
 * Should only be used when using OM
 */
int* Problem::return_solution(){
    string message = "return_solution method. Solution : [";
    int* solution = new int[size];
        for(int i = 0; i < size; i++){
            solution[i] = 1;
            message += to_string(solution[i]) + " ";
        }
    message += "]\n";
    writeToLogFile(message.c_str());
    return solution;
}

/**
 * Copy method
 * @return a copy of the current instance of the Problem class. Calls the copy constructor
 */
IntLexMinimizeSpace* Problem::copy(void) {
    return new Problem(*this);
}

/**
 * Constrain method for bab search
 * @todo modify this function if you want to use branch and bound
 * @param _b a solution to the problem from which we wish to add a constraint for the next solutions
 */
void Problem::constrain(const IntLexMinimizeSpace& _b) {
    const Problem &b = static_cast<const Problem &>(_b);
    IntVar current_sum = IntVar(*this, 0, 1000);
    max(*this, ordered_factors, current_sum);
}

IntVarArgs Problem::cost(void) const{
    IntVarArgs cost_var_args;
    for(const auto& var : cost_factors){
        cost_var_args << var;
    }
    return cost_var_args;
}

/**
 * toString method
 * @return a string representation of the current instance of the Problem class.
 * If a variable is not assigned when this function is called, it writes <not assigned> instead of the value
 * @todo modify this method to also print any additional attributes you add to the class
 */
string Problem::toString(){
    string message = "Problem object. \n";
    message += "size = " + to_string(size) + "\n" + "lower bound for the domain : " +
            to_string(0) + "\n" + "upper bound for the domain : " + to_string(127)
             + "\n";
    message += "Cantus firmus : " + int_vector_to_string(cantusFirmus) + "\n";
    message += "Parts : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for cp : [";
        for(int i = 0; i < size; i++){
            if (parts[k].notes[i].assigned())
                message += to_string(parts[k].notes[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "]\n";
    message += "H intervals : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for hInterVal : [";
        for(int i = 0; i < 4; i++){
            for(int h = 0; h < size; h++){
                if (parts[k].hIntervalsCpCf[i][h].assigned())
                    message += to_string(parts[k].hIntervalsCpCf[i][h].val()) + " ";
                else
                    message += "<not assigned> ";
            }
        }
        message += "]\n";
    }
    message += "]\n\n";  
    message += "H INTERVALS BRUT : [";
    for(int p = 1; p < parts.size(); p++){
        if(parts[p].species==2){
            message += " values for brut: ";
            for(int i = 0; i < size; i++){
                if(parts[p].hIntervalsBrut[i].assigned()){
                    message += to_string(parts[p].hIntervalsBrut[i].val()) + " ";
                }
            }
        }
    }
    message += "]\n";
    /*
    message += "M intervals brut : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for M intervals : [";
        for(int i = 0; i < size-1; i++){
            if (parts[k].m_intervals_brut[i].assigned())
                message += to_string(parts[k].m_intervals[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "ISCFB : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for CFB: [";
        for(int i = 0; i < size; i++){
            if (parts[k].isCFB[i].assigned())
                message += to_string(parts[k].isCFB[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "LOWEST : [";
    //for(int k = 1; k < parts.size(); k++){
        message += "current values for LOWEST: [";
        for(int i = 0; i < size; i++){
            if (lowest[0].notes[i].assigned())
                message += to_string(lowest[0].notes[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    //}
    message += "UPPER : [";
    for(int k = 0; k < upper.size(); k++){
        message += "current values for UPPER : [";
        for(int i = 0; i < upper[k].notes.size(); i++){
            if (upper[k].notes[i].assigned())
                message += to_string(upper[k].notes[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "IS NOT LOWEST : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for IS NOT LOWEST : [";
        for(int i = 0; i < size; i++){
            if (parts[k].is_not_lowest[i].assigned())
                message += to_string(parts[k].is_not_lowest[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "VAR COST : [";
    for(int k = 1; k < parts.size(); k++){
        message += "current values for VAR COST : [";
        for(int i = 0; i < parts[k].varietyArray.size(); i++){
            if (parts[k].varietyArray[i].assigned())
                message += to_string(parts[k].varietyArray[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "MOTIONS : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for MOTIONS : [";
        for(int i = 0; i < parts[k].motions.size(); i++){
            if (parts[k].motions[i].assigned())
                message += to_string(parts[k].motions[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "current values for M_deg_cost : [";
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            if (parts[p].m_degrees_cost[i].assigned())
                message += to_string(parts[p].m_degrees_cost[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
    message += "]\n";
    message += "current values for variety_cost : [";
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            if (parts[p].varietyArray[i].assigned())
                message += to_string(parts[p].varietyArray[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
    message += "]\n";
    message += "current values for direct cost : [";
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-2; i++){
            if (parts[p].direct_move_cost[i].assigned())
                message += to_string(parts[p].direct_move_cost[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
    message += "]\n";
    message += "current values for succ_cost : [";
    for(int p = 0; p < parts.size(); p++){
        for(int i = 0; i < size-2; i++){
            if (parts[p].succ_cost[i].assigned())
                message += to_string(parts[p].succ_cost[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
    message += "]\n";
    message += "current values for TRIAD : [";
    for(int p = 0; p < upper.size(); p++){
        for(int i = 0; i < size; i++){
            if (upper[p].triad_costs[i].assigned())
                message += to_string(upper[p].triad_costs[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
    message += "]\n";*/
     message += "current values for COST_FAC : [";
    for(int p = 0; p < cost_factors.size(); p++){
        for(int i = 0; i < cost_factors[p].size(); i++){
            if (cost_factors[p][i].assigned())
                message += to_string(cost_factors[p][i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
    message += "]\n";
     message += "COST NAMES : [";
        for(int i = 0; i < cost_names.size(); i++){
            message += cost_names[i] + " ";
        }
        message += "]\n";
    message += "SOLUTION ARRAY : [";
        for(int i = 0; i < solution_array.size(); i++){
            if(solution_array[i].assigned()){
                message += to_string(solution_array[i].val()) + " ";
            } else {
                message += "<not assigned> ";
            }
        }
        message += "]\n";
    /*message += "current values for P_CONS_CST: [";
        for(int i = 0; i < size; i++){
            if (P_cons_cost[i].assigned())
                message += to_string(P_cons_cost[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    
    message += "current values for LOWEST BRUT: [";
        for(int i = 0; i < size; i++){
            if (lowest[0].hIntervalsAbs[i].assigned())
                message += to_string(lowest[0].hIntervalsAbs[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
     message += "IS OFF : [";
    for(int k = 1; k < parts.size(); k++){
        message += "current values for IS OFF : [";
        for(int i = 0; i < size; i++){
            if (parts[k].is_off[i].assigned())
                message += to_string(parts[k].is_off[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "TRIAD COSTS : [";
    for(int k = 1; k < upper.size(); k++){
        message += "current values for IS OFF : [";
        for(int i = 0; i < size; i++){
            if (upper[k].triad_costs[i].assigned())
                message += to_string(upper[k].triad_costs[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "UPPER BRUT : [";
    for(int k = 0; k < upper.size(); k++){
        message += "current values for UPPER BRUT: [";
        for(int i = 0; i < upper[k].hIntervalsBrut.size(); i++){
            if (upper[k].hIntervalsBrut[i].assigned())
                message += to_string(upper[k].hIntervalsBrut[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "]\n\n";*/
    message += "ORDERED FACTORS : [";
        for(int i = 0; i < n_unique_costs; i++){
            if (ordered_factors[i].assigned())
                message += to_string(ordered_factors[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    message += "PART NOTES : [";
    for(int k = 1; k < parts.size(); k++){
        message += "current values for NOTES: [";
        for(int i = 0; i < parts[k].vector_notes.size(); i++){
            message += " [ ";
            for(int n = 0; n < parts[k].vector_notes[i].size(); n++){
                if (parts[k].vector_notes[i][n].assigned())
                    message += to_string(parts[k].vector_notes[i][n].val()) + " ";
                else
                    message += "<not assigned> ";     
            }
            message += " ] ";
        }
        message += "]\n";
    }
    message += "]\n\n";
    writeToLogFile(message.c_str());
    return message;
}

/*************************
 * Search engine methods *
 *************************/

/**
 * Creates a search engine for the given problem
 * Should only be used when using OM, otherwise you can create the solver etc in the main file
 * @todo Modify this function to add search options etc
 * @param pb an instance of the Problem class representing a given problem
 * @param type the type of search engine to create (see enumeration in headers/gecode_problem.hpp)
 * @return a search engine for the given problem
 */
Search::Base<Problem>* make_solver(Problem* pb, int type){
    string message = "make_solver function called. type of solver :\n" + to_string(type) + "\n";
    writeToLogFile(message.c_str());

    Gecode::Search::Options opts;
    /**@todo add here any options you want*/

    if (type == bab_solver)
        return new BAB<Problem>(pb, opts);
    else // default case
        return new DFS<Problem>(pb, opts);
}

/**
 * Returns the next solution space for the problem
 * Should only be used when using OM
 * @param solver a solver for the problem
 * @return an instance of the Problem class representing the next solution to the problem
 */
Problem* get_next_solution_space(Search::Base<Problem>* solver){
    string message = "get_next_solution_space function called.\n";
    Problem* sol_space = solver->next();
    if (sol_space == nullptr)
        return NULL;
    message += sol_space->toString();
    writeToLogFile(message.c_str());
    return sol_space;
}

/***********************
 * Auxiliary functions *
 ***********************/

/**
 * Write a text into a log file
 * @param message the text to write
 */
void writeToLogFile(const char* message){
    std::time_t currentTime = std::time(nullptr); // Get the current time
    std::string timeString = std::asctime(std::localtime(&currentTime)); // Convert to string

    const char* homeDir = std::getenv("HOME"); // Get the user's home directory
    if (homeDir) {
        std::string filePath(homeDir);
        filePath += "/log.txt"; // Specify the desired file path, such as $HOME/log.txt

        std::ofstream myfile(filePath, std::ios::app); // append mode
        if (myfile.is_open()) {
            myfile <<timeString<< endl << message << endl;
            myfile.close();
        }
    }
}
/*
string int_vector_to_string(vector<int> vector){
    string s;
    for (int i = 0; i < vector.size(); ++i) {
        s += to_string(vector[i]);
        if(i != vector.size() - 1)
            s += " , ";
    }
    return s;
}*/