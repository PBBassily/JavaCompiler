#ifndef INPUTPARSER_H
#define INPUTPARSER_H
#include "phase2.h"

using namespace std;
class InputParser
{
    public:
        // elements scaned from productions file
        vector<struct PR_ELEM> elements ;
        // each NT element will have its production rule by indeces
        map<int , vector<vector<int>>> prod_rules_indexes;
        // each NT element will have its production rule
        map<string , vector<vector<string>>> prod_rules;
        // handle flow of parsing
        void parse_rules();
        InputParser();
        virtual ~InputParser();

    private:
    // global counter for elements
    int elem_count;
    // used to map each element to its index
    map<string,int> elem_index_map;
    // read rules
    void read_file();
    // init elem struct
    void init_elem(string str);
    // init production rules
    void init_prod();
    // print for check
    void check();
};

#endif // INPUTPARSER_H
