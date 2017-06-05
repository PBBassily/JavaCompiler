/*
 * State.h
 *
 *  Created on: 21 Mar 2017
 *      Author: Paula B. Bassily
 */

#ifndef STATE_H_
#define STATE_H_
#include <string>
#include <vector>
#include <iostream>
#include <map>
namespace std {

class State {
public:
	// global shared variable between all states
	static int state_counter;

	//

	// state number , may be used in NFA-DFA conversion
	int num;

	// if empty string, so that this state not acceptance
	// else, it is acceptance holding the name of accepted token
	// for example : acceptance = "id"
	string acceptance;

	// input type
	char input;

	// next node(s)
	vector<int> next;

    int terminal;

	State(int num);
	~State();
};

} /* namespace std */

#endif /* STATE_H_ */
