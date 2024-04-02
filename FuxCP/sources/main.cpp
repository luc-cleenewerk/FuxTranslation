#include <gecode/gist.hh>
#include "../headers/1st-ctp.hpp"
#include <iostream>

using namespace Gecode;

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

    auto *pb = new FirstSpecies();

    vector<int> cp_range = {-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12};
    vector<int> cp_r = {sizeof(cp_range), -6, 12};
    vector<int> cp_domain = {2,1,2};
    vector<int> chrom_cp_dom = {1,1,1};
    vector<int> ext_cp_dom = {4,1,5};
    vector<int> off_dom = {1,2,2};
    int voice_type = 1;
    int species = 1;

    FirstSpecies *first = new FirstSpecies(cp_r, cp_domain, chrom_cp_dom, ext_cp_dom, off_dom, voice_type, species);
    Search::Options opts;
    opts.threads = 1;
    opts.stop = Search::Stop::time(600000); // stop after 120 seconds
    opts.cutoff = Search::Cutoff::merge(
            Search::Cutoff::linear(2*(cp_range.size()+cp_domain.size()+chrom_cp_dom.size()+ext_cp_dom.size()+off_dom.size())),
            Search::Cutoff::geometric((4*(cp_range.size()+cp_domain.size()+chrom_cp_dom.size()+ext_cp_dom.size()+off_dom.size()))^2, 2));
    opts.nogoods_limit = (cp_range.size()+cp_domain.size()+chrom_cp_dom.size()+ext_cp_dom.size()+off_dom.size()) * 4 * 4;

     
    std:cout << "Compiling...";

    Search::Base<FirstSpecies>* e = make_solver(first);

    delete first;

    int nb_sol = 0;

    while(FirstSpecies * pb = get_next_solution(e)){
        nb_sol++;
        cout << "Solution " << nb_sol << ": " << endl;
        pb->print_solution();
        delete pb;
    }
    cout << "No (more) solutions." << endl;

    return 0;

}
