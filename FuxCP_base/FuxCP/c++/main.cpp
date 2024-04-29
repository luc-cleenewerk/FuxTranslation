#include "headers/gecode_problem.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    int size = 12;
    int upper_bound_domain = 127;
    int lower_bound_domain = 1;
    int species = 0;
    int pconscst = 32;
    int tricst = 64;
    int con = 1;
    int obl = 2;
    int dir = 3;
    int var_cost = 1;
    vector<int> cantusFirmus = {57, 60, 59, 62, 60, 64, 65, 64, 62, 60, 59, 57};    // Thibault's first species cantus firmus
    vector<int> speciesList = {1, 1};
    // create a new problem
    Problem* p = new Problem(size, lower_bound_domain, upper_bound_domain, species, cantusFirmus, pconscst, tricst, speciesList, con, obl, dir, var_cost);

    // create a new search engine
    Search::Base<Problem>* e = make_solver(p, bab_solver);
    delete p;

    int nb_sol = 0;

    while(Problem * pb = get_next_solution_space(e)){
        nb_sol++;
        cout << "Solution " << nb_sol << ": " << endl;
        cout << pb->toString() << endl;
        delete pb;
    }
    cout << "No (more) solutions." << endl;
    return 0;
}

