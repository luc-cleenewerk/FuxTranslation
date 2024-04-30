#include "headers/gecode_problem.hpp"
#include "headers/Utilities.hpp"
#include "headers/1sp_constraints.hpp"

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
Problem::Problem(int s, int l, int u, int sp, vector<int> cf, int pcost, int mtricost, vector<int> splist, int con, int obl, int dir,
    int var_cost){
    string message = "WSpace object created. ";
    size = s;
    lower_bound_domain = l;
    upper_bound_domain = u;
    species = sp;
    cantusFirmus = cf;
    costpcons = pcost;
    costtritone = mtricost;
    speciesList = splist;
    con_motion_cost = con;
    obl_motion_cost = obl;
    dir_motion_cost = dir;
    variety_cost = var_cost;

    //parts contains the cantusFirmus in the first position, the rest are the counterpoints
    parts.push_back(Part(*this, cf, s, l, u)); //putting the cantusFirmus in first position
    for(int i = 0; i < splist.size(); i++){
        parts.push_back(Part(*this, s,l,u,species,cantusFirmus,pcost,mtricost,splist,con,obl,dir,var_cost)); //adding the counterpoints
    }

    //lowest is the lowest stratum for each note
    
    lowest.push_back(Stratum(*this, size, lower_bound_domain, upper_bound_domain));
    lowest[0].notes = IntVarArray(*this, size, l, u);

    //upper contains the upper strata for each note

    for(int j = 0; j < parts.size()-1; j++){
        upper.push_back(Stratum(*this, size, lower_bound_domain, upper_bound_domain));
        upper[j].notes = IntVarArray(*this, size, l, u);
    }

    //creation of the strata and putting the correct notes in each strata

    for(int i = 0; i < size; i++){

        IntVarArray voices = IntVarArray(*this, parts.size(), 0, 120);
        IntVarArray temp_hInterval = IntVarArray(*this, size, 0, 11);

        for(int j = 0; j < parts.size(); j++){
            rel(*this, voices[j], IRT_EQ, parts[j].getNotes()[i]); //getting the notes
            rel(*this, temp_hInterval[j], IRT_EQ, parts[j].hIntervalsCpCf[i]);
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

        BoolVar temp = expr(*this, (parts[0].is_not_lowest[i]==0)&&(lowest[0].notes[i]!=parts[1].notes[i]));

        rel(*this, temp, IRT_EQ, 1, Reify(parts[1].is_not_lowest[i])); //else it is the cp1

        if(parts.size()==3){
            rel(*this, expr(*this, parts[1].is_not_lowest[i]!=parts[0].is_not_lowest[i]), IRT_EQ, parts[2].is_not_lowest[i]); //else it is the cp2 (in 3 voices)
        }

    }
    /// constraints

    link_harmonic_arrays_1st_species(*this, size, parts, lowest, upper);

    link_cfb_arrays_1st_species(*this, size, parts);

    link_melodic_arrays_1st_species(*this, size, parts);

    link_motions_arrays(*this, size, con_motion_cost, obl_motion_cost, dir_motion_cost, parts, lowest);
    
    harmonic_intervals_consonance(*this, parts);

    perfect_consonance_constraints(*this, size, parts, speciesList.size());

    imperfect_consonances_are_preferred(*this, size, parts, costpcons);

    key_tone_tuned_to_cantusfirmus(*this, size, parts);

    voices_cannot_play_same_note(*this, size, parts);

    penultimate_note_must_be_major_sixth_or_minor_third(*this, size, parts);

    no_tritonic_intervals(*this, size, costtritone, parts);

    melodic_intervals_not_exceed_minor_sixth(*this, size, parts);

    no_direct_perfect_consonance(*this, size, parts, speciesList.size());

    no_battuta(*this, size, parts);

    if(speciesList.size()==2){

        no_tenth_in_last_chord(*this, size, parts, upper, lowest);

        variety_cost_constraint(*this, size, parts);

        avoid_perfect_consonances(*this, size, parts);
    }

    no_same_direction(*this, size, parts, speciesList.size());

    no_successive_ascending_sixths(*this, size, parts, speciesList.size());
    //todo add other constraints
    

    /// branching
    //branch(*this, cp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    for(int i = 1; i < parts.size(); i++){
        branch(*this, parts[i].notes, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    }
    writeToLogFile(message.c_str()); /// to debug when using in OM, otherwise just print it's easier
}

/**
 * Copy constructor
 * @param s an instance of the Problem class
 * @todo modify this copy constructor to also copy any additional attributes you add to the class
 */
Problem::Problem(Problem& s): Space(s){
    //IntVars update
    size = s.size;
    lower_bound_domain = s.lower_bound_domain;
    upper_bound_domain = s.upper_bound_domain;
    species = s.species;
    cantusFirmus = s.cantusFirmus;
    costpcons = s.costpcons;
    costtritone = s.costtritone;
    speciesList = s.speciesList;
    con_motion_cost = s.con_motion_cost;
    obl_motion_cost = s.obl_motion_cost;
    dir_motion_cost = s.dir_motion_cost;
    variety_cost = s.variety_cost;
    parts = s.parts;
    lowest = s.lowest;
    upper = s.upper;
    sorted_voices = s.sorted_voices;

    parts[0].home = s.parts[0].home;
    parts[0].size = s.parts[0].size;
    parts[0].upper_bound = s.parts[0].upper_bound;
    parts[0].lower_bound = s.parts[0].lower_bound;

    parts[0].notes.update(*this, s.parts[0].notes);
    parts[0].m_intervals.update(*this, s.parts[0].m_intervals);
    parts[0].m_intervals_brut.update(*this, s.parts[0].m_intervals_brut);
    parts[0].is_not_lowest.update(*this, s.parts[0].is_not_lowest);
    parts[0].hIntervalsCpCf.update(*this, s.parts[0].hIntervalsCpCf);
    parts[0].succ_cost.update(*this, s.parts[0].succ_cost);

    for(int p = 1; p < parts.size(); p++){
        parts[p].home = s.parts[p].home;
        parts[p].size = s.parts[p].size;
        parts[p].lower_bound = s.parts[p].lower_bound;
        parts[p].upper_bound = s.parts[p].upper_bound;
        parts[p].species = s.parts[p].species;
        parts[p].cantusFirmus = s.parts[p].cantusFirmus;
        parts[p].costpcons = s.parts[p].costpcons;
        parts[p].costtritone = s.parts[p].costtritone;
        parts[p].speciesList = s.parts[p].speciesList;
        parts[p].con_motion_cost = s.parts[p].con_motion_cost;
        parts[p].obl_motion_cost = s.parts[p].obl_motion_cost;
        parts[p].dir_motion_cost = s.parts[p].dir_motion_cost;
        parts[p].variety_cost = s.parts[p].variety_cost;

        parts[p].notes.update(*this, s.parts[p].notes);
        parts[p].m_intervals.update(*this, s.parts[p].m_intervals);
        parts[p].m_intervals_brut.update(*this, s.parts[p].m_intervals_brut);
        parts[p].isCFB.update(*this, s.parts[p].isCFB);
        parts[p].is_not_lowest.update(*this, s.parts[p].is_not_lowest);
        parts[p].hIntervalsCpCf.update(*this, s.parts[p].hIntervalsCpCf);
        parts[p].P_cons_cost.update(*this, s.parts[p].P_cons_cost);
        parts[p].M_deg_cost.update(*this, s.parts[p].M_deg_cost);
        parts[p].varietyArray.update(*this, s.parts[p].varietyArray);
        parts[p].motions.update(*this, s.parts[p].motions);
        parts[p].direct_move_cost.update(*this, s.parts[p].direct_move_cost);
        parts[p].succ_cost.update(*this, s.parts[p].succ_cost);
    }
    for(int p = 0; p < sorted_voices.size(); p++){
        sorted_voices[p].update(*this, s.sorted_voices[p]);
    }

    lowest[0].home = s.lowest[0].home;
    lowest[0].size = s.lowest[0].size;
    lowest[0].lower_bound = s.lowest[0].lower_bound;
    lowest[0].upper_bound = s.lowest[0].upper_bound;
    lowest[0].hIntervalsBrut.update(*this, s.lowest[0].hIntervalsBrut);
    lowest[0].hIntervalsAbs.update(*this, s.lowest[0].hIntervalsAbs);
    lowest[0].notes.update(*this, s.lowest[0].notes);
    lowest[0].hIntervals.update(*this, s.lowest[0].hIntervals);
    lowest[0].m_intervals_brut.update(*this, s.lowest[0].m_intervals_brut);

    for(int p = 0; p < upper.size(); p++){
        upper[p].home = s.upper[p].home;
        upper[p].size = s.upper[p].size;
        upper[p].lower_bound = s.upper[p].lower_bound;
        upper[p].upper_bound = s.upper[p].upper_bound;

        upper[p].hIntervals.update(*this, s.upper[p].hIntervals);
        upper[p].notes.update(*this, s.upper[p].notes);
        upper[p].hIntervalsBrut.update(*this, s.upper[p].hIntervalsBrut);
        upper[p].hIntervalsAbs.update(*this, s.upper[p].hIntervalsAbs);
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
Space* Problem::copy(void) {
    return new Problem(*this);
}

/**
 * Constrain method for bab search
 * @todo modify this function if you want to use branch and bound
 * @param _b a solution to the problem from which we wish to add a constraint for the next solutions
 */
void Problem::constrain(const Space& _b) {
    const Problem &b = static_cast<const Problem &>(_b);
    
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
            to_string(lower_bound_domain) + "\n" + "upper bound for the domain : " + to_string(upper_bound_domain)
             + "\n";
    message += "Cantus firmus : " + int_vector_to_string(cantusFirmus) + "\n";
     message += "current values for P_cons_cost : [";
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            if (parts[p].P_cons_cost[i].assigned())
                message += to_string(parts[p].P_cons_cost[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
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
    message += "H intervals : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for hInterVal : [";
        for(int i = 0; i < size; i++){
            if (parts[k].hIntervalsCpCf[i].assigned())
                message += to_string(parts[k].hIntervalsCpCf[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "]\n\n";
    message += "M intervals brut : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for M intervals : [";
        for(int i = 0; i < size-1; i++){
            if (parts[k].m_intervals_brut[i].assigned())
                message += to_string(parts[k].m_intervals_brut[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
        message += "]\n";
    }
    message += "ISCFB : [";
    for(int k = 1; k < parts.size(); k++){
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
    for(int k = 1; k < parts.size(); k++){
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
            if (parts[p].M_deg_cost[i].assigned())
                message += to_string(parts[p].M_deg_cost[i].val()) + " ";
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
    message += "current values for succ cost : [";
    for(int p = 0; p < parts.size(); p++){
        for(int i = 0; i < size-2; i++){
            if (parts[p].succ_cost[i].assigned())
                message += to_string(parts[p].succ_cost[i].val()) + " ";
            else
                message += "<not assigned> ";
        }
    }
    message += "]\n";
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

string int_vector_to_string(vector<int> vector){
    string s;
    for (int i = 0; i < vector.size(); ++i) {
        s += to_string(vector[i]);
        if(i != vector.size() - 1)
            s += " , ";
    }
    return s;
}