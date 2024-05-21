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
Problem::Problem(vector<int> cf, int s, int n_cp, vector<int> splist, vector<int> v_types, int b_mode, int min_skips, vector<int> general_params, 
        vector<int> motion_params, vector<int> melodic, vector<int> specific, vector<int> importance, int t_off, vector<int> scle, int scale_size, 
        vector<int> chromatic, int chrom_size, vector<int> borrow, int borrow_size){
    string message = "WSpace object created. ";
    message += "\n";
    message += int_vector_to_string(cf);
    message += "\n";
    message += to_string(s);
    message += "\n";
    message += to_string(n_cp);
    message += "\n";
    message += int_vector_to_string(splist);
    message += "\n";
    message += int_vector_to_string(v_types);
    message += "\n";
    message += to_string(b_mode);
    message += "\n";
    message += to_string(min_skips);
    message += "\n";
    message += int_vector_to_string(general_params);
    message += "\n";
    message += int_vector_to_string(motion_params);
    message += "\n";
    message += int_vector_to_string(melodic);
    message += "\n";
    message += int_vector_to_string(specific);
    message += "\n";
    message += int_vector_to_string(importance);
    message += "\n";
    message += to_string(t_off);
    message += "\n";
    message += int_vector_to_string(scle);



    size = s;                               //amount of measures
    n_unique_costs = 0;                     //amount of unique costs with the species
    lower_bound_domain = 1;                 //lower bound for the strata
    upper_bound_domain = 127;               //upper bound for the strata
    cantusFirmus = cf;                      //the cantusfirmus
    speciesList = splist;                   //list of species for each counterpoint
    con_motion_cost = motion_params[2];     //cost of contrary motion
    obl_motion_cost = motion_params[1];     //cost of oblique motion
    dir_motion_cost = motion_params[0];     //cost of direct motion
    voice_types = v_types;                  //voice type, by far above / below the cantusfirmus a counterpoint should be
    tone_offset = t_off;                    //tonality offset
    scale = scle;                           //scale
    borrow_mode = b_mode;                   //is borrowing allowed

    //creating the map with the names of the costs and their importance

    vector<string> importance_names = {"borrow", "fifth", "octave", "succ", "variety", "triad", "motion", "melodic"};
    prefs = {{importance_names[0], importance[0]},{importance_names[1], importance[1]},{importance_names[2], importance[2]},{importance_names[3], importance[3]}
        ,{importance_names[4], importance[4]},{importance_names[5], importance[5]},{importance_names[6], importance[7]},{importance_names[7], importance[13]}};
    
    //the cost names in order of how they are added later to the cost factors list (order of the costs is very important)

    cost_names = {"fifth", "octave", "borrow", "melodic", "motion", "variety", "succ", "triad"};

    //initializing the ordered costs list, aka the list containing the costs according to their importance

    ordered_factors = IntVarArray(*this, 14, 0, 1000);
    for(int i = 0; i < 14; i++){
        vector<string> tmp = {};
        ordered_costs.push_back(tmp);
    }

    P_cons_cost = IntVarArray(*this, size, 0, 1000);

    //initializing the cost_factors list

    int highest_species = 0;
    for(int s = 0; s < splist.size(); s++){
        if(splist[s]>highest_species){
            highest_species = s;
        }
    }
    if(speciesList.size()>1){
        if(highest_species==1){ //if the cp is of species 1
        cost_factors.push_back(IntVarArray(*this, 8, 0, 1000));
        } else {
            cost_factors.push_back(IntVarArray(*this, 8, 0, 1000)); //TODO : this should be 9 instead of 8 later on
        }
    }

    //parts contains the cantusFirmus in the first position, the rest are the counterpoints
    parts.push_back(Part(*this, cf, s, general_params[3])); //putting the cantusFirmus in first position
    for(int i = 0; i < splist.size(); i++){
        parts.push_back(Part(*this, s,splist[i],cantusFirmus,splist,con_motion_cost,obl_motion_cost,dir_motion_cost, voice_types[i], 
            tone_offset, scale, borrow, b_mode, general_params[5], melodic, general_params, chromatic)); //adding the counterpoints
    }

    //lowest is the lowest stratum for each note
    
    lowest.push_back(Stratum(*this, size, lower_bound_domain, upper_bound_domain, general_params[5]));
    lowest[0].notes = IntVarArray(*this, size, lower_bound_domain, upper_bound_domain);

    //upper contains the upper strata for each note

    for(int j = 0; j < parts.size()-1; j++){
        upper.push_back(Stratum(*this, size, lower_bound_domain, upper_bound_domain, general_params[5]));
        upper[j].notes = IntVarArray(*this, size, lower_bound_domain, upper_bound_domain);
    }

    //creation of the strata and putting the correct notes in each strata

    create_strata();

    if(speciesList.size()==2){
        apply_3v_general(*this, size, parts, lowest, upper);
    }

    if(speciesList.size()==1){
        if(speciesList[0]==1){
            first_species(*this, parts, lowest, upper, 1); //dispatch 2 voices 1st species
        }
    } else if(speciesList.size()==2){
        for(int i = 0; i < speciesList.size(); i++){
            if(speciesList[i]==1){
                first_species(*this, parts, lowest, upper, 6); //dispatch 3 voices 1st species
            }
        }
    }

    //adding the costs to the list

    if(speciesList.size()>1){
        add_fifth_cost(*this, cost_factors[0][0], size, splist, parts);

        add_octave_cost(*this, cost_factors[0][1], size, splist, parts);

        add_off_cost(*this, cost_factors[0][2], size, splist, parts);

        add_melodic_cost(*this, cost_factors[0][3], size, splist, parts);

        add_motion_cost(*this, cost_factors[0][4], size, splist, parts);

        add_variety_cost(*this, cost_factors[0][5], size, splist, parts);

        add_succ_cost(*this, cost_factors[0][6], size, splist, parts);

        add_triad_cost(*this, cost_factors[0][7], size, splist, upper);

        //ORDERING THE COSTS

        //putting the name of the cost in the ordered costs list at the index of its importance
        for(const auto& entry : prefs){ 
            int val = entry.second-1;
            ordered_costs[val].push_back(entry.first);
        }

        //creating the final ordered list
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
    }

    /// constraints

    //going through parts to check for second species
    int solution_len = 0;
    for(int p = 1; p < parts.size(); p++){
        if(parts[p].species==1){
            parts[p].sol_len = size;
            solution_len+=size;
            parts[p].solution_array = IntVarArray(*this, parts[p].sol_len, 0, 127);
            for(int n = 0; n < size; n++){
                parts[p].solution_array[n] = parts[p].vector_notes[0][n];
            }
        }
        if(parts[p].species==2){ //if it is a second species -> do modifications from the first species

            parts[p].sol_len = size + (size-1);
            parts[p].solution_array = IntVarArray(*this, parts[p].sol_len, 0, 127);
            int idx = 0;
            for(int n = 0; n < parts[p].sol_len; n+=2){
                parts[p].solution_array[n] = parts[p].vector_notes[0][idx];
                idx++;
            }
            idx=0;
            for(int n = 1; n < parts[p].sol_len; n+=2){
                parts[p].solution_array[n] = parts[p].vector_notes[2][idx];
                idx++;
            }

            solution_len+=parts[p].sol_len;

            if(speciesList.size()==1){
                first_species(*this, parts, lowest, upper, 2);
            } else if(speciesList.size()==2){
                first_species(*this, parts, lowest, upper, 7);
            }

            parts[p].hIntervalsAbs = IntVarArray(*this, size, 0, 127);
            parts[p].hIntervalsBrut = IntVarArray(*this, size, -127, 127);

            link_harmonic_arrays_2nd_species(*this, size, parts[p], lowest);

            link_melodic_arrays_2nd_species_next_meas(*this, size, parts[p]);

            parts[p].m_succ_intervals.push_back(IntVarArray(*this, size-1, 0, 12));
            parts[p].m_succ_intervals_brut.push_back(IntVarArray(*this, size-1, -12, 12));

            link_melodic_arrays_2nd_species_in_meas(*this, size, parts[p]);

            parts[p].m2_len = 2*(size-1)-1;
            parts[p].m2_intervals = IntVarArray(*this, parts[p].m2_len, 0, 12);
            parts[p].m2_intervals_brut = IntVarArray(*this, parts[p].m2_len, -12, 12);

            link_m2_arrays_2nd_species(*this, parts[p]);

            parts[p].total_m_len = 2*(size-1);
            parts[p].m_all_intervals = IntVarArray(*this, parts[p].total_m_len, 0, 12);
            parts[p].m_all_intervals_brut = IntVarArray(*this, parts[p].total_m_len, -12, 12);

            link_melodic_self_arrays_2nd_species(*this, parts[p]);

            parts[p].real_motions = IntVarArray(*this, size-1, -1, 2);
            parts[p].real_motions_cost = IntVarArray(*this, size-1, IntSet({0, parts[p].con_motion_cost, parts[p].dir_motion_cost, parts[p].obl_motion_cost}));

            link_motions_arrays_2nd_species(*this, parts[p], parts[0], lowest);
            link_real_motions_arrays_2nd_species(*this, parts[p]);

            parts[p].is_ta_dim = BoolVarArray(*this, size-1, 0, 1);

            link_ta_dim_array_2nd_species(*this, parts[p]);

            link_cfb_array_2nd_species(*this, parts[p].size-1, parts[p], parts[0]);

            parts[p].is_neighbour = BoolVarArray(*this, parts[p].size-1, 0, 1);

            link_is_neighbour_array_2nd_species(*this, parts[p], lowest);

            h_cons_arsis(*this, parts[p], PENULT_CONS);

            penult_cons(*this, parts[p], PENULT_CONS_3P, IntVar(*this, 9,9), IntVar(*this,3,3));

            //melodic_inter_arsis(*this, parts[p]);

            no_chromatic_motion(*this, parts[p]);

            //no_unison_at_all(*this, parts[p], 7);


        }
    }

    //creating the solution array which will contain the notes of the counterpoint

    solution_array = IntVarArray(*this, solution_len, 0, 127);
    
    create_solution_array(solution_array, parts);
    /// branching
    branch(*this, lowest[0].notes, INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
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
    lower_bound_domain = s.lower_bound_domain;
    upper_bound_domain = s.upper_bound_domain;
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
    parts[0].m_intervals = s.parts[0].m_intervals;
    parts[0].motions = s.parts[0].motions;
    parts[0].motions_cost = s.parts[0].motions_cost;
    parts[0].isCFB = s.parts[0].isCFB;

    parts[0].notes.update(*this, s.parts[0].notes);
    parts[0].is_not_lowest.update(*this, s.parts[0].is_not_lowest);
    for(int h = 0; h < 4; h++){
        parts[0].hIntervalsCpCf[h].update(*this, s.parts[0].hIntervalsCpCf[h]);
        parts[0].m_intervals[h].update(*this, s.parts[0].m_intervals[h]);
        parts[0].m_intervals_brut[h].update(*this, s.parts[0].m_intervals_brut[h]);
        parts[0].motions[h].update(*this, s.parts[0].motions[h]);
        parts[0].motions_cost[h].update(*this, s.parts[0].motions_cost[h]);
        parts[0].isCFB[h].update(*this, s.parts[0].isCFB[h]);
    }
    parts[0].succ_cost.update(*this, s.parts[0].succ_cost);

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
        parts[p].m_intervals = s.parts[p].m_intervals;
        parts[p].m_intervals_brut = s.parts[p].m_intervals_brut;
        parts[p].sol_len = s.parts[p].sol_len;
        parts[p].motions = s.parts[p].motions;
        parts[p].motions_cost = s.parts[p].motions_cost;
        parts[p].isCFB = s.parts[p].isCFB;
        parts[p].is_off = s.parts[p].is_off;
        parts[p].penult_rule_check = s.parts[p].penult_rule_check;
        //2nd species variables
        parts[p].m_succ_intervals = s.parts[p].m_succ_intervals;
        parts[p].m_succ_intervals_brut = s.parts[p].m_succ_intervals_brut;
        parts[p].m2_len = s.parts[p].m2_len;
        parts[p].total_m_len = s.parts[p].total_m_len;
        
        parts[p].m2_intervals.update(*this, s.parts[p].m2_intervals);
        parts[p].m2_intervals_brut.update(*this, s.parts[p].m2_intervals_brut);
        parts[p].notes.update(*this, s.parts[p].notes);
        parts[p].is_not_lowest.update(*this, s.parts[p].is_not_lowest);
        parts[p].hIntervalsAbs.update(*this, s.parts[p].hIntervalsAbs);
        parts[p].hIntervalsBrut.update(*this, s.parts[p].hIntervalsBrut);
        parts[p].solution_array.update(*this, s.parts[p].solution_array);
        parts[p].m_all_intervals.update(*this, s.parts[p].m_all_intervals);
        parts[p].m_all_intervals_brut.update(*this, s.parts[p].m_all_intervals_brut);
        parts[p].real_motions.update(*this, s.parts[p].real_motions);
        parts[p].real_motions_cost.update(*this, s.parts[p].real_motions_cost);
        parts[p].is_ta_dim.update(*this, s.parts[p].is_ta_dim);
        parts[p].is_neighbour.update(*this, s.parts[p].is_neighbour);
        for(int h = 0; h < 4; h++){
            parts[p].hIntervalsCpCf[h].update(*this, s.parts[p].hIntervalsCpCf[h]);
            parts[p].m_intervals[h].update(*this, s.parts[p].m_intervals[h]);
            parts[p].m_intervals_brut[h].update(*this, s.parts[p].m_intervals_brut[h]);
            parts[p].motions[h].update(*this, s.parts[p].motions[h]);
            parts[p].motions_cost[h].update(*this, s.parts[p].motions_cost[h]);
            parts[p].isCFB[h].update(*this, s.parts[p].isCFB[h]);
            parts[p].is_off[h].update(*this, s.parts[p].is_off[h]);
        }
        for(int h = 0; h < parts[p].m_succ_intervals.size(); h++){
            parts[p].m_succ_intervals[h].update(*this, s.parts[p].m_succ_intervals[h]);
            parts[p].m_succ_intervals_brut[h].update(*this, s.parts[p].m_succ_intervals_brut[h]);
        }
        parts[p].P_cons_cost.update(*this, s.parts[p].P_cons_cost);
        parts[p].M_deg_cost.update(*this, s.parts[p].M_deg_cost);
        parts[p].varietyArray.update(*this, s.parts[p].varietyArray);
        parts[p].direct_move_cost.update(*this, s.parts[p].direct_move_cost);
        parts[p].succ_cost.update(*this, s.parts[p].succ_cost);
        parts[p].triad_costs.update(*this, s.parts[p].triad_costs);
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
    lowest[0].lower_bound = s.lowest[0].lower_bound;
    lowest[0].upper_bound = s.lowest[0].upper_bound;
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
        upper[p].lower_bound = s.upper[p].lower_bound;
        upper[p].upper_bound = s.upper[p].upper_bound;
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
 * @return an integer representing the size of the vars array           RETURNS THE SIZE OF THE SOLUTION_ARRAY
 */
int Problem::getSize(){
    string message = "getSize function called. size = " + to_string(solution_array.size()) + "\n";
    writeToLogFile(message.c_str());
    return solution_array.size();
}

/**
 * Returns the values taken by the variables vars in a solution
 * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
 * @return an array of integers representing the values of the variables in a solution
 */
int* Problem::return_solution(){
    string message = "return_solution method. Solution : [";
    int* solution = new int[solution_array.size()];
    for(int i = 0; i < solution_array.size(); i++){
        solution[i] = solution_array[i].val();       // TODO : modify!!
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
    message += "size = " + to_string(size) + "\n";
    message += "Cantus firmus : " + int_vector_to_string(cantusFirmus) + "\n";
    
    for(int p = 0; p < parts.size(); p++){
        if(parts[p].species==2){
            message += "IS NEIGHBOUR : [";
            for(int n = 0; n < parts[p].is_neighbour.size(); n++){
                if(parts[p].is_neighbour.assigned()){
                    message += to_string(parts[p].is_neighbour[n].val()) + " ";
                } else {
                    message += " NA ";
                }
            }
            message += "]\n";
        }
    }
    message += "PART NOTES : ";
    for(int p = 1; p < parts.size(); p++){
        message += "[ ";
        for(int i = 0; i < 4; i++){
            message += "[ ";
            for(int n = 0; n < parts[p].vector_notes[i].size(); n++){
                if(parts[p].vector_notes[i][n].assigned()){
                    message += to_string(parts[p].vector_notes[i][n].val()) + " ";
                } else {
                    message += "...";
                }
            }
            message += "]";
        }
        message += "]\n";
    }
    message += "SOLUTION ARRAY : [";
    for(int i = 0; i < solution_array.size(); i++){
        if(solution_array[i].assigned()){
            message += to_string(solution_array[i].val()) + " ";
        }
    }
    message += "]\n";
    message += "COST NAMES : [";
    for(int i = 0; i < cost_names.size(); i++){
            message += cost_names[i] + " ";
    }
    message += "]\n";
    message += "COST FACTORS : [";
    for(int i = 0; i < cost_factors[0].size(); i++){
        if(cost_factors[0][i].assigned()){
            message += to_string(cost_factors[0][i].val()) + " ";
        }
    }
    message += "]\n";
    for(int n = 0; n < parts[2].is_not_lowest.size(); n++){
        if(parts[2].is_not_lowest[n].assigned()){
            message += to_string(parts[2].is_not_lowest[n].val()) + " ";
        } else {
            message += "... ";
        }
    }
    message += "\n";
    message += "MOTIONS COST : [";
    for(int i = 0; i < 4; i++){
        message += "[ ";
        for(int n = 0; n < parts[2].motions_cost[i].size(); n++){
            if(parts[2].motions_cost[i][n].assigned()){
                message += to_string(parts[2].motions_cost[i][n].val()) + " ";
            } else {
                message += "... ";
            }
        }
        message += "]";
    }
    message += "]\n";
    
    writeToLogFile(message.c_str());
    return message;
}

/*************************
 * Search engine methods *
 *************************/

Gecode::Search::TimeStop global_timeout(5000);

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
    // Gecode::Search::TimeStop timeout(5000); // Stops search after 5 seconds, like Anton. Do we have to reset it after calling next solution?
    opts.stop = &global_timeout;

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
    // RESET TIMEOUT OBJECT HERE
    global_timeout.reset();
    Problem* sol_space = solver->next();
    if (sol_space == nullptr){
        message += "solution_space was null. \n";
        writeToLogFile(message.c_str());
        return NULL;
    }
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
void Problem::create_strata(){
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
                if(parts[j].species==0){
                    corresponding_m_intervals.push_back(parts[j].m_intervals_brut[0]);
                } else if(parts[j].species==1){
                    corresponding_m_intervals.push_back(parts[j].m_intervals_brut[0]);
                } else if(parts[j].species==2){
                    corresponding_m_intervals.push_back(parts[j].m_intervals_brut[2]);
                } else if(parts[j].species==3){
                    corresponding_m_intervals.push_back(parts[j].m_intervals_brut[3]);
                } else if(parts[j].species==4){
                    corresponding_m_intervals.push_back(parts[j].m_intervals_brut[2]);
                } else{
                    corresponding_m_intervals.push_back(parts[j].m_intervals_brut[2]);
                }
            }

            rel(*this, corresponding_m_intervals[0][i-1], IRT_EQ, lowest[0].m_intervals_brut[i-1], Reify(parts[0].is_not_lowest[i]));
            rel(*this, corresponding_m_intervals[1][i-1], IRT_EQ, lowest[0].m_intervals_brut[i-1], Reify(parts[1].is_not_lowest[i]));
            if(parts.size()==3){
                rel(*this, corresponding_m_intervals[2][i-1], IRT_EQ, lowest[0].m_intervals_brut[i-1], Reify(parts[2].is_not_lowest[i]));
            }
        }

    }
}