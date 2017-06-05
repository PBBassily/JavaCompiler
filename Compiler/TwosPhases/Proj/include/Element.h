#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED

#include "phase2.h"

namespace std{

    struct PR_ELEM{
        string st;
        bool terminal; // true=terminal
        set <int> first;
        set <int> follow;
        vector<int> owners;
        bool first_has_epsilon = false;
    };


}

#endif // ELEMENT_H_INCLUDED
