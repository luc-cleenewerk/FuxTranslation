#include "headers/PartClass.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    // int size = 3;
    // int upper_bound_domain = 5;
    // int lower_bound_domain = 1;
    // int species = 0;

    vector<int> cantusFirmus = {1, 2};
    vector<int> speciesList = {0,1};

    // create a new problem
    //Problem* p = new Problem(cantusFirmus, speciesList);

    PartClass* p = new PartClass(2, 1, {63,63});

    // create a new search engine
    Search::Base<PartClass>* e = make_solver(p, bab_solver);
    delete p;

    int nb_sol = 0;

    while(PartClass * pb = get_next_solution_space(e)){
        nb_sol++;
        cout << "Solution " << nb_sol << ": " << endl;
        pb->print_solution();
        delete pb;
    }

    cout << "No (more) solutions." << endl;
    return 0;
}