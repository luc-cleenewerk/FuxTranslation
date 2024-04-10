#include "headers/gecode_problem.hpp"
#include "headers/Utilities.hpp"

/***********************************************************************************************************************
 *                                          Problem class methods                                                      *
 ***********************************************************************************************************************/

/**
 * Constructor
 * @todo Modify this constructor depending on your problem. This constructor is where the problem is defined
 * @todo (variables, constraints, branching, ...)
 * @param cf the cantus firmus notes 
 * @param sp an array of the species of the different voices (0 if cantus firmus, 1-5 for counterpoints)
 */
Problem::Problem(vector<int> cf, vector<int> sp) {

    // size = s;
    // lower_bound_domain = l;
    // upper_bound_domain = u;
    // species = sp;
    cantusFirmus = cf;
    species_list = sp;
    n_measures = cantusFirmus.size();
    n_voices   = species_list.size();
    size = sizeof(cf);

    // variable initialization todo depends on the species
    // cp = IntVarArray(*this, size, l, u);
    for (int s : species_list)
    {
        PartClass *tmp = new PartClass(n_measures, s);
        voices.push_back(tmp);
    }
    

    //constraints todo depends on the cantus firmus
    distinct(*this, voices.at(1)->get_notes()[0]);

    //branching
    // branch(*this, cp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());

    for (PartClass *partClass : voices) {
        if (partClass->species != 0){ // not cantus firmus
            for (IntVarArray notesArray : partClass->get_notes()) {
                branch(*this, notesArray, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
            }
        }
    }   

    // writeToLogFile(message.c_str());
}

/**
 * Copy constructor
 * @param s an instance of the Problem class
 * @todo modify this copy constructor to also copy any additional attributes you add to the class
 */
Problem::Problem(Problem& s): Space(s){
    //IntVars update
    cantusFirmus = s.cantusFirmus;
    species_list = s.species_list;
    
    // do we need to reinstantiate all variables? 

    // cp.update(*this, s.cp);
    
    for (PartClass *partClass : s.voices) {
        if (partClass->species != 0){ // not cantus firmus
            for (IntVarArray notesArray : partClass->get_notes()) {
                notesArray.update(*this, notesArray);
            }
        }
    }   
}

// /**
//  * Returns the size of the problem
//  * @return an integer representing the size of the vars array
//  */
int Problem::getSize(){
    string message = "getSize function called. size = " + to_string(size) + "\n";
    writeToLogFile(message.c_str());
    return size;
}

// /**
//  * Returns the values taken by the variables vars in a solution
//  * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
//  * @return an array of integers representing the values of the variables in a solution
//  */
int* Problem::return_solution(){
    string message = "return_solution method. Solution : [";
    int* solution = new int[999];
    int n = 0;
    for(int i = 0; i < voices.size(); i++){
        for(int j = 0; j < voices.at(i)->get_notes().size(); j++){
            IntVarArray tmp = voices.at(i)->get_notes()[j];
            for(IntVar var : tmp){
                for(int k = var.min(); k <= var.max(); k++){
                    solution[n] = k;
                    message += to_string(solution[i]) + " ";
                    n++;
                }
            }
        }
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

// /**
//  * Constrain method for bab search
//  * @todo modify this function if you want to use branch and bound
//  * @param _b a solution to the problem from which we wish to add a constraint for the next solutions
//  */
void Problem::constrain(const Space& _b) {
    const Problem &b = static_cast<const Problem &>(_b);
    //rel(*this, cp, IRT_GQ, 2);
}

// /**
//  * Prints the solution in the console
//  */
void Problem::print_solution(){
    for(int i = 0; i < voices.size(); i++){
        for(int j = 0; j < voices.at(i)->get_notes().size(); j++){
            cout << voices.at(i)->get_notes()[j] << " ";
        }
    }
    cout << endl;
}

// /**
//  * toString method
//  * @return a string representation of the current instance of the Problem class.
//  * Right now, it returns a string "Problem object. size = <size>"
//  * If a variable is not assigned when this function is called, it writes <not assigned> instead of the value
//  * @todo modify this method to also print any additional attributes you add to the class
//  */
// string Problem::toString(){
//     string message = "Problem object. \n";
//     message += "size = " + to_string(size) + "\n" + "lower bound for the domain : " +
//             to_string(lower_bound_domain) + "\n" + "upper bound for the domain : " + to_string(upper_bound_domain)
//              + "\n" + "current values for vars: [";
//     for(int i = 0; i < size; i++){
//         if (cp[i].assigned())
//             message += to_string(cp[i].val()) + " ";
//         else
//             message += "<not assigned> ";
//     }
//     message += "]\n\n";
//     writeToLogFile(message.c_str());
//     return message;
// }

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
    //message += sol_space->toString();
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