/*
 * NFA.h
 *
 *  Created on: 22 Mar 2017
 *      Author: Paula B. Bassily
 */

#ifndef NFA_H_
#define NFA_H_
#include "State.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include "State.h"
#include "DFA.h"

namespace std {

class NFA {

public:
	// map between state number and the state itself
	vector<State>all_nodes;
	// used in evaluating the REs
	stack<int> input_stack;

	// global counter to states created
	int state_num ;

	//DFA dfa;//////////edit

	/**
	 *  s1 goes to s2 on "in" input
	 *  @return s1
	 */
	void attach_states_on(int s1, int s2, char in);

	/**
	 * creates new state
	 * gives it state number
	 * adds it to the all_nodes map
	 * @return it
	 */
	int create_state() ;

	/**
	 * creates two states
	 * attach them on "in" input
	 * pushes first state to the stack
	 */
	void nfa_input(char in);

	/**
	 * pops two states from stack s1 ,s2
	 * creates new state s
	 * attach s to s1 & s to s2
	 * push s to the stack
	 */
	void nfa_or();
	void nfa_and();
	void nfa_astrick();
	void nfa_plus();
	void nfa_accpetance(string token);
	void nfa_check();
	void get_acceptance(int s);
	void update_terminal(int state, int terminal);

public :
    void build_nfa(string token, string experssion);
	int build_combined_nfa();

	NFA();

	~NFA();
};

} /* namespace std */

#endif /* NFA_H_ */
