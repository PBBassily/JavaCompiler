#ifndef LL_PRODUCTIONS_H
#define LL_PRODUCTIONS_H

#include "phase2.h"

using namespace std;
class LL_Productions{
    public:
        LL_Productions();
        // handles flow of program
        void get_bonus();
        virtual ~LL_Productions();

    protected:

    private:
    // all rules container
    vector<vector<vector<string>>> all_rules;
    // used in the left factor elemination
    int factor_index;
    // check for left recurion in a given rule
    bool check_left_rec(vector<vector<string>> rule);
    // check for left factor in a given rule
    bool check_left_fac(vector<vector<string>> rule);
    // removes left recurion in a given rule
    void remove_left_rec(vector<vector<string>> rule);
    // removes left factor in a given rule
    void remove_left_fac(vector<vector<string>> rule);
    // handle above operations for each rule
    void handle_rule(vector<vector<string>> rule);

    void read_rules();
    void write_rules();

};

#endif // LL_PRODUCTIONS_H
