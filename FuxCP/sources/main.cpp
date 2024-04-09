#include <gecode/gist.hh>
#include "../headers/1st-ctp.hpp"
#include <iostream>

using namespace Gecode;
using namespace Gecode::Search;

//these two functions can go in another file
/*Base<FirstSpecies> *make_solver(FirstSpecies *pb) {
    
    Search::Options opts;

    return new BAB<FirstSpecies>(pb, opts);
}

FirstSpecies* get_next_solution(Search::Base<FirstSpecies>* solver){
    FirstSpecies* next = solver->next();
    if(next==nullptr){
        return nullptr;
    }
    return next;
}*/


int main(int argc, char* argv[]) {

    PartClass *low = new PartClass(4);

    low->set_notes(65);
    low->set_h_intervals(4);

    vector<PartClass *> parts_upper = {low};

    PartClass *cf = new PartClass(4);
    cf->set_h_intervals(4);
    cf->set_notes(66);
    std::cout << low->h_intervals[0];
    std::cout << low->notes[0];
    std::cout << endl;

    FirstSpecies *first = new FirstSpecies(4, low, cf , 1, parts_upper);

    return 0;

}
