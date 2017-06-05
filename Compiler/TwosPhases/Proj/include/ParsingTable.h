#ifndef PARSINGTABLE_H
#define PARSINGTABLE_H

#include "phase2.h"

using namespace std;
class ParsingTable
{
public:
    ParsingTable(vector<struct PR_ELEM> all_elem,map<int, vector<vector<int>>> prod_rules );
    virtual ~ParsingTable();

    vector<struct PR_ELEM> elements;             /*list for the structs of terminals and non terminals*/
    map<pair< int, string>, vector<int> > parsing_table;     /* parsing table */

    /* function for constructing the parsing table */
    void construct_parsing_table();

protected:

private:
    map<int, vector<vector<int>>> prod_rules;    /* production rules*/

    /* functions for computing the first */
    void construct_first();
    void get_first(int);

    /* functions for computing the follow */
    void fill_owners();
    void construct_follow();
    void get_follow(int NT_index);
    void insert_to_map(int NT_index,int ter_index);

    /* functions for printing the steps of constructing the parsing table */
    void print_first_or_follow(string);
    void print_table();
};

#endif // PARSINGTABLE_H
