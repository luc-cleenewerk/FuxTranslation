#include "headers/gecode_problem.hpp"
#include "headers/Utilities.hpp"

/**
 * Imagine this represents now the first species problem.
 * If we extend this to the next species, we add more of these classes, like to a list where the first entry is this class,
 * the next entry the next class and so on.
 * The next classes could inherit from this class.
 * Still in the thinking portion.
*/

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
Problem::Problem(int s, int l, int u, int sp, vector<int> cf) {
    string message = "WSpace object created. ";
    size = s;
    lower_bound_domain = l;
    upper_bound_domain = u;
    species = sp;
    cantusFirmus = cf;
    
    // variable initialization todo depends on the species
    cp = IntVarArray(*this, size, l, u);

    h_intervals = IntVarArray(*this, size, 0, 11);

    m_intervals = IntVarArray(*this, size-1, 0, 12);
    m_intervals_brut = IntVarArray(*this, size-1, -12, 12);
    
    //motions = IntVarArray(*this, size-1, -1, 2);

    //motions_cost = IntVarArray();

    is_lowest = BoolVarArray(*this, size, 0, 1);
    cf_lowest = BoolVarArray(*this, size, 0, 1);

    is_cfb = BoolVarArray(*this, size, 0, 1);

    fifth_cost = IntVarArray(*this, size-1, 0, 1);
    octave_cost = IntVarArray(*this, size-1, 0, 1);

    //constraints todo depends on the cantus firmus
    //distinct(*this, cp);

    //create_h_intervals

    for(int i = 0; i < size; i++){
        IntVar t1 = expr(*this, cp[i]-cf[i]);
        IntVar t2 = IntVar(*this, 0, 127);
        abs(*this, t1, t2);
        mod(*this, t2, IntVar(*this, 12, 12), h_intervals[i]);
    }

    //create melodic intervals

    for(int i = 0; i < size-1; i++){
        rel(*this, expr(*this, cp[i]-cp[i+1]), IRT_EQ, m_intervals_brut[i]);
        abs(*this, expr(*this, cp[i]-cp[i+1]), m_intervals[i]);
    }

    //create cfb array

    for(int i = 0; i < size; i++){
        rel(*this, cp[i], IRT_GQ, cf[i], Reify(is_cfb[i], RM_EQV));
    }

    //create cf lowest
    for(int j = 0; j < size; j++){
        int low_note = get_lowest_stratum_note(j);
        if(cantusFirmus[j]==low_note){
            rel(*this, cf_lowest[j], IRT_EQ, 1);
        } else {
            rel(*this, cf_lowest[j], IRT_EQ, 0);
        }
    }
    
    //create is lowest
    for(int j = 0; j < size; j++){
        int low_note = get_lowest_stratum_note(j);
        cout << "LOW NOTE : " << low_note << endl;
        if(cp[j].assigned() && cf_lowest[j].assigned()){
            if((cp[j].val()==low_note) && cf_lowest[j].val()!=1){
                rel(*this, is_lowest[j], IRT_EQ, 1);
            } else {
                rel(*this, is_lowest[j], IRT_EQ, 0);
            }
        }
    }

    //create fifth cost
    for(int i = 0; i < size-1; i++){
        BoolVar b = BoolVar(*this, 0, 1);
        rel(*this, h_intervals[i], IRT_EQ, 7, Reify(b));
        ite(*this, b, IntVar(*this, 1, 1), IntVar(*this, 0, 0), fifth_cost[i]);
    }

    //CONSTRAINT 1, 2 et 3
    for(int i = 0; i < size; i++){
        if(i==0 || i==size-1){
            member(*this, CONS_P, h_intervals[i]);
        } else {
            member(*this, ALL_CONS, h_intervals[i]);
        }
    }

    //CONSTRAINT 4
    if(is_cfb[0].assigned()){
        if(is_cfb[0].val()==0){
            rel(*this, h_intervals[0], IRT_EQ, 0);
        }
    }

    //CONSTRAINT 4
    if(is_cfb[size-1].assigned()){
        if(is_cfb[size-1].val()==0){
            rel(*this, h_intervals[size-1], IRT_EQ, 0);
        }
    }

    //CONSTRAINT 5
    for(int i = 1; i < size-1; i++){
        rel(*this, cp[i], IRT_NQ, cf[i]);
    }

    //CONSTRAINT 6
    for(int j = 0; j < size; j++){

    }

    //CONSTRAINT 7
    int p = size-1;
    if(is_cfb[p].assigned()){
        int pitch = 3;
        if(is_cfb[p].val()==1){
            pitch = 9;
        }
        rel(*this, h_intervals[p], IRT_EQ, pitch);
    }

    // ===================
    // MELODIC CONSTRAINTS
    // ===================

    //CONSTRAINT 2

    for(int j = 0; j < size-1; j++){
        rel(*this, m_intervals[j], IRT_LQ, 8);
    }

    // ==================
    // MOTION CONSTRAINTS
    // ==================

    //CONSTRAINT 1 : NO PREFECT CONSONANCES THROUGH DIRECT MOTIONS
    for(int j = 0; j < size-1; j++){
        if(h_intervals[j].assigned() && motions[j].assigned()){
            if(h_intervals[j].val()==7 || h_intervals[j].val()==0){
                rel(*this, motions[j], IRT_NQ, 2);
            }
        }
    }

    //branching
    branch(*this, cp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, h_intervals, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, m_intervals, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, m_intervals_brut, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    for(BoolVar b : is_cfb){
        branch(*this, b, BOOL_VAL_MIN());
    }
    for(BoolVar b : cf_lowest){
        branch(*this, b, BOOL_VAL_MIN());
    }
    for(BoolVar b : is_lowest){
        branch(*this, b, BOOL_VAL_MIN());
    }
    branch(*this, fifth_cost, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    writeToLogFile(message.c_str());
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
    cp = s.cp;
    cantusFirmus = s.cantusFirmus;
    h_intervals = s.h_intervals;
    m_intervals = s.m_intervals;
    m_intervals_brut = s.m_intervals_brut;
    is_cfb = s.is_cfb;
    cf_lowest = s.cf_lowest;
    is_lowest = s.is_lowest;
    fifth_cost = s.fifth_cost;

    cp.update(*this, s.cp);
    h_intervals.update(*this, s.h_intervals);
    m_intervals.update(*this, s.m_intervals);
    m_intervals_brut.update(*this, s.m_intervals_brut);
    is_cfb.update(*this, s.is_cfb);
    cf_lowest.update(*this, s.cf_lowest);
    is_lowest.update(*this, s.is_lowest);
    fifth_cost.update(*this, s.fifth_cost);
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
 * Returns the values taken by the variables vars in a solution
 * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
 * @return an array of integers representing the values of the variables in a solution
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
}

/**
 * Prints the solution in the console
 */
void Problem::print_solution(){
    cout << "CF NOTES ";
    for(int i = 0; i < cantusFirmus.size(); i++){
        cout << cantusFirmus.at(i) << " ";
    }
    cout << endl;
    cout << "CP NOTES ";
    for(int i = 0; i < size; i++){
        cout << cp[i].val() << " ";
    }
    cout << endl;
    /*cout << "H INTERVALS ";
    for(int i = 0; i < size; i++){
        cout << h_intervals[i].val() << " ";
    }
    cout << endl;
    cout << "IS CFB ";
    for(int i = 0; i < size; i++){
        cout << is_cfb[i].val() << " ";
    }
    cout << endl;
    cout << "M INTERVALS ";
    for(int i = 0; i < size-1; i++){
        cout << m_intervals[i].val() << " ";
    }
    cout << endl;
    cout << "M INTERVALS BRUT ";
    for(int i = 0; i < size-1; i++){
        cout << m_intervals_brut[i].val() << " ";
    }
    cout << endl;*/
    cout << "CF LOWEST ";
    for(int i = 0; i < size; i++){
        cout << cf_lowest[i].val() << " ";
    }
    cout << endl;
    cout << "IS LOWEST ";
    for(int i = 0; i < size; i++){
        cout << is_lowest[i].val() << " ";
    }
    cout << endl;
}

/**
 * toString method
 * @return a string representation of the current instance of the Problem class.
 * Right now, it returns a string "Problem object. size = <size>"
 * If a variable is not assigned when this function is called, it writes <not assigned> instead of the value
 * @todo modify this method to also print any additional attributes you add to the class
 */
string Problem::toString(){
    string message = "Problem object. \n";
    message += "size = " + to_string(size) + "\n" + "lower bound for the domain : " +
            to_string(lower_bound_domain) + "\n" + "upper bound for the domain : " + to_string(upper_bound_domain)
             + "\n" + "current values for vars: [";
    for(int i = 0; i < size; i++){
        if (cp[i].assigned())
            message += to_string(cp[i].val()) + " ";
        else
            message += "<not assigned> ";
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

int Problem::get_lowest_stratum_note(int index){
    if(cp[index].assigned()){
        if(cp[index].val()<cantusFirmus[index]){
            return cp[index].val();
        } else {
            return cantusFirmus[index];
        }
    } else{
        return -1;
    }
}