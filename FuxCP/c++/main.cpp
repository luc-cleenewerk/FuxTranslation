#include "headers/gecode_problem.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    int size = 9;
    int upper_bound_domain = 127;
    int lower_bound_domain = 1;
    int species = 0;
    int pconscst = 32;
    int tricst = 64;
    int con = 1;
    int obl = 2;
    int dir = 3;
    int var_cost = 1;
    int tri = 1;
    vector<int> cantusFirmus = {60, 62, 65, 64, 67, 65, 64, 62, 60};
    vector<int> speciesList = {1, 1};
    vector<int> voice_types = {3, 1};
    int t_off = 0;
    vector<int> scale = {0,2,4,5,7,9,11,12,14,16,17,19,21,23,24,26,28,29,31,33,35,36,38,40,41,43,45,47,48,50,52,53,55,57,59,60,62,64,65,67,69,71,72,74,76,77,79,81,83,84,86,88,89,91,93,95,96,98,100,101,103,105,107,108,110,112,113,115,117,119,120,122,124,125,127};
    vector<int> b_scale = {0,5,9,11,12,17,21,23,24,29,33,35,36,41,45,47,48,53,57,59,60,65,69,71,72,77,81,83,84,89,93,95,96,101,105,107,108,113,117,119,120,125};
    vector<int> c_scale;
    for(int i = 0; i <= 127; i++){
        c_scale.push_back(i);
    }
    int b_mode = 0;
    // create a new problem
    Problem* p = new Problem(size, lower_bound_domain, upper_bound_domain, species, cantusFirmus, pconscst, tricst, speciesList, con, obl, dir, 
        var_cost, voice_types, t_off, scale, b_scale, b_mode, tri);

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

