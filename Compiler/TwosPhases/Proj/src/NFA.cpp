/*
 * NFA.cpp
 *
 *  Created on: 22 Mar 2017
 *      Author: Paula B. Bassily
 */

#include "NFA.h"
#include "DFA.h"
#include <vector>

namespace std
{

/**
  *   attaches two states s1 and s2 with in transition
  */
void NFA::attach_states_on(int s1, int s2, char in)
{
    State st1=all_nodes[s1];
    st1.input = in;
    st1.next.push_back(s2);
    all_nodes[s1]=st1;
}

/**
  * create state
  */
int NFA::create_state()
{
    State s(state_num);
    all_nodes.push_back(s);
    state_num++;
    return s.num;
}

/**
  *  handle '|'
  */
void NFA::nfa_or()
{
    int start = create_state();
    int end = create_state();

    int s1 = input_stack.top();
    input_stack.pop();

    int s2 = input_stack.top();
    input_stack.pop();

    State st1 = all_nodes[s1];
    State st2 = all_nodes[s2];
    attach_states_on(st1.terminal, end, ' ');
    attach_states_on(st2.terminal, end, ' ');

    attach_states_on(start,s1,' ');
    attach_states_on(start,s2,' ');

    update_terminal(start,end);

    input_stack.push(start);
}

/**
  *  handle input
  */
void NFA::nfa_input(char in)
{
    int s1 = create_state();
    int  s2 = create_state();
    attach_states_on(s1, s2, in);
    update_terminal(s1,s2);
    input_stack.push(s1);
}

/**
  *  handle concatenation
  */
void NFA::nfa_and()
{

    int s2 = input_stack.top();
    input_stack.pop();
    State st2 = all_nodes[s2];

    int  s1 = input_stack.top();
    State st1 = all_nodes[s1];

    attach_states_on(st1.terminal,s2,' ');
    update_terminal(s1,s2);

}

/**
  *  handle astrick
  */
void NFA::nfa_astrick()
{
    int start = create_state();
    int end = create_state();

    int s1 = input_stack.top();
    input_stack.pop();
    State st1 = all_nodes[s1];

    attach_states_on(start, s1, ' ');
    attach_states_on(st1.terminal,end,' ');
    attach_states_on(end,s1,' ');
    attach_states_on(start,end,' ');

    update_terminal(start,end);

    input_stack.push(start);
}

/**
  *  handle plus
  */
void NFA::nfa_plus()
{
    int start = create_state();
    int end = create_state();

    int s1 = input_stack.top();
    input_stack.pop();
    State st1 = all_nodes[s1];

    attach_states_on(start, s1, ' ');
    attach_states_on(st1.terminal,end,' ');
    attach_states_on(end,s1,' ');

    update_terminal(start,end);

    input_stack.push(start);
}

/**
  *  get the acceptance state of the s state
  */
void NFA::get_acceptance(int s)
{
    State real_state=all_nodes[s];
    if(real_state.next.empty())
    {
        return ;
    }
    for(int i=0; i<real_state.next.size(); i++)
    {
        int child = real_state.next[i];
        get_acceptance(child);
    }
}


void NFA::nfa_check()
{
    int start= input_stack.top();
    State st = all_nodes[start];
    State terminal = all_nodes[st.terminal];
}

/**
  *  set the acceptance state
  */
void NFA::nfa_accpetance(string token)
{
    int start = input_stack.top();
    State st = all_nodes[start];
    State terminal = all_nodes[st.terminal];
    terminal.acceptance=token;
    all_nodes[st.terminal]=terminal;
}

/**
  *  update the terminal state
  */
void NFA::update_terminal(int state, int state_of_terminal)
{
    State st1=all_nodes[state];
    State st2=all_nodes[state_of_terminal];
    st1.terminal=st2.terminal;
    all_nodes[state]=st1;
}

/**
  *  build the NFA
  */
void NFA::build_nfa(string token, string experssion)
{
    for (string::iterator it = experssion.begin(); it != experssion.end(); it++)
    {
        switch (*it)
        {
        case '*':
            // call NFA kleene closure creator
            nfa_astrick();
            break;
        case '+':
            // call NFA + closure creator
            nfa_plus();
            break;
        case '^':
            // call NFA concatenation creator
            nfa_and();
            break;
        case '|':
            // call NFA OR creator
            nfa_or();
            break;
        case '\\':
            // call NFA OR creator
            it++;
        default :
            nfa_input(*it);
        }
    }
    nfa_accpetance(token);
    nfa_check();

}

/**
  *  combine all NFA states to one combined NFA
  */
int NFA::build_combined_nfa()
{
    int el7azina = create_state();
    while(!input_stack.empty())
    {
        attach_states_on(el7azina,input_stack.top(),' ');
        input_stack.pop();
    }
    return el7azina;
}

NFA::NFA()
{
    state_num = 0;
}

NFA::~NFA()
{

}

}
