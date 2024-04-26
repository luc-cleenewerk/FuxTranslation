#include "headers/gecode_problem.hpp"
#include "headers/Utilities.hpp"

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
Problem::Problem(int s, int l, int u, int sp, vector<int> cf, int pcost, int mtricost, vector<int> splist, int con, int obl, int dir) {
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


    /// variable initialization todo depends on the species
    cp = IntVarArray(*this, size, l, u);
    for(int i = 0; i < splist.size(); i++){
        parts.push_back(IntVarArray(*this, size, l, u));
    }
    hIntervalsCpCf = IntVarArray(*this, size, 0, 11);
    isCFB = BoolVarArray(*this, size, 0, 1);
    m_intervals = IntVarArray(*this, size-1, 0, 12);
    m_intervals_brut = IntVarArray(*this, size-1, -12, 12);
    cf_m_intervals_brut = IntVarArray(*this, size-1, -12, 12);
    P_cons_cost = IntVarArray(*this, size, 0, 64);
    M_deg_cost = IntVarArray(*this, size-1, 0, 64);
    motions = IntVarArray(*this, size-1, -1, 2);
    motions_cost = IntVarArray(*this, size-1, IntSet({0, con_motion_cost, obl_motion_cost, dir_motion_cost}));
    is_P_cons = BoolVarArray(*this, size, 0, 1);

    /// constraints

    link_harmonic_arrays_1st_species(*this, size, cp, hIntervalsCpCf, cantusFirmus);

    link_cfb_arrays_1st_species(*this, size, cp, cantusFirmus, isCFB);

    link_melodic_arrays_1st_species(*this, size, cp, m_intervals, m_intervals_brut, cantusFirmus, cf_m_intervals_brut);

    link_P_cons_arrays(*this, size, hIntervalsCpCf, is_P_cons);

    link_motions_arrays(*this, size, m_intervals_brut, cf_m_intervals_brut, motions, motions_cost, isCFB,
        con_motion_cost, obl_motion_cost, dir_motion_cost);

    /// harmonic intervals must be consonnances (define the consonnances in Utilities.hpp so its easier to reuse)
    
    dom(*this, hIntervalsCpCf, consonances);

    perfect_consonance_constraints(*this, size, hIntervalsCpCf);

    imperfect_consonances_are_preferred(*this, size, hIntervalsCpCf, P_cons_cost, costpcons);

    key_tone_tuned_to_cantusfirmus(*this, size, isCFB, hIntervalsCpCf);

    voices_cannot_play_same_note(*this, size, cp, cantusFirmus);

    penultimate_note_must_be_major_sixth_or_minor_third(*this, size, hIntervalsCpCf, isCFB);

    no_tritonic_intervals(*this, size, m_intervals, costtritone, M_deg_cost);

    melodic_intervals_not_exceed_minor_sixth(*this, size, m_intervals);

    no_direct_perfect_consonance(*this, size, hIntervalsCpCf, motions);
    //todo add other constraints
    

    /// branching
    branch(*this, cp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
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
    parts = s.parts;
    con_motion_cost = s.con_motion_cost;
    obl_motion_cost = s.obl_motion_cost;
    dir_motion_cost = s.dir_motion_cost;

    cp.update(*this, s.cp);
    for(int p = 0; p < parts.size(); p++){
        parts[p].update(*this, s.parts[p]);
    }
    hIntervalsCpCf.update(*this, s.hIntervalsCpCf);
    isCFB.update(*this, s.isCFB);
    m_intervals.update(*this, s.m_intervals);
    m_intervals_brut.update(*this, s.m_intervals_brut);
    cf_m_intervals_brut.update(*this, s.cf_m_intervals_brut);
    P_cons_cost.update(*this, s.P_cons_cost);
    M_deg_cost.update(*this, s.M_deg_cost);
    motions.update(*this, s.motions);
    motions_cost.update(*this, s.motions_cost);
    is_P_cons.update(*this, s.is_P_cons);
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
        solution[i] = cp[i].val();
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
    rel(*this, cp, IRT_GQ, 2);
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
    message += "current values for cp : [";
    for(int i = 0; i < size; i++){
        if (cp[i].assigned())
            message += to_string(cp[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
    message += "current values for hIntervalsCpCf : [";
    for(int i = 0; i < size; i++){
        if (hIntervalsCpCf[i].assigned())
            message += to_string(hIntervalsCpCf[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
    message += "current values for isCFB : [";
    for(int i = 0; i < size; i++){
        if (isCFB[i].assigned())
            message += to_string(isCFB[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
     message += "current values for mIntervals : [";
    for(int i = 0; i < size-1; i++){
        if (m_intervals[i].assigned())
            message += to_string(m_intervals[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
     message += "current values for mIntervalsBrut : [";
    for(int i = 0; i < size-1; i++){
        if (m_intervals_brut[i].assigned())
            message += to_string(m_intervals_brut[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
     message += "current values for P_cons_cost : [";
    for(int i = 0; i < size; i++){
        if (P_cons_cost[i].assigned())
            message += to_string(P_cons_cost[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
     message += "current values for M_deg_cost : [";
    for(int i = 0; i < size-1; i++){
        if (M_deg_cost[i].assigned())
            message += to_string(M_deg_cost[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
    message += "current values for is_P_CONS : [";
    for(int i = 0; i < size; i++){
        if (is_P_cons[i].assigned())
            message += to_string(is_P_cons[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
    message += "current values for motions : [";
    for(int i = 0; i < size-1; i++){
        if (motions[i].assigned())
            message += to_string(motions[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n";
    message += "Parts : [";
    for(int k = 0; k < parts.size(); k++){
        message += "current values for cp : [";
        for(int i = 0; i < size; i++){
            if (parts[k][i].assigned())
                message += to_string(parts[k][i].val()) + " ";
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
 *      Constraints      *
 *************************/

 // todo move this into appropriate file (should be organised) for species and type of constraint (harmonic, melodic, motion, linking arrays,...)

 /**
  * Link the harmonic intervals arrays for the first species
  * @param home the problem
  * @param size the number of notes in the cf
  * @param cp the variable array for the counterpoint to be generated
  * @param hIntervalsCpCf the variable array for the harmonic intervals between the cp and the cf
  * @param cantusFirmus the cantus firmus
  */
void link_harmonic_arrays_1st_species(const Home &home, int size, IntVarArray cp, IntVarArray hIntervalsCpCf, vector<int> cantusFirmus){
    //works
    for(int i = 0; i < size; i++){
        rel(home, hIntervalsCpCf[i] == abs(cp[i] - cantusFirmus[i]));
    }
}

void link_cfb_arrays_1st_species(const Home &home, int size, IntVarArray cp, vector<int> cantusFirmus, BoolVarArray isCFB){
    //works
    for(int i = 0; i < size; i++){
        rel(home, cp[i], IRT_GQ, cantusFirmus[i], Reify(isCFB[i], RM_EQV));
    }
}

void link_melodic_arrays_1st_species(const Home &home, int size, IntVarArray cp, IntVarArray m_intervals, IntVarArray m_intervals_brut, 
    vector<int> cantusFirmus, IntVarArray cf_m_intervals_brut){
    //works
    for(int i = 0; i < size-1; i++){
        rel(home, expr(home, cp[i]-cp[i+1]), IRT_EQ, m_intervals_brut[i]);
        rel(home, expr(home, cantusFirmus[i]-cantusFirmus[i+1]), IRT_EQ, cf_m_intervals_brut[i]);
        abs(home, m_intervals_brut[i], m_intervals[i]);
    }
}

void link_P_cons_arrays(const Home &home, int size, IntVarArray hIntervalsCpCf, BoolVarArray Pcons){
    //works
    for(int i = 0; i<size; i++){
        rel(home, expr(home, hIntervalsCpCf[i]==0), BOT_OR, expr(home, hIntervalsCpCf[i]==7), Pcons[i]);
    }
}

void link_motions_arrays(const Home &home, int size, IntVarArray m_intervals_brut, IntVarArray cf_m_intervals_brut, IntVarArray motions,
     IntVarArray motions_cost, BoolVarArray isCFB, int con_motion_cost, int obl_motion_cost, int dir_motion_cost){
        for(int i = 0; i < size-1; i++){
            //direct motions help creation
            BoolVar both_up = expr(home, (m_intervals_brut[i]>0)&&(cf_m_intervals_brut[i]>0));
            BoolVar both_stay = expr(home, (m_intervals_brut[i]==0)&&(cf_m_intervals_brut[i]==0));
            BoolVar both_down = expr(home, (m_intervals_brut[i]<0)&&(cf_m_intervals_brut[i]<0));
            //oblique motions help creation
            BoolVar cf_stays_1 = expr(home, (m_intervals_brut[i]>0)&&(cf_m_intervals_brut[i]==0));
            BoolVar cf_stays_2 = expr(home, (m_intervals_brut[i]<0)&&(cf_m_intervals_brut[i]==0));
            BoolVar cp_stays_1 = expr(home, (m_intervals_brut[i]==0)&&(cf_m_intervals_brut[i]>0));
            BoolVar cp_stays_2 = expr(home, (m_intervals_brut[i]==0)&&(cf_m_intervals_brut[i]<0));
            //contrary motions help creation
            BoolVar cpd_cfu = expr(home, (m_intervals_brut[i]<0)&&(cf_m_intervals_brut[i]>0));
            BoolVar cpu_cfd = expr(home, (m_intervals_brut[i]>0)&&(cf_m_intervals_brut[i]<0));
            //direct constraints
            rel(home, ((both_up || both_stay || both_down) && (isCFB[i]==1)) >> (motions[i]==2));
            rel(home, ((both_up || both_stay || both_down) && (isCFB[i]==1)) >> (motions_cost[i]==dir_motion_cost));
            //oblique constraints
            rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (isCFB[i]==1)) >> (motions[i]==1));
            rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (isCFB[i]==1)) >> (motions_cost[i]==obl_motion_cost));
            //contrary constraints
            rel(home, ((cpd_cfu || cpu_cfd)&& (isCFB[i]==1)) >> (motions[i]==0));
            rel(home, ((cpd_cfu || cpu_cfd)&& (isCFB[i]==1)) >> (motions_cost[i]==con_motion_cost));
            //bass constraints
            rel(home, (isCFB[i]==0) >> (motions[i]==-1));
            rel(home, (isCFB[i]==0) >> (motions_cost[i]==0));
        }
     }

void perfect_consonance_constraints(const Home &home, int size, IntVarArray hIntervalsCpCf){
    //works
    dom(home, hIntervalsCpCf[0], perfect_consonance);
    dom(home, hIntervalsCpCf[size-1], perfect_consonance);
}

void imperfect_consonances_are_preferred(const Home &home, int size, IntVarArray hIntervalsCpCf, IntVarArray Pconscost, int costpcons){
    for(int i = 0; i < size; i++){
        rel(home, (hIntervalsCpCf[i]==UNISSON || hIntervalsCpCf[i]==PERFECT_FIFTH) >> (Pconscost[i]==costpcons));
        rel(home, (hIntervalsCpCf[i]!=UNISSON && hIntervalsCpCf[i]!=PERFECT_FIFTH) >> (Pconscost[i]==0));
    }
}

void key_tone_tuned_to_cantusfirmus(const Home &home, int size, BoolVarArray isCFB, IntVarArray hIntervalsCpCf){
    //work
    rel(home, (isCFB[0] == 0) >> (hIntervalsCpCf[0]==0));
    rel(home, (isCFB[size-1] == 0) >> (hIntervalsCpCf[size-1]==0));
}

void voices_cannot_play_same_note(const Home &home, int size, IntVarArray cp, vector<int> cantusFirmus){
    //works
    for(int i = 1; i < size-1; i++){
        rel(home, cp[i], IRT_NQ, cantusFirmus[i]);
    }
}

void penultimate_note_must_be_major_sixth_or_minor_third(const Home &home, int size, IntVarArray hIntervalsCpCf, BoolVarArray isCFB){
    //find better test case
    int p = size-1;
    //rel(home, (isCFB[p]==1) >> (hIntervalsCpCf[p]==MAJOR_SIXTH));
    //rel(home, (isCFB[p]==0) >> (hIntervalsCpCf[p]==MINOR_THIRD));
}

void no_tritonic_intervals(const Home &home, int size, IntVarArray m_intervals, int costtri, IntVarArray Mdegcost){
    for(int j = 0; j < size-1; j++){
        rel(home, (m_intervals[j]==6) >> (Mdegcost[j]==costtri));
        rel(home, (m_intervals[j]!=6) >> (Mdegcost[j]==0));
    }
}

void melodic_intervals_not_exceed_minor_sixth(const Home &home, int size, IntVarArray m_intervals){
    //works
    for(int j = 0; j < size-1; j++){
        rel(home, m_intervals[j], IRT_LQ, 8);
    }
}

void no_direct_perfect_consonance(const Home &home, int size, IntVarArray hIntervalsCpCf, IntVarArray motions){
    //works
    for(int j = 0; j < size-1; j++){
        rel(home, (hIntervalsCpCf[j]==0 || hIntervalsCpCf[j]==7 || hIntervalsCpCf[j+1]==0 || hIntervalsCpCf[j+1]==7) >> 
            (motions[j]!=2));
    }
}

void no_battuta(const Home &home, int size, IntVarArray motions, IntVarArray hIntervalsCpCf, IntVarArray m_intervals_brut, IntVarArray cf_m_intervals_brut,
    BoolVarArray isCFB){
    for(int j = 0; j < size-1; j++){
        rel(home, expr(home, !((hIntervalsCpCf[j+1]==0)&&(motions[j]==0)&&(m_intervals_brut[j]<-4)&&(isCFB[j]==1))));
        rel(home, expr(home, !((hIntervalsCpCf[j+1]==0)&&(motions[j]==0)&&(cf_m_intervals_brut[j]<-4)&&(isCFB[j]==0))));
    }
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