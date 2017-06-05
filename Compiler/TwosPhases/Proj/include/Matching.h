#ifndef MATCHING_H
#define MATCHING_H

#include "phase2.h"

using
namespace std;
class Matching
{
    public:
         // Methods
        void get_next_token();
        void  match_input();
        vector <int>refresh_stack();
        void write_file();

        // constractor
        Matching(map<pair< int, string> , vector<int> > parsing_table,vector<struct PR_ELEM> elements );
        virtual ~Matching();


    private:
    // Variables
    vector<struct PR_ELEM> elements ;
    map<pair< int, string> , vector<int> > parsing_table;
    vector <pair<string  ,vector <int> >> derivatins_steps;
    vector <string> tokens;
    stack  <int >  rules ;
};

#endif // MATCHING_H

