/*
 * State.cpp
 *
 *  Created on: 21 Mar 2017
 *      Author: Paula B. Bassily
 */

#include "State.h"

namespace std {

// static variables initialised out side its own class
int State::state_counter = 0;
State::State(int num) {
	// TODO Auto-generated constructor stub
	// increase the objects count
	State::state_counter++;
	this->num = num;
	this->terminal= num;
	this->input='\0';
	this->acceptance="";
}

State::~State() {
}

} /* namespace std */
