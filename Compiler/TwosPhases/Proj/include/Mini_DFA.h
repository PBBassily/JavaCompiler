#include "DFA.h"
#include "Phase_one.h"
#include <map>
#ifndef MINI_DFA_H
#define MINI_DFA_H

namespace std{
class Mini_DFA
{
    public:
    ////////////// attributes/////////////////////
        map<string,int > group_setter;
        string input;
        vector< int * > subset_construction;
        vector<DFA::DFA_STATE*> states;
        vector<int> ith_state_group;
        vector<vector<int>> groups;
        vector<vector<int>> nxt_groups;
        vector<vector<int>> next_grp_ident;
        vector <bool> is_taken;
        map<pair<int,char>,DFA::DFA_STATE> dfa_network;

    ////////////// functions/////////////////////
    void set_initial_grp();
    void minimize();
    void get_mini_dfa();
    Mini_DFA(vector< int * > subset_construction,vector<DFA::DFA_STATE*> states);
    void write_trans_file();
    virtual ~Mini_DFA();

    private :

    bool need_more_grps;
    void generate_nxt_group(int i);
    void re_group();
    bool compare_nxt_grp_idents(int st1,int st2);



    protected:

    private:
};
}

#endif // MINI_DFA_H
