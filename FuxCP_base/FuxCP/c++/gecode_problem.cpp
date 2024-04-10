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
Problem::Problem(vector<int> cf, vector<int> sp) {
    string message = "WSpace object created. ";
    // size = s;
    // lower_bound_domain = l;
    // upper_bound_domain = u;
    // species = sp;


    cantusFirmus = cf;
    speciesList = sp;
    n_measures = cantusFirmus.size();
    n_voices   = speciesList.size();



    for (int i = 0; i < n_voices; i++)
    {
        IntVarArray cpTemp = IntVarArray(*this, n_measures, 1, 5);    // use 1, 5 for now. update later (create range)
        counterpoints.push_back(cpTemp);
    }
    

    // variable initialization todo depends on the species
    // cp = IntVarArray(*this, size, l, u);
    // cp2 = IntVarArray(*this, size, l, u);

    // counterpoints.push_back(cp);
    // counterpoints.push_back(cp2);

    std::cout << counterpoints.size() << std::endl;
    std::cout << counterpoints[0] << std::endl;

    //constraints todo depends on the cantus firmus
    distinct(*this, counterpoints[0]);
    distinct(*this, counterpoints[1]);

    //branching
    branch(*this, counterpoints[0], INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, counterpoints[1], INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    writeToLogFile(message.c_str());
}

/**
 * Copy constructor
 * @param s an instance of the Problem class
 * @todo modify this copy constructor to also copy any additional attributes you add to the class
 */
Problem::Problem(Problem& s): Space(s){
    //IntVars update
    cantusFirmus = s.cantusFirmus;
    speciesList = s.speciesList;
    n_measures = s.n_measures;
    n_voices = s.n_voices;

    // The segfault was there because the counterpoints[] array was not initialized with IntVarArrays in the copy.
    // cp = IntVarArray(*this, size, lower_bound_domain, upper_bound_domain);
    // cp2 = IntVarArray(*this, size, lower_bound_domain, upper_bound_domain);
    // counterpoints.push_back(cp);
    // counterpoints.push_back(cp2);


    for (int i = 0; i < n_voices; i++)
    {
        IntVarArray cpTemp = IntVarArray(*this, n_measures, 1, 5);      // use 1, 5 for now. update later (create range)
        counterpoints.push_back(cpTemp);
    }

    // std::cout << counterpoints[0] << std::endl;

    counterpoints[0].update(*this, s.counterpoints[0]);
    counterpoints[1].update(*this, s.counterpoints[1]);
    // cp.update(*this, s.cp);
    // cp2.update(*this, s.cp2);
}

/**
 * Returns the size of the problem
 * @return an integer representing the size of the vars array
 */
int Problem::getSize(){
    string message = "getSize function called. size = " + to_string(n_measures) + "\n";
    writeToLogFile(message.c_str());
    return n_measures;
}

/**
 * Returns the values taken by the variables vars in a solution
 * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
 * @return an array of integers representing the values of the variables in a solution
 */
int* Problem::return_solution(){
    string message = "return_solution method. Solution : [";
    int* solution = new int[n_measures];
    for(int i = 0; i < n_measures; i++){
        solution[i] = counterpoints[0][i].val();       // TODO : modify!!
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
    // rel(*this, cp, IRT_GQ, 2);
}

/**
 * Prints the solution in the console
 */
void Problem::print_solution(){
    for(int i = 0; i < n_measures; i++){
        cout << counterpoints[0][i].val() << " ";
        cout << counterpoints[1][i].val() << " ";
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
    message += "size = " + to_string(n_measures) + "\n" + "lower bound for the domain : " +
            to_string(1) + "\n" + "upper bound for the domain : " + to_string(5)               // 1 and 5 for now, change later
             + "\n" + "current values for vars: [";
    for(int i = 0; i < n_measures; i++){
        if (counterpoints[0][i].assigned())
            message += to_string(counterpoints[0][i].val()) + " ";      // TODO : MODIFY!!
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