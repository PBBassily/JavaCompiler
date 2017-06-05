#ifndef DFA_H_
#define DFA_H_
#include "NFA.h"
#include"State.h"
#include "Phase_one.h"


namespace std
{
class DFA
{
public:
    DFA(map<char,string>);
    ~DFA();

    struct DFA_STATE
    {
        int number;
        set<int> *combined_states;
        char *acceptance;
    };

    void fill_subset_construction(int node,vector<State> );
   // void dfa_set_spec_char(char *);

    vector< int * > subset_construction;
    vector<DFA_STATE*> dfa_mapping;


private:
    map <char,string> sp_char_map;
    vector<State> nfa_nodes;
    char **dfa_special_char_str;

    //--
    char dfa_special_char[5]= {'$','@','&','#','%'};
    //string input="abcefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*/+-;,(){}=!<>.";
    //--
    //char epsilon = ' ';

    void epsilon_combine(DFA_STATE *,State,set<int> *);
};


}

#endif // DFA_H
