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

    PartClass *low = new PartClass(12);

    low->set_notes(1);

    vector<IntVarArray> note = low->get_notes();

    for(int i = 0; i < note.size(); i++){
        std::cout << intVarArray_to_string(note[i]);
        std::cout << endl;
    }

    FirstSpecies *first = new FirstSpecies(4, low);

    return 0;

}
